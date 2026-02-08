// #include <cstdio>
// #include <cstring>
// #include <queue>
// #include <thread>
// #include <mutex>
// #include <condition_variable>
// #include <atomic>

// #include <opencv2/opencv.hpp>

// #include "base/common.hpp"
// #include "base/detection.hpp"
// #include "base/pose.hpp"
// #include "base/stackchan.hpp"
// #include "middleware/io.hpp"
// #include "utilities/cmdline.hpp"
// #include "utilities/file.hpp"
// #include "utilities/timer.hpp"

// #include <ax_sys_api.h>
// #include <ax_engine_api.h>

// using namespace std;

// const int DEFAULT_IMG_H = 320;
// const int DEFAULT_IMG_W = 320;
// const int HAND_IMG_H    = 224;
// const int HAND_IMG_W    = 224;
// const int PALM_IN_H     = 192;
// const int PALM_IN_W     = 192;
// const int QUEUE_SIZE    = 2;

// // ====================== 帧队列类 ======================
// class FrameQueue {
//     std::queue<cv::Mat> queue_;
//     std::mutex mutex_;
//     std::condition_variable cv_;
//     size_t max_size_;
//     std::atomic<bool> stop_flag_;
// public:
//     FrameQueue(size_t max_size = QUEUE_SIZE) : max_size_(max_size), stop_flag_(false) {}
//     void push(const cv::Mat &frame) {
//         std::unique_lock<std::mutex> lock(mutex_);
//         while (queue_.size() >= max_size_ && !stop_flag_) queue_.pop();
//         if (!stop_flag_) {
//             queue_.push(frame.clone());
//             cv_.notify_one();
//         }
//     }
//     bool pop(cv::Mat &frame) {
//         std::unique_lock<std::mutex> lock(mutex_);
//         cv_.wait(lock, [this] { return !queue_.empty() || stop_flag_; });
//         if (queue_.empty()) return false;
//         frame = queue_.front();
//         queue_.pop();
//         return true;
//     }
//     void stop() {
//         std::lock_guard<std::mutex> lock(mutex_);
//         stop_flag_ = true;
//         cv_.notify_all();
//     }
// };

// // ====================== Engine Helper（复用 handle） ======================
// struct AxEngineContext {
//     AX_ENGINE_HANDLE         handle{nullptr};
//     AX_ENGINE_IO_INFO_T     *io_info{nullptr};
//     AX_ENGINE_IO_T           io_data{};
//     bool                     inited{false};
// };

// static bool init_engine_once(AxEngineContext &ctx, const std::string &model_path) {
//     if (ctx.inited) return true;

//     // 1. init engine (只在第一次使用时调用；AX_SYS_Init 在 main 中统一)
//     static bool s_engine_inited = false;
//     if (!s_engine_inited) {
//         AX_ENGINE_NPU_ATTR_T npu_attr;
//         memset(&npu_attr, 0, sizeof(npu_attr));
//         npu_attr.eHardMode = AX_ENGINE_VIRTUAL_NPU_DISABLE;
//         auto ret = AX_ENGINE_Init(&npu_attr);
//         if (0 != ret) {
//             fprintf(stderr, "AX_ENGINE_Init failed, ret = 0x%x\n", ret);
//             return false;
//         }
//         s_engine_inited = true;
//     }

//     // 2. read model
//     std::vector<char> model_buffer;
//     if (!utilities::read_file(model_path, model_buffer)) {
//         fprintf(stderr, "Read AX-Engine model(%s) file failed.\n", model_path.c_str());
//         return false;
//     }

//     // 3. create handle
//     auto ret = AX_ENGINE_CreateHandle(&ctx.handle,
//                                       model_buffer.data(),
//                                       model_buffer.size());
//     if (0 != ret) {
//         fprintf(stderr, "AX_ENGINE_CreateHandle failed, ret = 0x%x\n", ret);
//         return false;
//     }

//     // 4. create context
//     ret = AX_ENGINE_CreateContext(ctx.handle);
//     if (0 != ret) {
//         fprintf(stderr, "AX_ENGINE_CreateContext failed, ret = 0x%x\n", ret);
//         AX_ENGINE_DestroyHandle(ctx.handle);
//         ctx.handle = nullptr;
//         return false;
//     }

