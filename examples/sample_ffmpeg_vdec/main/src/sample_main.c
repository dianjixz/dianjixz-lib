/**********************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **********************************************************************************/


 #include <stdio.h>
 #include <stdlib.h>
 #include <signal.h>
 #include <assert.h>
 #include <pthread.h>
 #include <unistd.h>
 #include <string.h>
 #include <sys/time.h>
 #include <time.h>
 
 #include "ax_base_type.h"
 #include "ax_global_type.h"
 #include "axcl.h"
 
 #include "libavcodec/codec_id.h"
 #include "libavcodec/avcodec.h"
 #include "libavformat/avformat.h"
 #include "libavutil/opt.h"
 #include "libavutil/pixdesc.h"
 
 
 static AX_S32 gLoopExit = 0;
 static AX_S32 gWriteFrames = 0;
 
 struct timeval Timebegin_ff;
 struct timeval Timeend_ff;
 int frame_num = 0;
 
 #define SIZE_ALIGN(x,align) ((((x)+(align)-1)/(align))*(align))
 
 #define SAMPLE_ERR_LOG(str, arg...)  do { \
         printf("[FFMPEG_SAMPLE][AXCL_VDEC][ERROR][%s][line:%d]"str"\n", \
                 __func__, __LINE__, ##arg); \
     } while (0)
 
 static void SigInt(int sigNo)
 {
     printf("Catch signal %d\n", sigNo);
     gLoopExit = 1;
 }
 
 static void PrintHelp()
 {
     printf("usage: sample_vdec streamFile <args>\n");
     printf("args:\n");
     printf("  -c:       decoder name.               h264_axdec or hevc_axdec\n");
     printf("  -i:       input stream file.          need decoding stream file\n");
     printf("  -y:       write YUV frame to file.    (0: not write, others: write), dult: 0\n");
     printf("  -r:       decoder resize, just scaler down function.       widthxheight\n");
     printf("  -d:       device index from 0 to connected device num - 1, default, 0, range(0, AXCL_MAX_DEVICE_COUNT - 1)\n");
     printf("  -v:       set logging level                 \n");
 }
 #define FLAGS AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_DECODING_PARAM
 
 static enum AVPixelFormat get_format(AVCodecContext *s, const enum AVPixelFormat *pix_fmts)
 {
     const enum AVPixelFormat *p;
 
     for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
         const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(*p);
 
         if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL))
             break;
     }
 
     return *p;
 }
 
 int main(int argc, char *argv[])
 {
     AX_S32 c;
     AX_S32 isExit = 0;
     AX_S32 s32Ret = -1;
     FILE *fp_out = NULL;
     AX_S32 s32VideoIndex = 0;
     AVFormatContext *pstAvFmtCtx;
     AVPacket *pstAvPkt;
     int ret;
     int i = 0;
     int log_level = 0x1000;
     AX_CHAR device_index[16] = "0";
     const AVCodec *codec = NULL;
     AVCodecParameters *origin_par = NULL;
     AVCodecContext *avctx = NULL;
     AVFrame *frame = NULL;
 
     AX_CHAR ps8StreamFile[256];
     AX_CHAR codec_names[128];
     AX_CHAR resize[16] = "1280x720";
     enum AVCodecID eCodecID = AV_CODEC_ID_H264;
 
     size_t slen = 0;
     int len;
     AX_VOID *p_lu = NULL;
     AX_VOID *p_ch = NULL;
     AX_BOOL resize_class = 0;
     AVDictionary *codec_opts = malloc(sizeof(AVDictionary *));
 
     signal(SIGINT, SigInt);
     signal(SIGQUIT, SigInt);
     signal(SIGTERM, SigInt);
 
     while ((c = getopt(argc, argv, "c:i:y:r:d:v:h")) != -1) {
         isExit = 0;
         switch (c) {
         case 'c':
            slen = strlen(optarg);
 
             len = snprintf(codec_names, slen + 1, "%s", optarg);
             if ((len < 0) || (len != slen)) {
                 printf("snprintf FAILED, len:%d optarg:%s slen:%ld\n",
                                 len, optarg, slen);
             }
             break;
         case 'i':
             slen = strlen(optarg);
 
             len = snprintf(ps8StreamFile, slen + 1, "%s", optarg);
             if ((len < 0) || (len != slen)) {
                 printf("snprintf FAILED, len:%d optarg:%s slen:%ld\n",
                                 len, optarg, slen);
             }
             break;
         case 'y':
             gWriteFrames = atoi(optarg);
             break;
         case 'r':
             slen = strlen(optarg);
 
             len = snprintf(resize, slen + 1, "%s", optarg);
             if ((len < 0) || (len != slen)) {
                 printf("snprintf FAILED, len:%d optarg:%s slen:%ld\n",
                                 len, optarg, slen);
             }
             resize_class = 1;
             break;
         case 'd':
             slen = strlen(optarg);
             len = snprintf(device_index, slen + 1, "%s", optarg);
             if ((len < 0) || (len != slen)) {
                 printf("snprintf FAILED, len:%d optarg:%s slen:%ld\n",
                        len, optarg, slen);
             }
             break;
         case 'v':
             log_level = atoi(optarg);
             break;
         case 'h':
             isExit = 1;
             break;
         default:
             isExit = 1;
             break;
         }
     }
 
     if (isExit || (argc < 2)) {
         PrintHelp();
         exit(0);
     }
 
     if (!strcmp(codec_names, "h264_axdec")) {
         eCodecID = AV_CODEC_ID_H264;
     } else if (!strcmp(codec_names, "hevc_axdec")) {
         eCodecID = AV_CODEC_ID_HEVC;
     } else {
         SAMPLE_ERR_LOG("Not set codec name\n");
         PrintHelp();
         exit(0);
     }
 
     pstAvFmtCtx = avformat_alloc_context();
     if (pstAvFmtCtx == NULL) {
         SAMPLE_ERR_LOG("avformat_alloc_context() failed!");
         s32Ret = AVERROR_UNKNOWN;
         goto ERR_RET;
     }
 
     ret = avformat_open_input(&pstAvFmtCtx, ps8StreamFile, NULL, NULL);
     if (ret < 0) {
         AX_CHAR szError[64] = {0};
         av_strerror(ret, szError, 64);
         SAMPLE_ERR_LOG("open %s fail, error: %d, %s", ps8StreamFile, ret, szError);
         goto ERR_RET1;
     }
 
     ret = avformat_find_stream_info(pstAvFmtCtx, NULL);
     if (ret < 0) {
         SAMPLE_ERR_LOG("avformat_find_stream_info fail, error = %d", ret);
         goto ERR_RET1;
     }
 
     for (i = 0; i < pstAvFmtCtx->nb_streams; i++) {
         if (AVMEDIA_TYPE_VIDEO == pstAvFmtCtx->streams[i]->codecpar->codec_type) {
             s32VideoIndex = i;
             break;
         }
     }
 
     pstAvPkt = av_packet_alloc();
     if (!pstAvPkt) {
         SAMPLE_ERR_LOG("av_packet_alloc failed \n");
         goto ERR_RET1;
     }
 
     origin_par = pstAvFmtCtx->streams[s32VideoIndex]->codecpar;
 
     codec = avcodec_find_decoder_by_name(codec_names);
     if (!codec) {
         SAMPLE_ERR_LOG("avcodec_find_decoder error\n");
         s32Ret = AVERROR_UNKNOWN;
         goto ERR_RET2;
     }
     avctx = avcodec_alloc_context3(codec);
     if (!avctx) {
         SAMPLE_ERR_LOG("Can't allocate decoder context\n");
         s32Ret = AVERROR_UNKNOWN;
         goto ERR_RET2;
     }
 
     ret = avcodec_parameters_to_context(avctx, origin_par);
     if (ret) {
         SAMPLE_ERR_LOG("avcodec_parameters_to_context error\n");
         s32Ret = AVERROR_UNKNOWN;
         goto ERR_RET3;
     }
 
     avctx->thread_count = 2;
     avctx->thread_type = FF_THREAD_FRAME;
     avctx->debug = log_level;
     avctx->max_pixels = avctx->width * avctx->height * 3 / 2;
     avctx->get_format = get_format;
 
     if (resize_class) {
         av_dict_set(&codec_opts, "resize", resize, FLAGS);
     }
 
     /* Here set child card device index */
     av_dict_set(&codec_opts, "d", device_index, FLAGS);
 
     ret = avcodec_open2(avctx, codec, &codec_opts);
     if (ret < 0) {
         SAMPLE_ERR_LOG("Can't open decoder\n");
         s32Ret = AVERROR_UNKNOWN;
         goto ERR_RET3;
     }
 
     frame = av_frame_alloc();
     if (!frame) {
         SAMPLE_ERR_LOG("Can't allocate frame\n");
         s32Ret = AVERROR_UNKNOWN;
         goto ERR_RET3;
     }
 
     if (gWriteFrames) {
         fp_out = fopen("file.yuv", "w");
         if (fp_out == NULL) {
             SAMPLE_ERR_LOG("Unable to open output file\n");
             goto ERR_RET4;
         }
     }
 
     gettimeofday(&Timebegin_ff, NULL);
 
     while (1) {
         ret = av_read_frame(pstAvFmtCtx, pstAvPkt);
         if (ret < 0) {
             if (AVERROR_EOF == ret) {
                 pstAvPkt->size = 0;
                 pstAvPkt->data = NULL;
             } else {
                 SAMPLE_ERR_LOG("av_read_frame fail, error: %d", ret);
                 s32Ret = AVERROR_UNKNOWN;
                 break;
             }
         }
 
         avctx->codec_type = AVMEDIA_TYPE_VIDEO;
         avctx->codec_id = eCodecID;
         avctx->width = avctx->width;
         avctx->height = avctx->height;
         avctx->coded_width = avctx->width;
         avctx->coded_height = avctx->height;
         frame->width = avctx->width;
         frame->height = avctx->height;
         frame->format = AV_PIX_FMT_NONE;
 
         ret = avcodec_send_packet(avctx, pstAvPkt);
         if (ret == AVERROR(EAGAIN)) {
             SAMPLE_ERR_LOG("avcodec_send_packet error\n");
             break;
         } else if (ret == AVERROR_EOF) {
             break;
         }
 
         av_packet_unref(pstAvPkt);
 
         while (ret >= 0) {
             ret = avcodec_receive_frame(avctx, frame);
             if (ret == AVERROR_EOF) {
                 break;
             }
             else if (ret == AVERROR(EAGAIN)) {
                 ret = 0;
                 break;
             }
 
             if (gWriteFrames) {
                 p_lu = frame->data[0];
                 for (i = 0; i < frame->height; i++) {
                     fwrite(p_lu, 1, frame->width, fp_out);
                     p_lu += frame->linesize[0];
                 }
 
                 p_ch = frame->data[1];
                 for (i = 0; i < frame->height / 2; i++) {
                     fwrite(p_ch, 1, frame->width, fp_out);
                     p_ch += frame->linesize[1];
                 }
                 fflush(fp_out);
             }
             av_frame_unref(frame);
 
             frame_num ++;
         }
     }
 
     gettimeofday(&Timeend_ff, NULL);
     AX_U32 total_usec_f = 1000000 * (Timeend_ff.tv_sec - Timebegin_ff.tv_sec)
                           + Timeend_ff.tv_usec - Timebegin_ff.tv_usec;
     float total_msec_f = (float)total_usec_f / 1000.f;
     if (!frame_num) {
         SAMPLE_ERR_LOG("no frame\n");
         goto ERR_RET5;
     }
 
     float msec_per_f = total_msec_f / (float)frame_num;
     printf("msec per frame: %.1f ms, AVG FPS: %.1f fps frame_num:%d\n",
            msec_per_f, 1000.f / msec_per_f, frame_num);
 
     av_frame_free(&frame);
     av_packet_free(&pstAvPkt);
     avcodec_free_context(&avctx);
     avformat_free_context(pstAvFmtCtx);
 
     if (fp_out) {
         fclose(fp_out);
         fp_out = NULL;
     }
 
     return 0;
 
 ERR_RET5:
     if (fp_out) {
         fclose(fp_out);
         fp_out = NULL;
     }
 ERR_RET4:
     av_frame_free(&frame);
 ERR_RET3:
     avcodec_free_context(&avctx);
 ERR_RET2:
     av_packet_free(&pstAvPkt);
 ERR_RET1:
     avformat_free_context(pstAvFmtCtx);
 ERR_RET:
     return s32Ret;
 }
 