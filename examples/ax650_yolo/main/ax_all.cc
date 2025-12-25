#include <cstdio>
#include <cstring>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <opencv2/opencv.hpp>

#include "base/common.hpp"
#include "base/detection.hpp"
#include "base/pose.hpp"
#include "base/stackchan.hpp"
#include "middleware/io.hpp"
#include "utilities/cmdline.hpp"
#include "utilities/file.hpp"
#include "utilities/timer.hpp"

#include <ax_sys_api.h>
#include <ax_engine_api.h>

using namespace std;

const int DEFAULT_IMG_H = 320;
const int DEFAULT_IMG_W = 320;
const int HAND_IMG_H    = 224;
const int HAND_IMG_W    = 224;
const int PALM_IN_H     = 192;
const int PALM_IN_W     = 192;
const int QUEUE_SIZE    = 2;

// ====================== 帧队列类 ======================
class FrameQueue {
    std::queue<cv::Mat> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t max_size_;
    std::atomic<bool> stop_flag_;
public:
    FrameQueue(size_t max_size = QUEUE_SIZE) : max_size_(max_size), stop_flag_(false) {}
    void push(const cv::Mat &frame) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.size() >= max_size_ && !stop_flag_) queue_.pop();
        if (!stop_flag_) {
            queue_.push(frame.clone());
            cv_.notify_one();
        }
    }
    bool pop(cv::Mat &frame) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty() || stop_flag_; });
        if (queue_.empty()) return false;
        frame = queue_.front();
        queue_.pop();
        return true;
    }
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_flag_ = true;
        cv_.notify_all();
    }
};

// ====================== Engine Helper（复用 handle） ======================
struct AxEngineContext {
    AX_ENGINE_HANDLE         handle{nullptr};
    AX_ENGINE_IO_INFO_T     *io_info{nullptr};
    AX_ENGINE_IO_T           io_data{};
    bool                     inited{false};
};

static bool init_engine_once(AxEngineContext &ctx, const std::string &model_path) {
    if (ctx.inited) return true;

    // 1. init engine (只在第一次使用时调用；AX_SYS_Init 在 main 中统一)
    static bool s_engine_inited = false;
    if (!s_engine_inited) {
        AX_ENGINE_NPU_ATTR_T npu_attr;
        memset(&npu_attr, 0, sizeof(npu_attr));
        npu_attr.eHardMode = AX_ENGINE_VIRTUAL_NPU_DISABLE;
        auto ret = AX_ENGINE_Init(&npu_attr);
        if (0 != ret) {
            fprintf(stderr, "AX_ENGINE_Init failed, ret = 0x%x\n", ret);
            return false;
        }
        s_engine_inited = true;
    }

    // 2. read model
    std::vector<char> model_buffer;
    if (!utilities::read_file(model_path, model_buffer)) {
        fprintf(stderr, "Read AX-Engine model(%s) file failed.\n", model_path.c_str());
        return false;
    }

    // 3. create handle
    auto ret = AX_ENGINE_CreateHandle(&ctx.handle,
                                      model_buffer.data(),
                                      model_buffer.size());
    if (0 != ret) {
        fprintf(stderr, "AX_ENGINE_CreateHandle failed, ret = 0x%x\n", ret);
        return false;
    }

    // 4. create context
    ret = AX_ENGINE_CreateContext(ctx.handle);
    if (0 != ret) {
        fprintf(stderr, "AX_ENGINE_CreateContext failed, ret = 0x%x\n", ret);
        AX_ENGINE_DestroyHandle(ctx.handle);
        ctx.handle = nullptr;
        return false;
    }

    // 5. get io info
    ret = AX_ENGINE_GetIOInfo(ctx.handle, &ctx.io_info);
    if (0 != ret) {
        fprintf(stderr, "AX_ENGINE_GetIOInfo failed, ret = 0x%x\n", ret);
        AX_ENGINE_DestroyHandle(ctx.handle);
        ctx.handle = nullptr;
        return false;
    }

    // 6. alloc io
    memset(&ctx.io_data, 0, sizeof(ctx.io_data));
    ret = middleware::prepare_io(ctx.io_info,
                                 &ctx.io_data,
                                 std::make_pair(AX_ENGINE_ABST_DEFAULT,
                                                AX_ENGINE_ABST_CACHED));
    if (0 != ret) {
        fprintf(stderr, "prepare_io failed, ret = 0x%x\n", ret);
        AX_ENGINE_DestroyHandle(ctx.handle);
        ctx.handle = nullptr;
        return false;
    }

    ctx.inited = true;
    return true;
}