//     // 5. get io info
//     ret = AX_ENGINE_GetIOInfo(ctx.handle, &ctx.io_info);
//     if (0 != ret) {
//         fprintf(stderr, "AX_ENGINE_GetIOInfo failed, ret = 0x%x\n", ret);
//         AX_ENGINE_DestroyHandle(ctx.handle);
//         ctx.handle = nullptr;
//         return false;
//     }

//     // 6. alloc io
//     memset(&ctx.io_data, 0, sizeof(ctx.io_data));
//     ret = middleware::prepare_io(ctx.io_info,
//                                  &ctx.io_data,
//                                  std::make_pair(AX_ENGINE_ABST_DEFAULT,
//                                                 AX_ENGINE_ABST_CACHED));
//     if (0 != ret) {
//         fprintf(stderr, "prepare_io failed, ret = 0x%x\n", ret);
//         AX_ENGINE_DestroyHandle(ctx.handle);
//         ctx.handle = nullptr;
//         return false;
//     }

//     ctx.inited = true;
//     return true;
// }

// static inline bool run_engine(AxEngineContext &ctx,
//                               const std::vector<uint8_t> &data) {
//     AX_S32 ret = middleware::push_input(data, &ctx.io_data, ctx.io_info);
//     if (0 != ret) {
//         fprintf(stderr, "push_input failed, ret = 0x%x\n", ret);
//         return false;
//     }
//     ret = AX_ENGINE_RunSync(ctx.handle, &ctx.io_data);
//     if (0 != ret) {
//         fprintf(stderr, "AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
//         return false;
//     }
//     return true;
// }

// // ====================== Face 检测 ======================
// namespace task_face {
// using namespace detection;

// const char *CLASS_NAMES[]  = {"face"};
// int   NUM_CLASS            = 1;
// const float PROB_THRESHOLD = 0.45f;
// const float NMS_THRESHOLD  = 0.45f;

// static AxEngineContext ctx;
// std::string model_file;

// void post_process(const cv::Mat &mat_in,
//                   cv::Mat &mat_out,
//                   int iw, int ih)
// {
//     std::vector<Object> proposals, objects;

//     for (int i = 0; i < 3; ++i) {
//         float *feat_ptr = (float *)ctx.io_data.pOutputs[i].pVirAddr;
//         int stride      = (1 << i) * 8;
//         generate_proposals_yolov8_native(stride,
//                                          feat_ptr,
//                                          PROB_THRESHOLD,
//                                          proposals,
//                                          iw,
//                                          ih,
//                                          NUM_CLASS);
//     }

//     get_out_bbox(proposals, objects, NMS_THRESHOLD,
//                  ih, iw, mat_in.rows, mat_in.cols);

//     mat_out = draw_objects(mat_in, objects, CLASS_NAMES,
//                            "Face Detection", 1.0, 2);
//     process_objects_for_servo(objects);
// }

// bool run(cv::Mat &mat, const std::vector<uint8_t> &data, int h, int w)
// {
//     if (!init_engine_once(ctx, model_file)) return false;
//     if (!run_engine(ctx, data)) return false;
//     post_process(mat, mat, w, h);
//     return true;
// }
// } // namespace task_face

// // ====================== Pose 姿态 ======================
// namespace task_pose {
// using namespace detection;

// const char *CLASS_NAMES[] = {"person"};
// const std::vector<std::vector<uint8_t>> KPS_COLORS = {
//     {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},
//     {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},
//     {255, 128, 0},  {51, 153, 255}, {51, 153, 255}, {51, 153, 255}, {51, 153, 255},
//     {51, 153, 255}, {51, 153, 255}};
// const std::vector<std::vector<uint8_t>> LIMB_COLORS = {
//     {51, 153, 255}, {51, 153, 255}, {51, 153, 255}, {51, 153, 255},
//     {255, 51, 255}, {255, 51, 255}, {255, 51, 255},
//     {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},
//     {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},
//     {0, 255, 0},    {0, 255, 0}};
// const std::vector<std::vector<uint8_t>> SKELETON = {
//     {16, 14}, {14, 12}, {17, 15}, {15, 13}, {12, 13},
//     {6, 12},  {7, 13},  {6, 7},   {6, 8},   {7, 9},
//     {8, 10},  {9, 11},  {2, 3},   {1, 2},   {1, 3},
//     {2, 4},   {3, 5},   {4, 6},   {5, 7}};

// int   NUM_CLASS            = 1;
// int   NUM_POINT            = 17;
// const float PROB_THRESHOLD = 0.45f;
// const float NMS_THRESHOLD  = 0.45f;

