### Description
​	This module is part of the SDK package and provides sample code for the video encoding unit (H.264, H.265, JPEG, MJPEG). It aims to help customers quickly understand and master the usage of video encoding-related interfaces. The code demonstrates the following processes: initialization of the video encoding module, sending frame data via an encoding Send thread, obtaining and saving encoded stream data via an encoding Get thread, and deinitialization of the video encoding module.

 After compilation, the executable `axcl_sample_venc` is located in the /opt/bin/axcl directory and can be used to verify video encoding functionality.

- **-w**: Configure the source data width.
- **-h**: Configure the source data height.
- **-i**: Path to the input source data.
- **-l**: Input source data YUV format (1: I420; 3: NV12; 4: NV21; 13: YUYV422; 14: UYVY422). Default is 1.
- **-N**: Configure the number of encoding channels. By default, it enables four channels for encoding H.264, H.265, MJPEG, and JPEG.
- **-n**: When loop encoding is enabled, specify the number of frames to encode.
- **-W**: Whether to write the encoded stream to a file (default is 1, which means writing to a file. 0: do not write).

> [!NOTE]
>
> - Some parameters in the example code may not be optimal and are only intended for API demonstration. In actual development, users need to configure parameters according to specific business scenarios.
> - H.264/H.265 support a maximum resolution of 8192x8192.
> - JPEG/MJPEG support a maximum resolution of 16384x16384.

### Examples
Upon successful execution, press Ctrl+C to exit. Stream files should be generated in the current directory with extensions like .264, .265, .jpg, or .mjpg. Users can open these files to view the actual results.

1. View help information
   ```bash
   axcl_sample_venc -H
   ```

2. Enable two channels to encode 1080p NV12 format (Channel 0: H.264, Channel 1: H.265)
   ```bash
   axcl_sample_venc -w 1920 -h 1080 -i 1080p_nv12.yuv -N 2 -l 3
   ```

3. Enable two channels to loop encode 3840x2160 NV21 format (Channel 0: H.264, Channel 1: H.265), encoding 10 frames
   ```bash
   axcl_sample_venc -w 3840 -h 2160 -i 3840x2160_nv21.yuv -N 2 -l 4 -n 10
   ```

4. Encode one MJPEG stream with resolution 1920x1080, YUV420P format, encoding 5 frames
   ```bash
   axcl_sample_venc -w 1920 -h 1080 -i 1920x1080_yuv420p.yuv -N 1 --bChnCustom 1 --codecType 2 -l 1 -n 5
   ```


### 描述

本模块是 SDK 包的一部分，提供了视频编码单元（H.264、H.265、JPEG、MJPEG）的示例代码，旨在帮助客户快速理解并掌握与视频编码相关接口的使用方法。代码演示了以下流程：视频编码模块初始化，通过编码发送线程（Send 线程）发送帧数据，通过编码获取线程（Get 线程）获取并保存编码后的流数据，以及编码模块的反初始化。

编译完成后，可执行文件 `axcl_sample_venc` 位于 /opt/bin/axcl 目录下，可用于验证视频编码功能。

- **-w**：配置源数据宽度。
- **-h**：配置源数据高度。
- **-i**：输入源数据的路径。
- **-l**：输入源数据的 YUV 格式（1：I420；3：NV12；4：NV21；13：YUYV422；14：UYVY422）。默认值为 1。
- **-N**：配置编码通道数量，默认开启四个通道，分别编码 H.264、H.265、MJPEG 和 JPEG。
- **-n**：在开启循环编码时，指定编码的帧数。
- **-W**：是否将编码流写入文件（默认值为 1，写文件；0 表示不写）。

> [!注意]
>
> - 示例代码中的部分参数不一定是最优配置，仅用于 API 演示。实际开发中需根据具体业务场景调整参数。
> - H.264/H.265 最大支持分辨率为 8192x8192。
> - JPEG/MJPEG 最大支持分辨率为 16384x16384。

### 示例

运行成功后，按 Ctrl+C 退出。将在当前目录下生成扩展名为 .264、.265、.jpg 或 .mjpg 的码流文件，用户可用播放器查看实际效果。

1. 查看帮助信息
   ```bash
   axcl_sample_venc -H
   ```
2. 启用两路通道，编码 1080p NV12 格式（通道 0：H.264，通道 1：H.265）
   ```bash
   axcl_sample_venc -w 1920 -h 1080 -i 1080p_nv12.yuv -N 2 -l 3
   ```
3. 启用两路通道，循环编码 3840x2160 NV21 格式（通道 0：H.264，通道 1：H.265），编码 10 帧
   ```bash
   axcl_sample_venc -w 3840 -h 2160 -i 3840x2160_nv21.yuv -N 2 -l 4 -n 10
   ```
4. 编码一路分辨率为 1920x1080、YUV420P 格式的 MJPEG 流，编码 5 帧
   ```bash
   axcl_sample_venc -w 1920 -h 1080 -i 1920x1080_yuv420p.yuv -N 1 --bChnCustom 1 --codecType 2 -l 1 -n 5
   ```
```