static inline bool run_engine(AxEngineContext &ctx,
                              const std::vector<uint8_t> &data) {
    AX_S32 ret = middleware::push_input(data, &ctx.io_data, ctx.io_info);
    if (0 != ret) {
        fprintf(stderr, "push_input failed, ret = 0x%x\n", ret);
        return false;
    }
    ret = AX_ENGINE_RunSync(ctx.handle, &ctx.io_data);
    if (0 != ret) {
        fprintf(stderr, "AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
        return false;
    }
    return true;
}

// ====================== Face 检测 ======================
namespace task_face {
using namespace detection;

const char *CLASS_NAMES[]  = {"face"};
int   NUM_CLASS            = 1;
const float PROB_THRESHOLD = 0.45f;
const float NMS_THRESHOLD  = 0.45f;

static AxEngineContext ctx;
std::string model_file;

void post_process(const cv::Mat &mat_in,
                  cv::Mat &mat_out,
                  int iw, int ih)
{
    std::vector<Object> proposals, objects;

    for (int i = 0; i < 3; ++i) {
        float *feat_ptr = (float *)ctx.io_data.pOutputs[i].pVirAddr;
        int stride      = (1 << i) * 8;
        generate_proposals_yolov8_native(stride,
                                         feat_ptr,
                                         PROB_THRESHOLD,
                                         proposals,
                                         iw,
                                         ih,
                                         NUM_CLASS);
    }

    get_out_bbox(proposals, objects, NMS_THRESHOLD,
                 ih, iw, mat_in.rows, mat_in.cols);

    mat_out = draw_objects(mat_in, objects, CLASS_NAMES,
                           "Face Detection", 1.0, 2);
    process_objects_for_servo(objects);
}

bool run(cv::Mat &mat, const std::vector<uint8_t> &data, int h, int w)
{
    if (!init_engine_once(ctx, model_file)) return false;
    if (!run_engine(ctx, data)) return false;
    post_process(mat, mat, w, h);
    return true;
}
} // namespace task_face

// ====================== Pose 姿态 ======================
namespace task_pose {
using namespace detection;

const char *CLASS_NAMES[] = {"person"};
const std::vector<std::vector<uint8_t>> KPS_COLORS = {
    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},
    {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},
    {255, 128, 0},  {51, 153, 255}, {51, 153, 255}, {51, 153, 255}, {51, 153, 255},
    {51, 153, 255}, {51, 153, 255}};
const std::vector<std::vector<uint8_t>> LIMB_COLORS = {
    {51, 153, 255}, {51, 153, 255}, {51, 153, 255}, {51, 153, 255},
    {255, 51, 255}, {255, 51, 255}, {255, 51, 255},
    {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},
    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},
    {0, 255, 0},    {0, 255, 0}};
const std::vector<std::vector<uint8_t>> SKELETON = {
    {16, 14}, {14, 12}, {17, 15}, {15, 13}, {12, 13},
    {6, 12},  {7, 13},  {6, 7},   {6, 8},   {7, 9},
    {8, 10},  {9, 11},  {2, 3},   {1, 2},   {1, 3},
    {2, 4},   {3, 5},   {4, 6},   {5, 7}};

int   NUM_CLASS            = 1;
int   NUM_POINT            = 17;
const float PROB_THRESHOLD = 0.45f;
const float NMS_THRESHOLD  = 0.45f;

static AxEngineContext ctx;
std::string model_file;