// static AxEngineContext ctx;
// std::string model_file;

// void post_process(const cv::Mat &mat_in,
//                   cv::Mat &mat_out,
//                   int iw, int ih)
// {
//     std::vector<Object> proposals, objects;

//     float *out_ptr[3] = {
//         (float *)ctx.io_data.pOutputs[0].pVirAddr,
//         (float *)ctx.io_data.pOutputs[1].pVirAddr,
//         (float *)ctx.io_data.pOutputs[2].pVirAddr};
//     float *kps_ptr[3] = {
//         (float *)ctx.io_data.pOutputs[3].pVirAddr,
//         (float *)ctx.io_data.pOutputs[4].pVirAddr,
//         (float *)ctx.io_data.pOutputs[5].pVirAddr};

//     for (int i = 0; i < 3; ++i) {
//         int stride = (1 << i) * 8;
//         generate_proposals_yolov8_pose_native(stride,
//                                               out_ptr[i],
//                                               kps_ptr[i],
//                                               PROB_THRESHOLD,
//                                               proposals,
//                                               iw,
//                                               ih,
//                                               NUM_POINT,
//                                               NUM_CLASS);
//     }

//     get_out_bbox_kps(proposals, objects, NMS_THRESHOLD,
//                      ih, iw, mat_in.rows, mat_in.cols);

//     mat_out = draw_keypoints(mat_in, objects,
//                              KPS_COLORS, LIMB_COLORS, SKELETON,
//                              "Pose", 1.0, 2);
// }

// bool run(cv::Mat &mat, const std::vector<uint8_t> &data, int h, int w)
// {
//     if (!init_engine_once(ctx, model_file)) return false;
//     if (!run_engine(ctx, data)) return false;
//     post_process(mat, mat, w, h);
//     return true;
// }
// } // namespace task_pose

// // ====================== Palm + HandPose ======================
// namespace task_hand {
// using namespace detection;

// const int HAND_JOINTS        = 21;
// const float PROB_THRESHOLD   = 0.65f;
// const float NMS_THRESHOLD    = 0.45f;
// const int map_size[2]        = {24, 12};
// const int strides[2]         = {8, 16};
// const int anchor_size[2]     = {2, 6};
// const float anchor_offset[2] = {0.5f, 0.5f};

// static AxEngineContext palm_ctx;
// static AxEngineContext hand_ctx;
// std::string palm_model_file, hand_model_file;

// bool init_palm() {
//     return init_engine_once(palm_ctx, palm_model_file);
// }
// bool init_hand() {
//     return init_engine_once(hand_ctx, hand_model_file);
// }

// bool run_hand_model(const std::vector<uint8_t> &data,
//                     pose::ai_hand_parts_s &out_pose)
// {
//     if (!init_hand()) return false;

//     AX_S32 ret = middleware::push_input(data, &hand_ctx.io_data, hand_ctx.io_info);
//     if (0 != ret) {
//         fprintf(stderr, "Hand push_input failed, ret = 0x%x\n", ret);
//         return false;
//     }
//     ret = AX_ENGINE_RunSync(hand_ctx.handle, &hand_ctx.io_data);
//     if (0 != ret) {
//         fprintf(stderr, "Hand AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
//         return false;
//     }

//     auto &info_point = hand_ctx.io_data.pOutputs[0];
//     auto &info_score = hand_ctx.io_data.pOutputs[1];
//     float *point_ptr = (float *)info_point.pVirAddr;
//     float *score_ptr = (float *)info_score.pVirAddr;
//     pose::post_process_hand(point_ptr, score_ptr,
//                             out_pose, HAND_JOINTS,
//                             HAND_IMG_H, HAND_IMG_W);
//     return true;
// }

// void post_process_palm(cv::Mat &mat)
// {
//     std::vector<PalmObject> proposals, objects;

//     auto bboxes_ptr = (float *)palm_ctx.io_data.pOutputs[0].pVirAddr;
//     auto scores_ptr = (float *)palm_ctx.io_data.pOutputs[1].pVirAddr;

//     float prob_threshold_unsigmoid =
//         -1.0f * (float)std::log((1.0f / PROB_THRESHOLD) - 1.0f);

//     generate_proposals_palm(proposals,
//                             PROB_THRESHOLD,
//                             PALM_IN_W,
//                             PALM_IN_H,
//                             scores_ptr,
//                             bboxes_ptr,
//                             2,
//                             strides,
//                             anchor_size,
//                             anchor_offset,
//                             map_size,
//                             prob_threshold_unsigmoid);

