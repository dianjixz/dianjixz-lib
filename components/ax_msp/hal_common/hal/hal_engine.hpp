#ifndef HAL_ENGINE_H
#define HAL_ENGINE_H

#include "hal_device.h"
#include <vector>

class modelhandle {
private:
    /* data */
public:
    int _in_width;
    int _in_height;
    std::string _model_path;
    std::string _model_type;
    std::vector<std::string> _class_names;
    std::vector<int> _anchors;
    typedef std::function<void(void*, void*)> model_result_put_func_t;
    // void on_result(const AX_ENGINE_IO_T* data, const AX_ENGINE_IO_INFO_T* info)
    model_result_put_func_t model_result_put_func;
    int id_info[3];
    int _handle_index;
    void* model_info;
    void* model_data;
public:
    modelhandle(/* args */);
    virtual void load_model(const std::string& mode)                  = 0;
    virtual bool ForwardComputation(const std::vector<uint8_t>& data) = 0;
    void connect(model_result_put_func_t func)
    {
        model_result_put_func = func;
    }
    virtual int work() = 0;  // 修复：应该是纯虚函数或提供实现
    template <class T>
    std::shared_ptr<T> link_id();
    virtual void destroy() = 0;
    virtual ~modelhandle();  // 修复：虚析构函数
};

class hal_engine {
private:
    std::shared_ptr<SysGuard> sys;
    std::list<std::shared_ptr<modelhandle>> encoder_list;
    int _handle_index;
    // static std::atomic<int> _self_count;
    static int _self_count;

public:
    hal_engine(const hal_device& device);
    std::shared_ptr<modelhandle> creat(const std::string& mode);
    void destroy(const std::shared_ptr<modelhandle>& encoder)
    {
        for (auto it = encoder_list.begin(); it != encoder_list.end();) {
            if (it->get() == encoder.get()) {
                it = encoder_list.erase(it);
                break;
            } else {
                ++it;
            }
        }
    }
    void destroy(int encoder)
    {
        for (auto it = encoder_list.begin(); it != encoder_list.end();) {
            if ((*it)->_handle_index == encoder) {
                it = encoder_list.erase(it);
                break;
            } else {
                ++it;
            }
        }
    }
    ~hal_engine();
};

#include <cstdio>
#include <cstring>
#include <vector>
#include <utility>

#include <ax_sys_api.h>
#include <ax_engine_api.h>

#define AX_CMM_ALIGN_SIZE 128

extern const char* AX_CMM_SESSION_NAME;

typedef enum {
    AX_ENGINE_ABST_DEFAULT = 0,
    AX_ENGINE_ABST_CACHED  = 1,
} AX_ENGINE_ALLOC_BUFFER_STRATEGY_T;

typedef std::pair<AX_ENGINE_ALLOC_BUFFER_STRATEGY_T, AX_ENGINE_ALLOC_BUFFER_STRATEGY_T> INPUT_OUTPUT_ALLOC_STRATEGY;

#define SAMPLE_AX_ENGINE_DEAL_HANDLE            \
    if (0 != ret) {                             \
        return AX_ENGINE_DestroyHandle(handle); \
    }

#define SAMPLE_AX_ENGINE_DEAL_HANDLE_IO         \
    if (0 != ret) {                             \
        middleware::free_io(&io_data);          \
        return AX_ENGINE_DestroyHandle(handle); \
    }

namespace middleware {
void free_io_index(AX_ENGINE_IO_BUFFER_T* io_buf, size_t index);
void free_io(AX_ENGINE_IO_T* io);
int prepare_io(AX_ENGINE_IO_INFO_T* info, AX_ENGINE_IO_T* io_data, INPUT_OUTPUT_ALLOC_STRATEGY strategy);
int push_input(const std::vector<uint8_t>& data, AX_ENGINE_IO_T* io_t, AX_ENGINE_IO_INFO_T* info_t);
}  // namespace middleware

#include <cmath>
#include <cstdint>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

namespace detection {
typedef struct {
    int grid0;
    int grid1;
    int stride;
} GridAndStride;

typedef struct {
    cv::Rect_<float> rect;
    int label;
    float prob;
    cv::Point2f landmark[5];
} Object;

void nms_sorted_bboxes(const std::vector<Object>& faceobjects, std::vector<int>& picked, float nms_threshold);

void generate_proposals_255(int stride, const float* feat, float prob_threshold, std::vector<Object>& objects,
                            int letterbox_cols, int letterbox_rows, const float* anchors,
                            float prob_threshold_unsigmoid);

void draw_objects(const cv::Mat& bgr, const std::vector<Object>& objects, const char** class_names,
                  const char* output_name);

void reverse_letterbox(std::vector<Object>& proposal, std::vector<Object>& objects, int letterbox_rows,
                       int letterbox_cols, int src_rows, int src_cols);

void get_out_bbox_no_letterbox(std::vector<Object>& proposals, std::vector<Object>& objects, const float nms_threshold,
                               int model_h, int model_w, int src_rows, int src_cols);

void get_out_bbox(std::vector<Object>& proposals, std::vector<Object>& objects, const float nms_threshold,
                  int letterbox_rows, int letterbox_cols, int src_rows, int src_cols);

}  // namespace detection

#endif  // HAL_ENGINE_H