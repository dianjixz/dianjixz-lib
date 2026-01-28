// 用于解码器配置
#include "hal_venc.hpp"
#include <ax_sys_api.h>
#include <ax_venc_api.h>
#include <string.h>

class jpeg_encode : public venc_encode {
private:
    hal_venc& venc_dev;
    AX_U64 _in_Phy;
    AX_VOID* _in_Vir;
    uint32_t _in_Size;
    AX_U64 _out_Phy;
    AX_VOID* _out_Vir;
    uint32_t _out_Size;
    VENC_CHN _VeChn;
    AX_VENC_CHN_ATTR_T stChnAttr;
public:
    jpeg_encode(hal_venc& dev): venc_dev(dev)
    {
        _in_Phy    = 0;
        _in_Vir    = nullptr;
        _in_Size   = 0;
        _out_Phy   = 0;
        _out_Vir   = nullptr;
        _out_Size  = 0;
        _VeChn     = -1;
        id_info[0] = AX_ID_JENC;
        id_info[1] = 0;
        id_info[2] = 0;
    }
    virtual int set_encode_frame(int width, int height, int format, bool malloc_in = false,
                                 bool malloc_out = false) override
    {
        int ret;
        _in_width  = width;
        _in_height = height;
        _in_format = format;
        if (malloc_in) {
            ret = malloc_encode_in_frame();
            if (ret) {
                return ret;
            }
        }
        if (malloc_out) {
            ret = malloc_encode_out_frame();
            if (ret) {
                if (_in_Phy != 0 && _in_Vir != nullptr) {
                    AX_SYS_MemFree(_in_Phy, _in_Vir);
                    _in_Phy = 0;
                    _in_Vir = nullptr;
                }
                return ret;
            }
        }
        return 0;
    }
    virtual int malloc_encode_in_frame() override
    {
        _in_Size = get_format_size(_in_format, _in_width, _in_height);  // 修复：参数顺序
        return AX_SYS_MemAlloc(&_in_Phy, &_in_Vir, _in_Size, 0, (AX_S8*)"JENC_IN");
    }
    virtual int malloc_encode_out_frame() override
    {
        _out_Size = get_format_size(_in_format, _in_width, _in_height);  // 修复：参数顺序
        return AX_SYS_MemAllocCached(&_out_Phy, &_out_Vir, _out_Size, 0, (AX_S8*)"JENC_OUT");
    }
    virtual void free_encode_in_frame() override
    {
        if (_in_Phy != 0 && _in_Vir != nullptr) {
            AX_SYS_MemFree(_in_Phy, _in_Vir);
            _in_Phy = 0;
            _in_Vir = nullptr;
        }
    }
    virtual void free_encode_out_frame() override
    {
        if (_out_Phy != 0 && _out_Vir != nullptr) {
            AX_SYS_MemFree(_out_Phy, _out_Vir);
            _out_Phy = 0;
            _out_Vir = nullptr;
        }
    }
    virtual int malloc_encode_chn() override
    {
        if (_VeChn >= 0) return 0;
        for (int i = 0; i < MAX_VENC_CHN_NUM; i++) {
            AX_S32 ret = AX_VENC_CreateChn(i, &stChnAttr);
            if (ret == AX_SUCCESS) {
                _VeChn     = i;
                id_info[2] = i;
                return 0;
            }
        }
        return -1;
    }
    virtual void free_encode_chn() override
    {
        if (_VeChn >= 0) {
            AX_VENC_DestroyChn(_VeChn);
            _VeChn = -1;
        }
    }
    virtual void set_encode_fps(int fps) override
    {
        _fps = fps;
    }
    virtual int encode_frame(int qfactor) override  // 修复：拼写错误 fqactor -> qfactor
    {
        if (_VeChn < 0) {  // 编码一帧
            return encode_frame_once(qfactor);
        } else {  // 通道编码
            return -1;
        }
    }
    virtual int encode_frame_once(int qfactor) override
    {
        AX_JPEG_ENCODE_ONCE_PARAMS_T stJpegParam;
        memset(&stJpegParam, 0, sizeof(stJpegParam));
        stJpegParam.u32Width               = _in_width;
        stJpegParam.u32Height              = _in_height;
        stJpegParam.enImgFormat            = (AX_IMG_FORMAT_E)_in_format;
        stJpegParam.stJpegParam.u32Qfactor = (AX_U32)qfactor;
        // stJpegParam.stJpegParam.bDblkEnable = AX_FALSE;
        stJpegParam.u64PhyAddr[0] = _in_Phy;
        stJpegParam.u64PhyAddr[1] = _in_Phy + _in_width * _in_height;
        stJpegParam.u64PhyAddr[2] = 0;
        stJpegParam.u32PicStride[0] = _in_width;
        stJpegParam.u32PicStride[1] = _in_width;
        stJpegParam.u32PicStride[2] = 0;
        stJpegParam.enStrmBufType = AX_STREAM_BUF_CACHE;
        stJpegParam.ulPhyAddr     = _out_Phy;
        stJpegParam.pu8Addr       = (AX_U8*)_out_Vir;
        stJpegParam.u32Len        = _out_Size;
        int ret                   = AX_VENC_JpegEncodeOneFrame(&stJpegParam);
        if (ret == 0) {
            if (frame_put_func) {
                frame_put_func(_out_Phy, _out_Vir, stJpegParam.u32Len, _in_format);
            }
        }
        return ret;  // 修复：添加返回值
    }
    virtual int work() override
    {
        return 0;  // 修复：添加返回值
    }
    virtual int destroy() override
    {
        free_encode_chn();
        free_encode_in_frame();
        free_encode_out_frame();
    }
    ~jpeg_encode()
    {
        destroy();
        venc_dev.destroy(_handle_index);
    }
};