//     get_out_bbox_palm(proposals, objects, NMS_THRESHOLD,
//                       PALM_IN_H, PALM_IN_W, mat.rows, mat.cols);

//     cv::Mat mat_draw = mat;

//     // 手势去抖
//     static int last_gesture_id = -1;
//     static int same_count      = 0;
//     const int required_count   = 10;

//     for (size_t i = 0; i < objects.size(); ++i) {
//         cv::Mat hand_roi;
//         cv::warpAffine(mat, hand_roi,
//                        objects[i].affine_trans_mat,
//                        cv::Size(HAND_IMG_W, HAND_IMG_H));

//         std::vector<uint8_t> hand_image(HAND_IMG_H * HAND_IMG_W * 3);
//         common::get_input_data_no_letterbox(hand_roi,
//                                             hand_image,
//                                             HAND_IMG_H,
//                                             HAND_IMG_W,
//                                             true);

//         pose::ai_hand_parts_s hand_parts;
//         if (!run_hand_model(hand_image, hand_parts)) {
//             continue;
//         }

//         pose::draw_result_hand_on_image(mat_draw,
//                                         hand_parts,
//                                         HAND_JOINTS,
//                                         objects[i].affine_trans_mat_inv);

//         if (check_palm_objects_size(objects, 0.05, 0.05) == 0) {
//             int gesture_id = classify_gesture(hand_parts);
//             if (gesture_id != -1) {
//                 if (gesture_id == last_gesture_id) {
//                     same_count++;
//                 } else {
//                     same_count      = 1;
//                     last_gesture_id = gesture_id;
//                 }

//                 if (same_count >= required_count) {
//                     same_count = 0;
//                     const std::string &label = gesture_defs[gesture_id].label;
//                     if (label == "ok") {
//                         send_motion("reverse");
//                     } else if (label == "one") {
//                         send_motion("shake");
//                     } else if (label == "two") {
//                         send_motion("nod");
//                     } else if (label == "five") {
//                         send_motion("forward");
//                     } else if (label == "four") {
//                         send_motion("photo");
//                     }
//                 }
//             }
//         }
//     }

//     mat = draw_objects_palm(mat_draw, objects, "Palm detection");
// }

// bool run(cv::Mat &mat, const std::vector<uint8_t> &data)
// {
//     if (!init_palm()) return false;

//     AX_S32 ret = middleware::push_input(data, &palm_ctx.io_data, palm_ctx.io_info);
//     if (0 != ret) {
//         fprintf(stderr, "Palm push_input failed, ret = 0x%x\n", ret);
//         return false;
//     }
//     ret = AX_ENGINE_RunSync(palm_ctx.handle, &palm_ctx.io_data);
//     if (0 != ret) {
//         fprintf(stderr, "Palm AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
//         return false;
//     }

//     post_process_palm(mat);
//     return true;
// }
// } // namespace task_hand

// // ====================== 采集线程 ======================
// void captureFrames(cv::VideoCapture &cap, FrameQueue &fq, std::atomic<bool> &stop)
// {
//     cv::Mat frame;
//     while (!stop) {
//         cap >> frame;
//         if (frame.empty()) {
//             stop = true;
//             break;
//         }
//         cv::flip(frame, frame, 1);
//         fq.push(frame);
//     }
//     fq.stop();
// }
// void send_jpeg(void *buff, int size);
// // ====================== 主程序 ======================
// int main(int argc, char **argv)
// {
//     cmdline::parser cmd;
//     cmd.add<std::string>("face_model", 'f', "face model(joint)", true, "");
//     cmd.add<std::string>("pose_model", 'p', "pose model(joint)", true, "");
//     cmd.add<std::string>("palm_model", 'm', "palm model(joint)", true, "");
//     cmd.add<std::string>("hand_model", 'h', "hand model(joint)", true, "");
//     cmd.add<std::string>("video",      'v', "video src",         true, "");
//     cmd.parse_check(argc, argv);

//     task_face::model_file      = cmd.get<std::string>("face_model");
//     task_pose::model_file      = cmd.get<std::string>("pose_model");
//     task_hand::palm_model_file = cmd.get<std::string>("palm_model");
//     task_hand::hand_model_file = cmd.get<std::string>("hand_model");
//     std::string video_src      = cmd.get<std::string>("video");

