/*
 * AXERA is pleased to support the open source community by making ax-samples available.
 *
 * Copyright (c) 2024, AXERA Semiconductor Co., Ltd. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

/*
 * Note: For the YOLO11 series exported by the ultralytics project.
 * Author: QQC
 */



/*

ffmpeg -i input.mp4 -f rawvideo -pix_fmt rgb24 - | ./ax_yolo11_steps_stdout -m yolo11.axmodel -s 650x650 | ffmpeg -f rawvideo -pix_fmt rgb24 -s 1920x1080 -r 30 -i - -c:v libx264 -f rtsp rtsp://yourserver/app/stream

*/




#include <cstdio>
#include <cstring>
#include <numeric>

#include <opencv2/opencv.hpp>
#include "base/common.hpp"
#include "base/detection.hpp"

#include "utilities/args.hpp"
#include "utilities/cmdline.hpp"
#include "utilities/file.hpp"
#include "utilities/timer.hpp"

#include <axcl.h>
#include "ax_model_runner/ax_model_runner_axcl.hpp"

const int DEFAULT_IMG_H = 640;
const int DEFAULT_IMG_W = 640;
std::array<int, 2> input_size;
const char *CLASS_NAMES[] = {
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
    "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
    "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
    "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
    "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
    "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
    "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
    "hair drier", "toothbrush"};

int NUM_CLASS = 80;

const int DEFAULT_LOOP_COUNT = 1;

const float PROB_THRESHOLD = 0.45f;
const float NMS_THRESHOLD = 0.45f;
namespace ax
{
    void post_process(const ax_runner_tensor_t *output, const int nOutputSize, const cv::Mat &mat, int input_w, int input_h, const std::vector<float> &time_costs)
    {
        std::vector<detection::Object> proposals;
        std::vector<detection::Object> objects;
        timer timer_postprocess;
        for (int i = 0; i < 3; ++i)
        {
            auto feat_ptr = (float *)output[i].pVirAddr;
            int32_t stride = (1 << i) * 8;
            detection::generate_proposals_yolov8_native(stride, feat_ptr, PROB_THRESHOLD, proposals, input_w, input_h, NUM_CLASS);
        }

        detection::get_out_bbox(proposals, objects, NMS_THRESHOLD, input_h, input_w, mat.rows, mat.cols);
        fprintf(stderr, "post process cost time:%.2f ms \n", timer_postprocess.cost());
        fprintf(stderr, "--------------------------------------\n");
        auto total_time = std::accumulate(time_costs.begin(), time_costs.end(), 0.f);
        auto min_max_time = std::minmax_element(time_costs.begin(), time_costs.end());
        fprintf(stderr,
                "Repeat %d times, avg time %.2f ms, max_time %.2f ms, min_time %.2f ms\n",
                (int)time_costs.size(),
                total_time / (float)time_costs.size(),
                *min_max_time.second,
                *min_max_time.first);
        fprintf(stderr, "--------------------------------------\n");
        fprintf(stderr, "detection num: %zu\n", objects.size());

        detection::draw_objects(mat, objects, CLASS_NAMES, NULL);
    }

