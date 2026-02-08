#include "d_hal_nn.hpp"
#include <ax_engine_api.h>
#include <ax_sys_api.h>
#include <string.h>
#include "../../utilities/include/json.hpp"
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// #ifndef STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
// #endif
#include "private_inc/stb_image_resize2.h"

#define AX_CMM_ALIGN_SIZE 128
typedef enum {
    AX_ENGINE_ABST_DEFAULT = 0,
    AX_ENGINE_ABST_CACHED  = 1,
} AX_ENGINE_ALLOC_BUFFER_STRATEGY_T;
const char* AX_CMM_SESSION_NAME = "npu";
typedef std::pair<AX_ENGINE_ALLOC_BUFFER_STRATEGY_T, AX_ENGINE_ALLOC_BUFFER_STRATEGY_T> INPUT_OUTPUT_ALLOC_STRATEGY;
namespace middleware {

void free_io_index(AX_ENGINE_IO_BUFFER_T* io_buf, size_t index)
{
    for (size_t i = 0; i < index; ++i) {
        AX_ENGINE_IO_BUFFER_T* pBuf = io_buf + i;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
}

void free_io(AX_ENGINE_IO_T* io)
{
    for (size_t j = 0; j < io->nInputSize; ++j) {
        AX_ENGINE_IO_BUFFER_T* pBuf = io->pInputs + j;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
    for (size_t j = 0; j < io->nOutputSize; ++j) {
        AX_ENGINE_IO_BUFFER_T* pBuf = io->pOutputs + j;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
    delete[] io->pInputs;
    delete[] io->pOutputs;
}

int prepare_io(AX_ENGINE_IO_INFO_T* info, AX_ENGINE_IO_T* io_data, INPUT_OUTPUT_ALLOC_STRATEGY strategy)
{
    memset(io_data, 0, sizeof(*io_data));
    io_data->pInputs = new AX_ENGINE_IO_BUFFER_T[info->nInputSize];
    memset(io_data->pInputs, 0, sizeof(AX_ENGINE_IO_BUFFER_T) * info->nInputSize);

    io_data->nInputSize = info->nInputSize;

    auto ret = 0;
    for (AX_U32 i = 0; i < info->nInputSize; ++i) {
        auto meta   = info->pInputs[i];
        auto buffer = &io_data->pInputs[i];
        if (strategy.first == AX_ENGINE_ABST_CACHED) {
            ret = AX_SYS_MemAllocCached((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE,
                                        (const AX_S8*)(AX_CMM_SESSION_NAME));
        } else {
            ret = AX_SYS_MemAlloc((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE,
                                  (const AX_S8*)(AX_CMM_SESSION_NAME));
        }

        if (ret != 0) {
            free_io_index(io_data->pInputs, i);
            fprintf(stderr, "Allocate input{%d} { phy: %p, vir: %p, size: %lu Bytes }. fail \n", i,
                    (void*)buffer->phyAddr, buffer->pVirAddr, (long)meta.nSize);
            return ret;
        }
        // fprintf(stderr, "Allocate input{%d} { phy: %p, vir: %p, size: %lu Bytes }. \n", i, (void*)buffer->phyAddr,
        // buffer->pVirAddr, (long)meta.nSize);
    }

    io_data->pOutputs = new AX_ENGINE_IO_BUFFER_T[info->nOutputSize];
    memset(io_data->pOutputs, 0, sizeof(AX_ENGINE_IO_BUFFER_T) * info->nOutputSize);

    io_data->nOutputSize = info->nOutputSize;
    for (AX_U32 i = 0; i < info->nOutputSize; ++i) {
        auto meta     = info->pOutputs[i];
        auto buffer   = &io_data->pOutputs[i];
        buffer->nSize = meta.nSize;
        if (strategy.second == AX_ENGINE_ABST_CACHED) {
            ret = AX_SYS_MemAllocCached((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE,
                                        (const AX_S8*)(AX_CMM_SESSION_NAME));
        } else {
            ret = AX_SYS_MemAlloc((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE,
                                  (const AX_S8*)(AX_CMM_SESSION_NAME));
        }
        if (ret != 0) {
            fprintf(stderr, "Allocate output{%d} { phy: %p, vir: %p, size: %lu Bytes }. fail \n", i,
                    (void*)buffer->phyAddr, buffer->pVirAddr, (long)meta.nSize);
            free_io_index(io_data->pInputs, io_data->nInputSize);
            free_io_index(io_data->pOutputs, i);
            return ret;
        }
        // fprintf(stderr, "Allocate output{%d} { phy: %p, vir: %p, size: %lu Bytes }.\n", i, (void*)buffer->phyAddr,
        // buffer->pVirAddr, (long)meta.nSize);
    }

    return 0;
}

int push_input(const std::vector<uint8_t>& data, AX_ENGINE_IO_T* io_t, AX_ENGINE_IO_INFO_T* info_t)
{
    if (info_t->nInputSize != 1) {
        fprintf(stderr, "Only support Input size == 1 current now");
        return -1;
    }

    if (data.size() != info_t->pInputs[0].nSize) {
        fprintf(stderr, "The input data size is not matched with tensor {name: %s, size: %d}.\n",
                info_t->pInputs[0].pName, info_t->pInputs[0].nSize);
        return -1;
    }

    memcpy(io_t->pInputs[0].pVirAddr, data.data(), data.size());

    return 0;
}
}  // namespace middleware

namespace d_hal {


class d_hal_nn_ax650c_yolo26 : public d_hal_nn_yolo
{
private:
public:
    d_hal_nn_ax650c_yolo26(d_hal_nn* nn)
    {
        _nn = nn;
        init();
    };
private:
    std::vector<std::string> _yolo_class_name;
    std::vector<float> _yolo_anchors;
    int _yolo_image_width;
    int _yolo_image_height;
    int _yolo_image_type;


public:
    virtual int set_class(const std::vector<std::string> &class_name) override
    {
        _yolo_class_name = class_name;
        return 0;
    }
    virtual int set_anchors(const std::vector<float> &anchors) override
    {
        _yolo_anchors = anchors;
        return 0;
    }
    virtual int set_image_size(int width, int height, int type) override
    {
        _yolo_image_width = width;
        _yolo_image_height = height;
        _yolo_image_type = type;
        return 0;
    }
    virtual int encode_image(void *img, int width, int height, int type,int yolo_type) override
    {
        
        if(type != V4L2_PIX_FMT_RGB24)
        {
            return -1;
        }
        if((width == _yolo_image_width) && (height == _yolo_image_height)){memcpy(_io_data->pInputs[0].pVirAddr, img, _yolo_image_width * _yolo_image_height * (((V4L2_PIX_FMT_RGB24 >> 24) & 0xff) - '0'));}
        else if((width == _yolo_image_width) && (height > _yolo_image_height)){}
        else if((width == _yolo_image_width) && (height < _yolo_image_height)){}
        else if((width > _yolo_image_width) && (height == _yolo_image_height)){}
        else if((width > _yolo_image_width) && (height > _yolo_image_height)){}
        else if((width > _yolo_image_width) && (height < _yolo_image_height)){}
        else if((width < _yolo_image_width) && (height == _yolo_image_height)){}
        else if((width < _yolo_image_width) && (height > _yolo_image_height)){}
        else if((width < _yolo_image_width) && (height < _yolo_image_height)){}
        return 0;
    }
    virtual int infer_with_decode_object(int yolo_type, float prob_threshold, float nms_threshold, std::shared_ptr<void> &result) override
    {
        int ret;
        std::shared_ptr<std::vector<void*>> io;
        _nn->infer(nullptr, io);
        return 0;
    }





    virtual ~d_hal_nn_ax650c_yolo26()
    {
        deinit();
    };
};











bool endsWith(const std::string& str, const std::string& suffix)
{
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

class d_hal_nn_ax650c : public d_hal_nn {
private:
    static int hardware_initialized;
    AX_ENGINE_HANDLE handle;
    AX_ENGINE_IO_INFO_T* _io_info;
    AX_ENGINE_IO_T* _io_data;

public:
    void* _out_io;

public:
    d_hal_nn_ax650c(D_HAL_NN_TYPE_E type)
    {
        _type = type;
        init();
    };

    static int hardware_init()
    {
        if (hardware_initialized == 0) {
            AX_ENGINE_NPU_ATTR_T npu_attr;
            memset(&npu_attr, 0, sizeof(npu_attr));
            npu_attr.eHardMode = AX_ENGINE_VIRTUAL_NPU_DISABLE;
            AX_ENGINE_Init(&npu_attr);
        }
        hardware_initialized++;
        return 0;
    }
    static int hardware_deinit()
    {
        hardware_initialized--;
        if (hardware_initialized == 0) {
            AX_ENGINE_Deinit();
        }
        return 0;
    }

    void with_read_file(const std::string& path, std::function<void(const char*, size_t)> func)
    {
        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("Error opening file");
        }
        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            close(fd);
            throw std::runtime_error("Error getting file size");
        }
        off_t filesize = sb.st_size;
        char* mapped   = (char*)mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapped == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Error mapping file");
        }
        func(mapped, filesize);
        munmap(mapped, filesize);
        close(fd);
    }

    virtual int init() override
    {
        _out_io = NULL;
        _out_io = malloc(sizeof(void*) * 2);
        _io_data = (AX_ENGINE_IO_T*)malloc(sizeof(AX_ENGINE_IO_T));
        memset(&_io_data, 0, sizeof(_io_data));
        hardware_init();
        return 0;
    }
    virtual int deinit() override
    {
        hardware_deinit();
        free(_out_io);
        free(_io_data);
        _out_io = NULL;
        return 0;
    }

    virtual int load_model(const std::string& model) override
    {
        int ret;
        std::string model_path = model;
        if (endsWith(model, ".json")) {
            nlohmann::json config;
            std::ifstream file(model);
            if (!file) {
                throw std::runtime_error("Error opening file: " + model);
            }
            try {
                file >> config;
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Error parsing JSON: ") + e.what());
            }
            if (!config.contains("MODEL_PATH") || !config["MODEL_PATH"].is_string()) {
                throw std::runtime_error("Missing or invalid 'MODEL_PATH' field in json");
            }
            model_path = config["MODEL_PATH"];
            if (model_path.empty()) {
                throw std::runtime_error("Model Path cannot be empty");
            }
        }
        with_read_file(model_path, [this](const char* data, size_t size) {
            int ret = AX_ENGINE_CreateHandle(&handle, data, size);
            if (0 != ret) {
                // fprintf(stderr, "AX_ENGINE_CreateHandle failed, ret = 0x%x\n", ret);
                throw std::runtime_error("Error creating engine handle");
            }
        });
        ret = AX_ENGINE_CreateContext(handle);
        if (0 != ret) {
            // fprintf(stderr, "AX_ENGINE_CreateContext failed, ret = 0x%x\n", ret);
            AX_ENGINE_DestroyHandle(handle);
            throw std::runtime_error("Error creating engine context");
        }
        ret = AX_ENGINE_GetIOInfo(handle, &_io_info);
        if (0 != ret) {
            // fprintf(stderr, "AX_ENGINE_GetIOInfo failed, ret = 0x%x\n", ret);
            AX_ENGINE_DestroyHandle(handle);
            handle = nullptr;
            throw std::runtime_error("Error getting engine IO info");
        }
        // int a =  V4L2_PIX_FMT_YUYV;
        ret =
            middleware::prepare_io(_io_info, _io_data, std::make_pair(AX_ENGINE_ABST_DEFAULT, AX_ENGINE_ABST_CACHED));
        if (0 != ret) {
            // fprintf(stderr, "prepare_io failed, ret = 0x%x\n", ret);
            AX_ENGINE_DestroyHandle(handle);
            handle = nullptr;
            throw std::runtime_error("Error preparing engine IO");
        }
        return 0;
    }
    virtual int request_infer_mem(void** mem) override
    {
        *mem = _io_data->pInputs[0].pVirAddr;
        return _io_info->pInputs[0].nSize;
    }
    virtual int request_infer_mem_size() override
    {
        return _io_info->pInputs[0].nSize;
    }

    virtual int infer(const void* in, std::shared_ptr<std::vector<void*>>& io) override
    {
        AX_S32 ret;
        if (in != nullptr && ( in != _io_data->pInputs[0].pVirAddr)) {
            memcpy(_io_data->pInputs[0].pVirAddr, in, _io_info->pInputs[0].nSize);
        }
        ret = AX_ENGINE_RunSync(handle, _io_data);
        if (0 != ret) {
            fprintf(stderr, "AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
            return -1;
        }

        io->resize(2);
        (*io)[0] = _io_data;
        (*io)[1] = _io_info;
        return 0;
    }
    // virtual int decode_yolo(const std::shared_ptr<std::vector<void*>>& io, int yolo_type,
    //                         std::shared_ptr<void>& result) override
    // {
    //     AX_ENGINE_IO_T* io_data      = static_cast<AX_ENGINE_IO_T*>((*io)[0]);
    //     AX_ENGINE_IO_INFO_T* io_info = static_cast<AX_ENGINE_IO_INFO_T*>((*io)[1]);

    //     // Decode YOLO results specific to AX650C
    //     return 0;
    // }
    virtual std::shared_ptr<void> get_param(const std::string& param_name) override
    {
        // Get parameter code specific to AX650C
        return nullptr;
    }

    virtual int set_param(const std::string& param_name, const std::shared_ptr<void>& value) override
    {
        // Set parameter code specific to AX650C
        return 0;
    }




private:
    std::vector<std::string> _yolo_class_name;
    std::vector<float> _yolo_anchors;
    int _yolo_image_width;
    int _yolo_image_height;
    int _yolo_image_type;


public:
    virtual int yolo_set_class(const std::vector<std::string> &class_name) override
    {
        _yolo_class_name = class_name;
        return 0;
    }
    virtual int yolo_set_anchors(const std::vector<float> &anchors) override
    {
        _yolo_anchors = anchors;
        return 0;
    }
    virtual int yolo_set_image_size(int width, int height, int type) override
    {
        _yolo_image_width = width;
        _yolo_image_height = height;
        _yolo_image_type = type;
        return 0;
    }
    virtual int yolo_encode_image(void *img, int width, int height, int type,int yolo_type) override
    {
        
        if(type != V4L2_PIX_FMT_RGB24)
        {
            return -1;
        }
        if((width == _yolo_image_width) && (height == _yolo_image_height)){memcpy(_io_data->pInputs[0].pVirAddr, img, _yolo_image_width * _yolo_image_height * (((V4L2_PIX_FMT_RGB24 >> 24) & 0xff) - '0'));}
        else if((width == _yolo_image_width) && (height > _yolo_image_height)){}
        else if((width == _yolo_image_width) && (height < _yolo_image_height)){}
        else if((width > _yolo_image_width) && (height == _yolo_image_height)){}
        else if((width > _yolo_image_width) && (height > _yolo_image_height)){}
        else if((width > _yolo_image_width) && (height < _yolo_image_height)){}
        else if((width < _yolo_image_width) && (height == _yolo_image_height)){}
        else if((width < _yolo_image_width) && (height > _yolo_image_height)){}
        else if((width < _yolo_image_width) && (height < _yolo_image_height)){}
        return 0;
    }
    virtual int yolo_infer_with_decode_object(int yolo_type, float prob_threshold, float nms_threshold, std::shared_ptr<void> &result) override
    {
        int ret;
        ret = AX_ENGINE_RunSync(handle, _io_data);
        if (0 != ret) {
            fprintf(stderr, "AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
            return -1;
        }
        return 0;
    }











    virtual ~d_hal_nn_ax650c()
    {
        deinit();
    };
};

int d_hal_nn_ax650c::hardware_initialized = 0;

std::shared_ptr<d_hal_nn> create_d_hal_nn(D_HAL_NN_TYPE_E type)
{
    if (type == D_HAL_NN_AXERA_AX650C) {
        return std::make_shared<d_hal_nn_ax650c>(type);
    }

    return nullptr;
}
}  // namespace  d_hal