//     // 打开摄像头 / 视频
//     cv::VideoCapture cap;
//     try {
//         int idx = std::stoi(video_src);
//         cap.open(idx, cv::CAP_V4L2);
//     } catch (...) {
//         cap.open(video_src);
//     }
//     if (!cap.isOpened()) {
//         fprintf(stderr, "Video open failed.\n");
//         return -1;
//     }

//     int mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
//     cap.set(cv::CAP_PROP_FOURCC, mjpg);
//     cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
//     cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
//     cap.set(cv::CAP_PROP_FPS, 60);

//     // AX 系统初始化
//     AX_S32 ret = AX_SYS_Init();
//     if (0 != ret) {
//         fprintf(stderr, "AX_SYS_Init failed, ret = 0x%x\n", ret);
//         return ret;
//     }

//     FrameQueue fq(QUEUE_SIZE);
//     std::atomic<bool> stop(false);
//     std::thread t_cap(captureFrames, std::ref(cap), std::ref(fq), std::ref(stop));

//     cv::Mat frame;
//     std::vector<uint8_t> resized(DEFAULT_IMG_H * DEFAULT_IMG_W * 3);
//     std::vector<uint8_t> hand_resized(PALM_IN_H * PALM_IN_W * 3);

//     serial_init("/dev/ttyACM0");

//     struct PerfInfo {
//         std::string name;
//         double time_ms;
//     };
//     std::vector<PerfInfo> perf_stats{
//         {"YOLO11n-Detect", 0},
//         {"YOLO11n-Pose",   0},
//         {"HandPose",       0},
//     };

//     auto update_time_only = [](PerfInfo &p, double ms) { p.time_ms = ms; };

//     auto draw_perf = [](cv::Mat &mat, const std::vector<PerfInfo> &stats) {
//         int x = 10, y = 25;
//         cv::Mat overlay;
//         mat.copyTo(overlay);
//         cv::rectangle(overlay,
//                       cv::Point(0, 0),
//                       cv::Point(220, stats.size() * 25 + 15),
//                       cv::Scalar(0, 0, 0),
//                       -1);
//         double alpha = 0.4;
//         cv::addWeighted(overlay, alpha, mat, 1 - alpha, 0, mat);
//         const double font_scale = 0.5;
//         const int thickness = 1;
//         const int outline_thickness = 2;
//         for (auto &p : stats) {
//             char buf[100];
//             snprintf(buf, sizeof(buf), "%s: %.1f ms", p.name.c_str(), p.time_ms);
//             cv::putText(mat, buf, cv::Point(x + 1, y + 1),
//                         cv::FONT_HERSHEY_SIMPLEX, font_scale,
//                         cv::Scalar(0, 0, 0), outline_thickness);
//             cv::putText(mat, buf, cv::Point(x, y),
//                         cv::FONT_HERSHEY_SIMPLEX, font_scale,
//                         cv::Scalar(0, 255, 255), thickness);
//             y += 25;
//         }
//     };

//     // 预热：确保三个模型的 Engine 已经创建
//     {
//         std::vector<uint8_t> dummy_face(DEFAULT_IMG_H * DEFAULT_IMG_W * 3, 0);
//         std::vector<uint8_t> dummy_palm(PALM_IN_H * PALM_IN_W * 3, 0);
//         cv::Mat dummy(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
//         task_face::run(dummy, dummy_face, DEFAULT_IMG_H, DEFAULT_IMG_W);
//         task_pose::run(dummy, dummy_face, DEFAULT_IMG_H, DEFAULT_IMG_W);
//         task_hand::run(dummy, dummy_palm);
//     }

//     while (!stop) {
//         if (!fq.pop(frame)) break;

//         cv::Mat canvas = frame.clone();

//         common::get_input_data_letterbox(frame, resized,
//                                          DEFAULT_IMG_H, DEFAULT_IMG_W,
//                                          true);
//         common::get_input_data_letterbox(frame, hand_resized,
//                                          PALM_IN_H, PALM_IN_W,
//                                          true);

//         // Face
//         {
//             int64 start = cv::getTickCount();
//             task_face::run(canvas, resized, DEFAULT_IMG_H, DEFAULT_IMG_W);
//             double elapsed_ms = (cv::getTickCount() - start) * 1000.0 / cv::getTickFrequency();
//             update_time_only(perf_stats[0], elapsed_ms);
//         }