    bool run_model(const std::string &model, const std::vector<uint8_t> &data, const int &repeat, cv::Mat &mat, int input_h, int input_w)
    {
        // 1. init engine
        ax_runner_axcl runner;
        int ret = runner.init(model.c_str());
        if (ret != 0)
        {
            fprintf(stderr, "init ax model runner failed.\n");
            return false;
        }
        std::vector<uint8_t> indata(input_size[0] * input_size[1] * 3, 0);
        cv::Mat inmat(input_size[0], input_size[1], CV_8UC3);
        while (std::cin.read(reinterpret_cast<char*>(inmat.data), inmat.step * inmat.rows))
        {

            common::get_input_data_letterbox(inmat, indata, input_size[0], input_size[1]);
            // 2. insert input
            memcpy(runner.get_input(0).pVirAddr, indata.data(), indata.size());
            fprintf(stderr, "Engine push input is done. \n");
            fprintf(stderr, "--------------------------------------\n");

            // 8. warn up
            for (int i = 0; i < 5; ++i)
            {
                runner.inference();
            }

            // 9. run model
            std::vector<float> time_costs(repeat, 0);
            for (int i = 0; i < repeat; ++i)
            {
                ret = runner.inference();
                time_costs[i] = runner.get_inference_time();
            }

            // 10. get result
            post_process(runner.get_outputs_ptr(0), runner.get_num_outputs(), inmat, input_w, input_h, time_costs);
            std::cout.write(reinterpret_cast<char*>(inmat.data), inmat.step * inmat.rows);
            std::cout.flush();
            fprintf(stderr, "--------------------------------------\n");
        }
        
        runner.release();
        return 0;
    }
} // namespace ax

int main(int argc, char *argv[])
{
    cmdline::parser cmd;
    cmd.add<std::string>("model", 'm', "joint file(a.k.a. joint model)", true, "");
    cmd.add<std::string>("size", 'g', "input_h, input_w", false, std::to_string(DEFAULT_IMG_H) + "," + std::to_string(DEFAULT_IMG_W));

    cmd.parse_check(argc, argv);

    // 0. get app args, can be removed from user's app
    auto model_file = cmd.get<std::string>("model");

    auto model_file_flag = utilities::file_exist(model_file);

    if (!model_file_flag)
    {
        auto show_error = [](const std::string &kind, const std::string &value)
        {
            fprintf(stderr, "Input file %s(%s) is not exist, please check it.\n", kind.c_str(), value.c_str());
        };

        if (!model_file_flag)
        {
            show_error("model", model_file);
        }

        return -1;
    }

    auto input_size_string = cmd.get<std::string>("size");

    input_size = {DEFAULT_IMG_H, DEFAULT_IMG_W};

    auto input_size_flag = utilities::parse_string(input_size_string, input_size);

    if (!input_size_flag)
    {
        auto show_error = [](const std::string &kind, const std::string &value)
        {
            fprintf(stderr, "Input %s(%s) is not allowed, please check it.\n", kind.c_str(), value.c_str());
        };

        show_error("size", input_size_string);

        return -1;
    }


    // 1. print args
    fprintf(stderr, "--------------------------------------\n");
    fprintf(stderr, "model file : %s\n", model_file.c_str());
    fprintf(stderr, "img_h, img_w : %d %d\n", input_size[0], input_size[1]);
    fprintf(stderr, "--------------------------------------\n");

    // 2. read image & resize & transpose
    std::vector<uint8_t> image(input_size[0] * input_size[1] * 3, 0);
    cv::Mat mat ;
    // if (mat.empty())
    // {
    //     fprintf(stderr, "Read image failed.\n");
    //     return -1;
    // }
    

    // 3. init axcl
    {
        if (auto ret = axclInit(0); 0 != ret)
        {
            fprintf(stderr, "Init AXCL failed{0x%8x}.\n", ret);
            return -1;
        }
        axclrtDeviceList lst;
        if (const auto ret = axclrtGetDeviceList(&lst); 0 != ret || 0 == lst.num)
        {
            fprintf(stderr, "Get AXCL device failed{0x%8x}, find total %d device.\n", ret, lst.num);
            return -1;
        }
        if (const auto ret = axclrtSetDevice(lst.devices[0]); 0 != ret)
        {
            fprintf(stderr, "Set AXCL device failed{0x%8x}.\n", ret);
            return -1;
        }
        int ret = axclrtEngineInit(AXCL_VNPU_DISABLE);
        if (0 != ret)
        {
            fprintf(stderr, "axclrtEngineInit %d\n", ret);
            return ret;
        }
    }

    // 4. -  engine model
    {
        ax::run_model(model_file, image, 1, mat, input_size[0], input_size[1]);
    }

    // 5. finalize
    axclFinalize();
    return 0;
}