template <class T>
std::shared_ptr<T> venc_encode::link_id()
{
    auto link_info = std::make_shared<T>(id_info[0], id_info[1], id_info[2]);
    return std::static_pointer_cast<T>(link_info);
}

venc_encode::venc_encode()
{
    
}

venc_encode::~venc_encode()  // 修复：添加析构函数实现
{
    
}

hal_venc::hal_venc(const hal_device& device)
{
    sys = device.get_sys_guard();
    AX_VENC_MOD_ATTR_T stModAttr;
    memset(&stModAttr, 0, sizeof(stModAttr));
    stModAttr.enVencType                     = AX_VENC_MULTI_ENCODER;
    stModAttr.stModThdAttr.u32TotalThreadNum = 1;
    stModAttr.stModThdAttr.bExplicitSched    = AX_FALSE;
    AX_S32 ret                               = AX_VENC_Init(&stModAttr);
}

hal_venc::hal_venc(const hal_device& device, int type)
{
    sys = device.get_sys_guard();
    AX_VENC_MOD_ATTR_T stModAttr;
    memset(&stModAttr, 0, sizeof(stModAttr));
    stModAttr.enVencType                     = (AX_VENC_ENCODER_TYPE_E)type;
    stModAttr.stModThdAttr.u32TotalThreadNum = 1;
    stModAttr.stModThdAttr.bExplicitSched    = AX_FALSE;
    AX_S32 ret                               = AX_VENC_Init(&stModAttr);
}

hal_venc::hal_venc(const hal_device& device, int type, int thread_num, int b_sched)
{
    sys = device.get_sys_guard();
    AX_VENC_MOD_ATTR_T stModAttr;
    memset(&stModAttr, 0, sizeof(stModAttr));
    stModAttr.enVencType                     = (AX_VENC_ENCODER_TYPE_E)type;
    stModAttr.stModThdAttr.u32TotalThreadNum = thread_num;
    stModAttr.stModThdAttr.bExplicitSched    = b_sched ? AX_TRUE : AX_FALSE;
    AX_S32 ret                               = AX_VENC_Init(&stModAttr);
}

std::shared_ptr<venc_encode> hal_venc::creat(int type)
{
    switch (type) {
        case TYPE_JPEG: {
            auto encoder_jpeg = std::make_shared<jpeg_encode>(*this);
            encoder_jpeg->_handle_index = encoder_count++;
            encoder_list.push_back(encoder_jpeg);
            return encoder_jpeg;
        } break;
        default:
            break;
    }
    return nullptr;
}

hal_venc::~hal_venc()
{
    for (auto it = encoder_list.begin(); it != encoder_list.end(); it++) {
        (*it)->destroy();
    }
    encoder_list.clear();
    AX_VENC_Deinit();
    sys.reset();
}