//         // Pose
//         {
//             int64 start = cv::getTickCount();
//             task_pose::run(canvas, resized, DEFAULT_IMG_H, DEFAULT_IMG_W);
//             double elapsed_ms = (cv::getTickCount() - start) * 1000.0 / cv::getTickFrequency();
//             update_time_only(perf_stats[1], elapsed_ms);
//         }

//         // Hand (Palm + HandPose)
//         {
//             int64 start = cv::getTickCount();
//             task_hand::run(canvas, hand_resized);
//             double elapsed_ms = (cv::getTickCount() - start) * 1000.0 / cv::getTickFrequency();
//             update_time_only(perf_stats[2], elapsed_ms);
//         }

//         draw_perf(canvas, perf_stats);

//         {
//             // 2. 设置JPEG编码参数（可选），如压缩质量为85
//             std::vector<int> params;
//             params.push_back(cv::IMWRITE_JPEG_QUALITY);
//             params.push_back(85); // 取值0~100，默认95

//             // 3. 编码为JPEG，存入内存缓冲区
//             std::vector<uchar> buf;
//             if (cv::imencode(".jpg", canvas, buf, params)) {
//                 send_jpeg(buf.data(), buf.size());
//             }
//         }
//         // cv::imshow("YOLO11 AX-Engine Demo", canvas);
//         // char key = (char)cv::waitKey(1);
//         // if (key == 27 || key == 'q') {
//         //     stop = true;
//         //     break;
//         // }
//     }

//     fq.stop();
//     if (t_cap.joinable()) t_cap.join();
//     cap.release();
//     cv::destroyAllWindows();

//     // 释放 Engine IO/Handle（简单起见，只做存在判断）
//     auto free_ctx = [](AxEngineContext &c) {
//         if (!c.inited) return;
//         middleware::free_io(&c.io_data);
//         if (c.handle) AX_ENGINE_DestroyHandle(c.handle);
//         c.handle = nullptr;
//         c.io_info = nullptr;
//         c.inited  = false;
//     };
//     free_ctx(task_face::ctx);
//     free_ctx(task_pose::ctx);
//     free_ctx(task_hand::palm_ctx);
//     free_ctx(task_hand::hand_ctx);

//     AX_ENGINE_Deinit();
//     AX_SYS_Deinit();
//     return 0;
// }

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
                            "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0,max-age=0\r\n" 
                            "Connection: close\r\n" 
                            "Content-Type:multipart/x-mixed-replace;boundary=--boundarydonotcross\r\n" 
                            "Expires: Mon, 1 Jan 213000:00:00 GMT\r\n" 
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
                                  "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0,max-age=0\r\n" 
                                  "Connection: close\r\n" 
                                  "Content-Type: image/jpeg\r\n" 
                                  "Expires:Mon, 1 Jan 2130 00:00:00 GMT\r\n" 
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

// #include "hal_device.h"
// #include "hal_venc.hpp"
// #include "hal_vdec.hpp"

// // yolo 示例demo

// void jpeg_stream()
// {

// }

// void yolo_process(void *ptr)
// {
//     vo.update_osd();
// }

// int main(int argc, char** argv)
// {
//     hal_device ax650();
//     hal_ivps ivps(ax650);
//     hasl_venc venc(ax650);
//     hal_v4l2 video(0);
//     hal_engine npu_engine();
//     hal_vo vo(ax650);

//     auto yolov8 = npu_engine.load_model("yolov8s.onnx");
//     yolov8.results_connect(yolo_process);
//     yolov8.work();

//     auto venc_jpeg = venc.create_channel(0, HAL_VENC_JPEG);
//     venc_jpeg.work();
//     vo.ch0.connect(venc_jpeg);
//     vo.work();

//     auto ivps_input = ivps.create_group(-1);

//     video.connect(vo);
//     video.connect(ivps_input);
//     ivps_input.ch0.connect(yolov8);
//     ivps_input.work();

//     while (1)
//     {
//         sleep(1);
//     }

//     return 0;
// }

#define CALC_FPS(tips)                                                                                           \
    {                                                                                                            \
        static int fcnt = 0;                                                                                     \
        fcnt++;                                                                                                  \
        static struct timespec ts1, ts2;                                                                         \
        clock_gettime(CLOCK_MONOTONIC, &ts2);                                                                    \
        if ((ts2.tv_sec * 1000 + ts2.tv_nsec / 1000000) - (ts1.tv_sec * 1000 + ts1.tv_nsec / 1000000) >= 1000) { \
            printf("%s => H26X FPS:%d\n", tips, fcnt);                                                           \
            ts1  = ts2;                                                                                          \
            fcnt = 0;                                                                                            \
        }                                                                                                        \
    }