void post_process(const cv::Mat &mat_in,
                  cv::Mat &mat_out,
                  int iw, int ih)
{
    std::vector<Object> proposals, objects;

    float *out_ptr[3] = {
        (float *)ctx.io_data.pOutputs[0].pVirAddr,
        (float *)ctx.io_data.pOutputs[1].pVirAddr,
        (float *)ctx.io_data.pOutputs[2].pVirAddr};
    float *kps_ptr[3] = {
        (float *)ctx.io_data.pOutputs[3].pVirAddr,
        (float *)ctx.io_data.pOutputs[4].pVirAddr,
        (float *)ctx.io_data.pOutputs[5].pVirAddr};

    for (int i = 0; i < 3; ++i) {
        int stride = (1 << i) * 8;
        generate_proposals_yolov8_pose_native(stride,
                                              out_ptr[i],
                                              kps_ptr[i],
                                              PROB_THRESHOLD,
                                              proposals,
                                              iw,
                                              ih,
                                              NUM_POINT,
                                              NUM_CLASS);
    }

    get_out_bbox_kps(proposals, objects, NMS_THRESHOLD,
                     ih, iw, mat_in.rows, mat_in.cols);

    mat_out = draw_keypoints(mat_in, objects,
                             KPS_COLORS, LIMB_COLORS, SKELETON,
                             "Pose", 1.0, 2);
}

bool run(cv::Mat &mat, const std::vector<uint8_t> &data, int h, int w)
{
    if (!init_engine_once(ctx, model_file)) return false;
    if (!run_engine(ctx, data)) return false;
    post_process(mat, mat, w, h);
    return true;
}
} // namespace task_pose

