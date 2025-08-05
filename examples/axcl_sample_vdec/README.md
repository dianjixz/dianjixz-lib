### vdec sample
1. Load .mp4 or .h264/h265 stream file
2. Demux nalu by ffmpeg
3. Send nalu to VDEC by frame
4. Get decodec YUV


### usage
```bash
usage: ./axcl_sample_vdec --url=string [options] ...
options:
  -i, --url       mp4|.264|.265 file path (string)
  -d, --device    device index from 0 to connected device num - 1 (unsigned int [=0])
      --count     grp count (int [=1])
      --json      axcl.json path (string [=./axcl.json])
  -w, --width     frame width (int [=1920])
  -h, --height    frame height (int [=1080])
      --VdChn     channel id (int [=0])
      --yuv       transfer nv12 from device (int [=0])
  -?, --help      print this message

--count: how many streams are decoded at same time
-w: width of decoded output nv12 image
-h: height of decoded output nv12 image
--VdChn: VDEC output channel index
      0: PP0, same width and height for input stream, cannot support scaler down.
	  1: PP1, support scale down. range: [48x48, 4096x4096]
	  2: PP2, support scale down. range: [48x48, 1920x1080]
```

### example
decode 4 streams:
```bash
$ ./axcl_sample_vdec -i bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4 -d 0 --count 4
[INFO ][                            main][  43]: ============== V2.26.1 sample started Feb 13 2025 11:10:18 ==============

[INFO ][                            main][  67]: json: ./axcl.json
[INFO ][                            main][  87]: device index: 0, bus number: 129
[INFO ][             ffmpeg_init_demuxer][ 438]: [0] url: bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4
[INFO ][             ffmpeg_init_demuxer][ 501]: [0] url bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4: codec 96, 1920x1080, fps 30
[INFO ][             ffmpeg_init_demuxer][ 438]: [1] url: bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4
[INFO ][             ffmpeg_init_demuxer][ 501]: [1] url bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4: codec 96, 1920x1080, fps 30
[INFO ][             ffmpeg_init_demuxer][ 438]: [2] url: bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4
[INFO ][             ffmpeg_init_demuxer][ 501]: [2] url bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4: codec 96, 1920x1080, fps 30
[INFO ][             ffmpeg_init_demuxer][ 438]: [3] url: bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4
[INFO ][             ffmpeg_init_demuxer][ 501]: [3] url bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4: codec 96, 1920x1080, fps 30
[INFO ][                            main][ 115]: init sys
[INFO ][                            main][ 124]: init vdec
[INFO ][                            main][ 138]: start decoder 0
[INFO ][sample_get_vdec_attr_from_stream_info][ 251]: stream info: 1920x1080 payload 96 fps 30
[INFO ][                            main][ 175]: start demuxer 0
[INFO ][ sample_get_decoded_image_thread][ 310]: [decoder  0] decode thread +++
[INFO ][          ffmpeg_dispatch_thread][ 188]: [0] +++
[INFO ][             ffmpeg_demux_thread][ 294]: [0] +++
[INFO ][                            main][ 138]: start decoder 1
[INFO ][sample_get_vdec_attr_from_stream_info][ 251]: stream info: 1920x1080 payload 96 fps 30
[INFO ][                            main][ 175]: start demuxer 1
[INFO ][ sample_get_decoded_image_thread][ 310]: [decoder  1] decode thread +++
[INFO ][          ffmpeg_dispatch_thread][ 188]: [1] +++
[INFO ][                            main][ 138]: start decoder 2
[INFO ][sample_get_vdec_attr_from_stream_info][ 251]: stream info: 1920x1080 payload 96 fps 30
[INFO ][             ffmpeg_demux_thread][ 294]: [1] +++
[INFO ][                            main][ 175]: start demuxer 2
[INFO ][ sample_get_decoded_image_thread][ 310]: [decoder  2] decode thread +++
[INFO ][          ffmpeg_dispatch_thread][ 188]: [2] +++
[INFO ][             ffmpeg_demux_thread][ 294]: [2] +++
[INFO ][                            main][ 138]: start decoder 3
[INFO ][sample_get_vdec_attr_from_stream_info][ 251]: stream info: 1920x1080 payload 96 fps 30
[INFO ][ sample_get_decoded_image_thread][ 310]: [decoder  3] decode thread +++
[INFO ][                            main][ 175]: start demuxer 3
[INFO ][          ffmpeg_dispatch_thread][ 188]: [3] +++
[INFO ][             ffmpeg_demux_thread][ 294]: [3] +++
[INFO ][             ffmpeg_demux_thread][ 327]: [0] reach eof
[INFO ][             ffmpeg_demux_thread][ 434]: [0] demuxed    total 470 frames ---
[INFO ][             ffmpeg_demux_thread][ 327]: [1] reach eof
[INFO ][             ffmpeg_demux_thread][ 434]: [1] demuxed    total 470 frames ---
[INFO ][             ffmpeg_demux_thread][ 327]: [2] reach eof
[INFO ][             ffmpeg_demux_thread][ 434]: [2] demuxed    total 470 frames ---
[INFO ][             ffmpeg_demux_thread][ 327]: [3] reach eof
[INFO ][             ffmpeg_demux_thread][ 434]: [3] demuxed    total 470 frames ---
[INFO ][          ffmpeg_dispatch_thread][ 271]: [0] dispatched total 470 frames ---
[INFO ][          ffmpeg_dispatch_thread][ 271]: [1] dispatched total 470 frames ---
[INFO ][          ffmpeg_dispatch_thread][ 271]: [2] dispatched total 470 frames ---
[INFO ][          ffmpeg_dispatch_thread][ 271]: [3] dispatched total 470 frames ---
[WARN ][ sample_get_decoded_image_thread][ 356]: [decoder  2] flow end
[INFO ][ sample_get_decoded_image_thread][ 391]: [decoder  2] total decode 470 frames
[WARN ][ sample_get_decoded_image_thread][ 356]: [decoder  1] flow end
[INFO ][ sample_get_decoded_image_thread][ 391]: [decoder  1] total decode 470 frames
[WARN ][ sample_get_decoded_image_thread][ 356]: [decoder  0] flow end
[INFO ][ sample_get_decoded_image_thread][ 391]: [decoder  0] total decode 470 frames
[INFO ][                            main][ 196]: stop decoder 0
[INFO ][ sample_get_decoded_image_thread][ 397]: [decoder  2] dfecode thread ---
[INFO ][ sample_get_decoded_image_thread][ 397]: [decoder  1] dfecode thread ---
[INFO ][ sample_get_decoded_image_thread][ 397]: [decoder  0] dfecode thread ---
[INFO ][                            main][ 201]: decoder 0 is eof
[INFO ][                            main][ 196]: stop decoder 1
[INFO ][                            main][ 201]: decoder 1 is eof
[INFO ][                            main][ 196]: stop decoder 2
[WARN ][ sample_get_decoded_image_thread][ 356]: [decoder  3] flow end
[INFO ][ sample_get_decoded_image_thread][ 391]: [decoder  3] total decode 470 frames
[INFO ][ sample_get_decoded_image_thread][ 397]: [decoder  3] dfecode thread ---
[INFO ][                            main][ 201]: decoder 2 is eof
[INFO ][                            main][ 196]: stop decoder 3
[INFO ][                            main][ 201]: decoder 3 is eof
[INFO ][                            main][ 226]: stop demuxer 0
[INFO ][                            main][ 226]: stop demuxer 1
[INFO ][                            main][ 226]: stop demuxer 2
[INFO ][                            main][ 226]: stop demuxer 3
[INFO ][                            main][ 234]: deinit vdec
[INFO ][                            main][ 238]: deinit sys
[INFO ][                            main][ 242]: axcl deinit
[INFO ][                            main][ 246]: ============== V2.26.1 sample exited Feb 13 2025 11:10:18 ==============
```