#include <iostream>
#include "hal_v4l2.hpp"
// #include "hal_engine.hpp"
// #include "hal_venc.hpp"
// #include "hal_yolo.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
// #include "/home/nihao/w2T/github/dianjixz-lib/examples/ax650_yolo/main/ax-samples/examples/base/common.hpp"
// const int DEFAULT_IMG_H = 640;
// const int DEFAULT_IMG_W = 640;
// void get_input_data_letterbox(cv::Mat mat, std::vector<uint8_t>& image, int letterbox_rows, int letterbox_cols,
//                               bool bgr2rgb = false)
// {
//     /* letterbox process to support different letterbox size */
//     float scale_letterbox;
//     int resize_rows;
//     int resize_cols;
//     if ((letterbox_rows * 1.0 / mat.rows) < (letterbox_cols * 1.0 / mat.cols)) {
//         scale_letterbox = (float)letterbox_rows * 1.0f / (float)mat.rows;
//     } else {
//         scale_letterbox = (float)letterbox_cols * 1.0f / (float)mat.cols;
//     }
//     resize_cols = int(scale_letterbox * (float)mat.cols);
//     resize_rows = int(scale_letterbox * (float)mat.rows);

//     cv::Mat img_new(letterbox_rows, letterbox_cols, CV_8UC3, image.data());

//     cv::resize(mat, mat, cv::Size(resize_cols, resize_rows));

//     int top   = (letterbox_rows - resize_rows) / 2;
//     int bot   = (letterbox_rows - resize_rows + 1) / 2;
//     int left  = (letterbox_cols - resize_cols) / 2;
//     int right = (letterbox_cols - resize_cols + 1) / 2;

//     // Letterbox filling
//     cv::copyMakeBorder(mat, img_new, top, bot, left, right, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
//     if (bgr2rgb) {
//         cv::cvtColor(img_new, img_new, cv::COLOR_BGR2RGB);
//     }

//     // static int frame_count = 0;
//     // frame_count++;
//     // if (frame_count % 30 == 0) {
//     //     cv::imwrite(std::string("/root/nihao_") + std::to_string(frame_count) + ".jpg", img_new);
//     // }


// }
// #include <numeric>
// #include "/home/nihao/w2T/github/dianjixz-lib/github_source/m5stack_ax650n_msp_3.6.4/sample/npu/include/common.hpp"
// #include "/home/nihao/w2T/github/dianjixz-lib/github_source/m5stack_ax650n_msp_3.6.4/sample/npu/include/detection.hpp"

// const float PROB_THRESHOLD = 0.45f;
// const float NMS_THRESHOLD  = 0.45f;

// const char* CLASS_NAMES[] = {
//     "person",         "bicycle",    "car",           "motorcycle",    "airplane",     "bus",           "train",
//     "truck",          "boat",       "traffic light", "fire hydrant",  "stop sign",    "parking meter", "bench",
//     "bird",           "cat",        "dog",           "horse",         "sheep",        "cow",           "elephant",
//     "bear",           "zebra",      "giraffe",       "backpack",      "umbrella",     "handbag",       "tie",
//     "suitcase",       "frisbee",    "skis",          "snowboard",     "sports ball",  "kite",          "baseball bat",
//     "baseball glove", "skateboard", "surfboard",     "tennis racket", "bottle",       "wine glass",    "cup",
//     "fork",           "knife",      "spoon",         "bowl",          "banana",       "apple",         "sandwich",
//     "orange",         "broccoli",   "carrot",        "hot dog",       "pizza",        "donut",         "cake",
//     "chair",          "couch",      "potted plant",  "bed",           "dining table", "toilet",        "tv",
//     "laptop",         "mouse",      "remote",        "keyboard",      "cell phone",   "microwave",     "oven",
//     "toaster",        "sink",       "refrigerator",  "book",          "clock",        "vase",          "scissors",
//     "teddy bear",     "hair drier", "toothbrush"};
// const float ANCHORS[18] = {10, 13, 16, 30, 33, 23, 30, 61, 62, 45, 59, 119, 116, 90, 156, 198, 373, 326};