// ====================== Palm + HandPose ======================
namespace task_hand {
using namespace detection;

const int HAND_JOINTS        = 21;
const float PROB_THRESHOLD   = 0.65f;
const float NMS_THRESHOLD    = 0.45f;
const int map_size[2]        = {24, 12};
const int strides[2]         = {8, 16};
const int anchor_size[2]     = {2, 6};
const float anchor_offset[2] = {0.5f, 0.5f};

static AxEngineContext palm_ctx;
static AxEngineContext hand_ctx;
std::string palm_model_file, hand_model_file;

bool init_palm() {
    return init_engine_once(palm_ctx, palm_model_file);
}
bool init_hand() {
    return init_engine_once(hand_ctx, hand_model_file);
}

bool run_hand_model(const std::vector<uint8_t> &data,
                    pose::ai_hand_parts_s &out_pose)
{
    if (!init_hand()) return false;

    AX_S32 ret = middleware::push_input(data, &hand_ctx.io_data, hand_ctx.io_info);
    if (0 != ret) {
        fprintf(stderr, "Hand push_input failed, ret = 0x%x\n", ret);
        return false;
    }
    ret = AX_ENGINE_RunSync(hand_ctx.handle, &hand_ctx.io_data);
    if (0 != ret) {
        fprintf(stderr, "Hand AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
        return false;
    }

    auto &info_point = hand_ctx.io_data.pOutputs[0];
    auto &info_score = hand_ctx.io_data.pOutputs[1];
    float *point_ptr = (float *)info_point.pVirAddr;
    float *score_ptr = (float *)info_score.pVirAddr;
    pose::post_process_hand(point_ptr, score_ptr,
                            out_pose, HAND_JOINTS,
                            HAND_IMG_H, HAND_IMG_W);
    return true;
}

void post_process_palm(cv::Mat &mat)
{
    std::vector<PalmObject> proposals, objects;

    auto bboxes_ptr = (float *)palm_ctx.io_data.pOutputs[0].pVirAddr;
    auto scores_ptr = (float *)palm_ctx.io_data.pOutputs[1].pVirAddr;

    float prob_threshold_unsigmoid =
        -1.0f * (float)std::log((1.0f / PROB_THRESHOLD) - 1.0f);

    generate_proposals_palm(proposals,
                            PROB_THRESHOLD,
                            PALM_IN_W,
                            PALM_IN_H,
                            scores_ptr,
                            bboxes_ptr,
                            2,
                            strides,
                            anchor_size,
                            anchor_offset,
                            map_size,
                            prob_threshold_unsigmoid);

    get_out_bbox_palm(proposals, objects, NMS_THRESHOLD,
                      PALM_IN_H, PALM_IN_W, mat.rows, mat.cols);

    cv::Mat mat_draw = mat;

    // 手势去抖
    static int last_gesture_id = -1;
    static int same_count      = 0;
    const int required_count   = 10;

    for (size_t i = 0; i < objects.size(); ++i) {
        cv::Mat hand_roi;
        cv::warpAffine(mat, hand_roi,
                       objects[i].affine_trans_mat,
                       cv::Size(HAND_IMG_W, HAND_IMG_H));

        std::vector<uint8_t> hand_image(HAND_IMG_H * HAND_IMG_W * 3);
        common::get_input_data_no_letterbox(hand_roi,
                                            hand_image,
                                            HAND_IMG_H,
                                            HAND_IMG_W,
                                            true);

        pose::ai_hand_parts_s hand_parts;
        if (!run_hand_model(hand_image, hand_parts)) {
            continue;
        }

        pose::draw_result_hand_on_image(mat_draw,
                                        hand_parts,
                                        HAND_JOINTS,
                                        objects[i].affine_trans_mat_inv);

        if (check_palm_objects_size(objects, 0.05, 0.05) == 0) {
            int gesture_id = classify_gesture(hand_parts);
            if (gesture_id != -1) {
                if (gesture_id == last_gesture_id) {
                    same_count++;
                } else {
                    same_count      = 1;
                    last_gesture_id = gesture_id;
                }

                if (same_count >= required_count) {
                    same_count = 0;
                    const std::string &label = gesture_defs[gesture_id].label;
                    if (label == "ok") {
                        send_motion("reverse");
                    } else if (label == "one") {
                        send_motion("shake");
                    } else if (label == "two") {
                        send_motion("nod");
                    } else if (label == "five") {
                        send_motion("forward");
                    } else if (label == "four") {
                        send_motion("photo");
                    }
                }
            }
        }
    }

    mat = draw_objects_palm(mat_draw, objects, "Palm detection");
}

bool run(cv::Mat &mat, const std::vector<uint8_t> &data)
{
    if (!init_palm()) return false;

    AX_S32 ret = middleware::push_input(data, &palm_ctx.io_data, palm_ctx.io_info);
    if (0 != ret) {
        fprintf(stderr, "Palm push_input failed, ret = 0x%x\n", ret);
        return false;
    }
    ret = AX_ENGINE_RunSync(palm_ctx.handle, &palm_ctx.io_data);
    if (0 != ret) {
        fprintf(stderr, "Palm AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
        return false;
    }

    post_process_palm(mat);
    return true;
}
} // namespace task_hand

// ====================== 采集线程 ======================
void captureFrames(cv::VideoCapture &cap, FrameQueue &fq, std::atomic<bool> &stop)
{
    cv::Mat frame;
    while (!stop) {
        cap >> frame;
        if (frame.empty()) {
            stop = true;
            break;
        }
        cv::flip(frame, frame, 1);
        fq.push(frame);
    }
    fq.stop();
}
void send_jpeg(void *buff, int size);
// ====================== 主程序 ======================
int main(int argc, char **argv)
{
    cmdline::parser cmd;
    cmd.add<std::string>("face_model", 'f', "face model(joint)", true, "");
    cmd.add<std::string>("pose_model", 'p', "pose model(joint)", true, "");
    cmd.add<std::string>("palm_model", 'm', "palm model(joint)", true, "");
    cmd.add<std::string>("hand_model", 'h', "hand model(joint)", true, "");
    cmd.add<std::string>("video",      'v', "video src",         true, "");
    cmd.parse_check(argc, argv);

    task_face::model_file      = cmd.get<std::string>("face_model");
    task_pose::model_file      = cmd.get<std::string>("pose_model");
    task_hand::palm_model_file = cmd.get<std::string>("palm_model");
    task_hand::hand_model_file = cmd.get<std::string>("hand_model");
    std::string video_src      = cmd.get<std::string>("video");

    // 打开摄像头 / 视频
    cv::VideoCapture cap;
    try {
        int idx = std::stoi(video_src);
        cap.open(idx, cv::CAP_V4L2);
    } catch (...) {
        cap.open(video_src);
    }
    if (!cap.isOpened()) {
        fprintf(stderr, "Video open failed.\n");
        return -1;
    }

    int mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    cap.set(cv::CAP_PROP_FOURCC, mjpg);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(cv::CAP_PROP_FPS, 60);

    // AX 系统初始化
    AX_S32 ret = AX_SYS_Init();
    if (0 != ret) {
        fprintf(stderr, "AX_SYS_Init failed, ret = 0x%x\n", ret);
        return ret;
    }

    FrameQueue fq(QUEUE_SIZE);
    std::atomic<bool> stop(false);
    std::thread t_cap(captureFrames, std::ref(cap), std::ref(fq), std::ref(stop));

    cv::Mat frame;
    std::vector<uint8_t> resized(DEFAULT_IMG_H * DEFAULT_IMG_W * 3);
    std::vector<uint8_t> hand_resized(PALM_IN_H * PALM_IN_W * 3);

    serial_init("/dev/ttyACM0");

    struct PerfInfo {
        std::string name;
        double time_ms;
    };
    std::vector<PerfInfo> perf_stats{
        {"YOLO11n-Detect", 0},
        {"YOLO11n-Pose",   0},
        {"HandPose",       0},
    };

    auto update_time_only = [](PerfInfo &p, double ms) { p.time_ms = ms; };

    auto draw_perf = [](cv::Mat &mat, const std::vector<PerfInfo> &stats) {
        int x = 10, y = 25;
        cv::Mat overlay;
        mat.copyTo(overlay);
        cv::rectangle(overlay,
                      cv::Point(0, 0),
                      cv::Point(220, stats.size() * 25 + 15),
                      cv::Scalar(0, 0, 0),
                      -1);
        double alpha = 0.4;
        cv::addWeighted(overlay, alpha, mat, 1 - alpha, 0, mat);
        const double font_scale = 0.5;
        const int thickness = 1;
        const int outline_thickness = 2;
        for (auto &p : stats) {
            char buf[100];
            snprintf(buf, sizeof(buf), "%s: %.1f ms", p.name.c_str(), p.time_ms);
            cv::putText(mat, buf, cv::Point(x + 1, y + 1),
                        cv::FONT_HERSHEY_SIMPLEX, font_scale,
                        cv::Scalar(0, 0, 0), outline_thickness);
            cv::putText(mat, buf, cv::Point(x, y),
                        cv::FONT_HERSHEY_SIMPLEX, font_scale,
                        cv::Scalar(0, 255, 255), thickness);
            y += 25;
        }
    };

    // 预热：确保三个模型的 Engine 已经创建
    {
        std::vector<uint8_t> dummy_face(DEFAULT_IMG_H * DEFAULT_IMG_W * 3, 0);
        std::vector<uint8_t> dummy_palm(PALM_IN_H * PALM_IN_W * 3, 0);
        cv::Mat dummy(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
        task_face::run(dummy, dummy_face, DEFAULT_IMG_H, DEFAULT_IMG_W);
        task_pose::run(dummy, dummy_face, DEFAULT_IMG_H, DEFAULT_IMG_W);
        task_hand::run(dummy, dummy_palm);
    }

    while (!stop) {
        if (!fq.pop(frame)) break;

        cv::Mat canvas = frame.clone();

        common::get_input_data_letterbox(frame, resized,
                                         DEFAULT_IMG_H, DEFAULT_IMG_W,
                                         true);
        common::get_input_data_letterbox(frame, hand_resized,
                                         PALM_IN_H, PALM_IN_W,
                                         true);

        // Face
        {
            int64 start = cv::getTickCount();
            task_face::run(canvas, resized, DEFAULT_IMG_H, DEFAULT_IMG_W);
            double elapsed_ms = (cv::getTickCount() - start) * 1000.0 / cv::getTickFrequency();
            update_time_only(perf_stats[0], elapsed_ms);
        }

        // Pose
        {
            int64 start = cv::getTickCount();
            task_pose::run(canvas, resized, DEFAULT_IMG_H, DEFAULT_IMG_W);
            double elapsed_ms = (cv::getTickCount() - start) * 1000.0 / cv::getTickFrequency();
            update_time_only(perf_stats[1], elapsed_ms);
        }

        // Hand (Palm + HandPose)
        {
            int64 start = cv::getTickCount();
            task_hand::run(canvas, hand_resized);
            double elapsed_ms = (cv::getTickCount() - start) * 1000.0 / cv::getTickFrequency();
            update_time_only(perf_stats[2], elapsed_ms);
        }

        draw_perf(canvas, perf_stats);
        
        {
            // 2. 设置JPEG编码参数（可选），如压缩质量为85
            std::vector<int> params;
            params.push_back(cv::IMWRITE_JPEG_QUALITY);
            params.push_back(85); // 取值0~100，默认95

            // 3. 编码为JPEG，存入内存缓冲区
            std::vector<uchar> buf;
            if (cv::imencode(".jpg", canvas, buf, params)) {
                send_jpeg(buf.data(), buf.size());
            }
        }
        // cv::imshow("YOLO11 AX-Engine Demo", canvas);
        // char key = (char)cv::waitKey(1);
        // if (key == 27 || key == 'q') {
        //     stop = true;
        //     break;
        // }
    }

    fq.stop();
    if (t_cap.joinable()) t_cap.join();
    cap.release();
    cv::destroyAllWindows();

    // 释放 Engine IO/Handle（简单起见，只做存在判断）
    auto free_ctx = [](AxEngineContext &c) {
        if (!c.inited) return;
        middleware::free_io(&c.io_data);
        if (c.handle) AX_ENGINE_DestroyHandle(c.handle);
        c.handle = nullptr;
        c.io_info = nullptr;
        c.inited  = false;
    };
    free_ctx(task_face::ctx);
    free_ctx(task_pose::ctx);
    free_ctx(task_hand::palm_ctx);
    free_ctx(task_hand::hand_ctx);

    AX_ENGINE_Deinit();
    AX_SYS_Deinit();
    return 0;
}












#include <hv/TcpServer.h>
#include <sys/time.h>
#include <time.h>
using namespace hv;
static int get_jpeg_status = 0;
std::vector<uint8_t> jpeg_buf_vec;
TcpServer srv;
#ifndef STREAM_PORT
#define STREAM_PORT 8081
#endif

const char *http_response = "HTTP/1.0 200 OK\r\n"
                            "Server: BaseHTTP/0.6 Python/3.10.12\r\n"
                            "Date: %s\r\n"
                            "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n"
                            "Connection: close\r\n"
                            "Content-Type: multipart/x-mixed-replace;boundary=--boundarydonotcross\r\n"
                            "Expires: Mon, 1 Jan 2130 00:00:00 GMT\r\n"
                            "Pragma: no-cache\r\n"
                            "Access-Control-Allow-Origin: *\r\n";
const char *http_jpeg_response = "\r\n"
                                 "--boundarydonotcross\r\n"
                                 "X-Timestamp: %lf\r\n"
                                 "Content-Length: %d\r\n"
                                 "Content-Type: image/jpeg\r\n"
                                 "\r\n";
const char *http_static_response = "HTTP/1.0 200 OK\r\n"
                                  "Server: BaseHTTP/0.6 Python/3.10.12\r\n"
                                  "Date: %s\r\n"
                                  "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n"
                                  "Connection: close\r\n"
                                  "Content-Type: image/jpeg\r\n"
                                  "Expires: Mon, 1 Jan 2130 00:00:00 GMT\r\n"
                                  "Pragma: no-cache\r\n"
                                  "Access-Control-Allow-Origin: *\r\n"
                                  "\r\n";
char http_response_buff[1024];
char http_response_buff1[1024];

void send_jpeg(void *buff, int size)
{
    if(size > jpeg_buf_vec.size())
    {
        jpeg_buf_vec.resize(size);
    }
    memcpy(jpeg_buf_vec.data(), buff, size);
    switch (get_jpeg_status) {
        case 0: {
            int listenfd = srv.createsocket(STREAM_PORT);
            if (listenfd < 0) {
                break;
            }
            srv.onConnection = [](const SocketChannelPtr &channel) {
                std::string peeraddr = channel->peeraddr();
                if (channel->isConnected()) {
                    auto st =  channel->newContextPtr<std::vector<int>>();
                    st->resize(2);
                    st->at(0) = 0;
                    st->at(1) = 0;
                    printf("%s connected! connfd=%d\n", peeraddr.c_str(), channel->fd());
                } else {
                    channel->deleteContextPtr();
                    printf("%s disconnected! connfd=%d\n", peeraddr.c_str(), channel->fd());
                }
            };
            srv.onMessage = [](const SocketChannelPtr &channel, Buffer *buf) {
                // echo
                auto st = channel->getContextPtr<std::vector<int>>();
                printf("< %.*s\n", (int)buf->size(), (char *)buf->data());
                if (strstr((char *)buf->data(), "static_image") != NULL)
                {
                    st->at(0) = 1;
                }
                if (strstr((char *)buf->data(), "\r\n\r\n") != NULL)
                {
                    if (st->at(0) == 1)
                    {
                        memset(http_response_buff, 0, 1024);

                        time_t current_time;
                        struct tm *time_info;

                        time(&current_time);
                        time_info = gmtime(&current_time);  // 使用gmtime以获取GMT时间

                        char time_str[30];  // 存储时间字符串的数组

                        // 使用strftime函数将时间格式化为指定的格式
                        strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", time_info);

                        sprintf(http_response_buff, http_static_response, time_str);
                        channel->write(http_response_buff);
                        channel->write(jpeg_buf_vec.data(), jpeg_buf_vec.size());
                        channel->close();
                    }else
                    {
                        memset(http_response_buff, 0, 1024);

                        time_t current_time;
                        struct tm *time_info;

                        time(&current_time);
                        time_info = gmtime(&current_time);  // 使用gmtime以获取GMT时间

                        char time_str[30];  // 存储时间字符串的数组

                        // 使用strftime函数将时间格式化为指定的格式
                        strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", time_info);

                        sprintf(http_response_buff, http_response, time_str);
                        channel->write(http_response_buff);
                        st->at(1) = 1;
                    }
                }

                // channel->write(buf);
            };
            srv.setThreadNum(2);
            srv.start();
            get_jpeg_status = 1;
        } break;
        case 1: {
            char tmpsdas[256];

            struct timeval tv;
            gettimeofday(&tv, NULL);

            double timestamp = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

            memset(http_response_buff1, 0, 1024);
            sprintf(http_response_buff1, http_jpeg_response, timestamp, size);

            srv.foreachChannel([&](const SocketChannelPtr &channel) {
                auto st = channel->getContextPtr<std::vector<int>>();
                if (st->at(1) == 1)
                {
                    channel->write(http_response_buff1);
                    channel->write(buff, size);
                }
            });
        } break;
        default:
            break;
    }

    // static int nihasd = 0;
    // if(nihasd < 60)
    // {

    //     char filename[100];
    //     sprintf(filename, "./nihao_%d.jpg", nihasd);

    //     FILE *fp_w = fopen(filename, "wb");
    //     if (fp_w) {
    //         // ALOGN("\tWrite new JPG result image to file: %s", dstFile);
    //         fwrite(buff->p_vir, 1, buff->n_size, fp_w);
    //         fclose(fp_w);
    //     }
    //     nihasd ++;
    // }
}