// void post_process(const AX_ENGINE_IO_INFO_T* io_info, const AX_ENGINE_IO_T* io_data, int input_w, int input_h)
// {
//     std::vector<detection::Object> proposals;
//     std::vector<detection::Object> objects;
//     float prob_threshold_u_sigmoid = -1.0f * (float)std::log((1.0f / PROB_THRESHOLD) - 1.0f);
//     // timer timer_postprocess;
//     for (uint32_t i = 0; i < io_info->nOutputSize; ++i) {
//         auto& output = io_data->pOutputs[i];
//         // auto& info = io_info->pOutputs[i];
//         auto ptr       = (float*)output.pVirAddr;
//         int32_t stride = (1 << i) * 8;
//         detection::generate_proposals_255(stride, ptr, PROB_THRESHOLD, proposals, input_w, input_h, ANCHORS,
//                                           prob_threshold_u_sigmoid);
//     }

//     detection::get_out_bbox(proposals, objects, NMS_THRESHOLD, input_h, input_w, DEFAULT_IMG_H, DEFAULT_IMG_W);
//     // // fprintf(stdout, "post process cost time:%.2f ms \n", timer_postprocess.cost());
//     // fprintf(stdout, "--------------------------------------\n");
//     // // auto total_time = std::accumulate(time_costs.begin(), time_costs.end(), 0.f);
//     // // auto min_max_time = std::minmax_element(time_costs.begin(), time_costs.end());
//     // // fprintf(stdout,
//     // //         "Repeat %d times, avg time %.2f ms, max_time %.2f ms, min_time %.2f ms\n",
//     // //         (int)time_costs.size(),
//     // //         total_time / (float)time_costs.size(),
//     // //         *min_max_time.second,
//     // //         *min_max_time.first);
//     // for (auto& obj : objects) {
//     //     std::cout << obj.label << std::endl;
//     //     std::cout << obj.prob << std::endl;
//     //     std::cout << obj.rect.x << std::endl;
//     //     std::cout << obj.rect.y << std::endl;
//     //     std::cout << obj.rect.width << std::endl;
//     //     std::cout << obj.rect.height << std::endl;

//     // }
//     // fprintf(stdout, "--------------------------------------\n");
//     // fprintf(stdout, "detection num: %zu\n", objects.size());

//     // detection::draw_objects(mat, objects, CLASS_NAMES, "yolov5s_out");
// }

// // model_handle* yolov5 = nullptr;
// std::shared_ptr<ModelHandle> yolov5;



// void post(const void* data, const void* info, void* pt)
// {
//     // const AX_ENGINE_IO_T* data, const AX_ENGINE_IO_INFO_T* info, void* pt;
//     // post_process(info, data, DEFAULT_IMG_W, DEFAULT_IMG_H);
// }

// void v4l2_fream(int w, int h, int f, void* data, int size)
// {
//     // printf("w=%d, h=%d, f=%d, s:%d\n", w, h, f, size);
//     CALC_FPS("V4L2 Capture");

//     std::vector<uchar> mjpegData((uchar*)data, (uchar*)data + size);
//     cv::Mat frame = cv::imdecode(mjpegData, cv::IMREAD_COLOR);
//     // cv::Mat frame = cv::imread("/opt/data/npu/images/cat.jpg", cv::IMREAD_COLOR);
//     if (frame.empty()) {
//         return;
//     }
//     std::vector<uint8_t> resized(DEFAULT_IMG_H * DEFAULT_IMG_W * 3);
//     // try {
//     get_input_data_letterbox(frame, resized, DEFAULT_IMG_H, DEFAULT_IMG_W, true);
//     yolov5->ForwardComputation(resized);

//     // } catch (...) {
//     //     std::cout << "exception ..." << std::endl;
//     // }
// }


void get_picture(int w, int h, int f, void* data, int size)
{
    // printf("w=%d, h=%d, f=%d, s:%d\n", w, h, f, size);
    CALC_FPS("VENC Capture");
    send_jpeg(data, size);
}
// #include "hal_v4l2.hpp"
int main(int argc, char** argv)
{
    std::cout << "Hello, World!" << std::endl;
    int a = 0;
    int b = 2;

    // hal_device ax650c;
    hal_v4l2 v4l2_camera(0);
    v4l2_camera.set_video_frame(3840, 2160, HAL_V4L2_FORMAT_MJPEG, 60);
    v4l2_camera.connect(get_picture);
    v4l2_camera.work();
    // hal_yolo yolos(ax650c);
    // auto yolov5 = yolos.creat("yolov8s.onnx");

    // auto result = yolov5->yolo_computation_result();

    while (1) {
        sleep(1);
    }

    return 0;
}
