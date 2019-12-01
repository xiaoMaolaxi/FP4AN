//
// Created by PLX on 2019/11/10.
//
#include <Use_FFMPEG.h>
#include "../../../../include/libavformat/avformat.h"
#include "Use_FFMPEG.h"
#include "../../../../include/libavcodec/avcodec.h"

int Init_FP_Context(const char* url, FP_Context& FP_Ct, bool isHwDecode)
{

    int ret;
    FP_Ct.fileURL = url;
    AVFormatContext **fmt_ctx = &FP_Ct.fmt_ctx;
    int SwsOutHeight = 1920;
    int SwsOutWidth  = 1080;
    FP_Ct.Vrgba = new char[1920*1080*4];
    FP_Ct.Apcm  = new char[48000*2*4];

    av_register_all();
    avcodec_register_all();
    avformat_network_init();

    ret = avformat_open_input(&(*fmt_ctx), url, NULL, NULL);
    if(ret != 0)
        goto ERR_END;
    FFLOGE("avformat_open_input, %s", FP_Ct.fileURL);

    ret = avformat_find_stream_info((*fmt_ctx), NULL);
    if(ret != 0)
        goto ERR_END;

    // default use sw decode
    ret = Init_FP_Codec(FP_Ct, isHwDecode);
    if(ret != 0)
        goto ERR_END;
    //av_log_set_callback(log_callback_test2);
    //av_dump_format(fmt_ctx, 0, url, 0);
    //av_dump_format(fmt_ctx, 1, url, 0);


    if(!avcodec_is_open(FP_Ct.VCodecC))
    {
        FFLOGE("1VCodecC not open, info:%s", av_err2str(ret));
        return 0;
    }

    ret = Init_FP_SwAudiocover(FP_Ct,
                               av_get_default_channel_layout(FP_Ct.ACodecC->channels),
                               AV_SAMPLE_FMT_S16, FP_Ct.ACodecC->sample_rate);
    if(ret != 0)
        goto ERR_END;
    FFLOGE("fmt_ctx->duration:%lld, fmt_ctx->nb_streams:%d", (*fmt_ctx)->duration, (*fmt_ctx)->nb_streams);
    FFLOGE("Init_FP_Context done, isHwDecode:%d", isHwDecode);
    return ret;

ERR_END:
        FFLOGE("we got err msg, %s", av_err2str(ret));
        QuitAndRelease_FP(FP_Ct);
        return ret;
}
void QuitAndRelease_FP(FP_Context& FP_Ct)
{
    FFLOGE("QuitAndRelease_FP enter");
    if(FP_Ct.Vrgba != NULL)
    {
        delete []FP_Ct.Vrgba;
    }
    if(FP_Ct.Apcm != NULL)
    {
        delete []FP_Ct.Apcm;
    }
    if(FP_Ct.fmt_ctx != NULL)
    {
        avformat_close_input(&FP_Ct.fmt_ctx);
        FP_Ct.fmt_ctx = NULL;
    }
    if(FP_Ct.ACodecC != NULL)
    {
        avcodec_free_context(&FP_Ct.ACodecC);
        FP_Ct.ACodecC = NULL;
    }
    if(FP_Ct.aSRctx != NULL)
    {
        swr_free(&FP_Ct.aSRctx);
    }
    if(FP_Ct.VCodecC != NULL)
    {
        avcodec_free_context(&FP_Ct.VCodecC);
        FP_Ct.VCodecC = NULL;
    }
    FFLOGE("QuitAndRelease_FP done");
}
int Init_FP_Codec(FP_Context& FP_Ct, bool isHWdecode)
{
    int ret = 0;
    AVFormatContext *fmt_ctx = FP_Ct.fmt_ctx;
    if(FP_Ct.fmt_ctx == NULL)
    {
        FFLOGE("not init FP_Context yeah , %s", __FUNCTION__);
        return 0;
    }
    for(int i = 0; i < fmt_ctx->nb_streams; i++)
    {
        AVStream* as = fmt_ctx->streams[i];
        if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            FP_Ct.videostreamID = i;
            FP_Ct.fps = r2d(as->avg_frame_rate);
            FP_Ct.width = as->codecpar->width;
            FP_Ct.height = as->codecpar->height;
            FP_Ct.VideoCodeID = as->codecpar->codec_id;
            FP_Ct.Videopixformat = as->codecpar->format;  //AVPixelFormat
            FFLOGE("video stream find, fmt_ctx->streams videostreamID:%d", i);
            FFLOGE("fps:%f, width:%d, height:%d, VideoCodeID:%d, pixformat:%d",
                   FP_Ct.fps, FP_Ct.width, FP_Ct.height,
                   FP_Ct.VideoCodeID, FP_Ct.Videopixformat);
        }
        else if(as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            FP_Ct.audiostreamID = i;
            FP_Ct.Audiochannels = as->codecpar->channels;
            FP_Ct.Audiosample_rate = as->codecpar->sample_rate;
            FP_Ct.AudioCodeID = as->codecpar->codec_id;
            FP_Ct.Audiosampleformat = as->codecpar->format;   //AVSampleFormat
            FP_Ct.Audioframe_size = as->codecpar->frame_size;
            FFLOGE("audio stream find, fmt_ctx->streams audiostreamID:%d", i);
            FFLOGE("Audiochannels:%d, Audiosample_rate:%d, AudioCodeID:%d, Audiosampleformat:%d, Audioframe_size:%d",
                   FP_Ct.Audiochannels, FP_Ct.Audiosample_rate, FP_Ct.AudioCodeID,
                   FP_Ct.Audiosampleformat, FP_Ct.Audioframe_size);
        }
    }
    // open decoder
    /*////////////////////////////////////////////////////*/
    // videio
    // sw decoder
    //AVCodec** VCodec = &FP_Context.VCodec;
    if(isHWdecode == false)
        FP_Ct.VCodec = avcodec_find_decoder(fmt_ctx->streams[FP_Ct.videostreamID]->codecpar->codec_id);
    // hw decoder
    else
        FP_Ct.VCodec = avcodec_find_decoder_by_name("h264_mediacodec");
    if(!FP_Ct.VCodec)
    {
        FFLOGE("can not find Video decoder:%d", FP_Ct.VideoCodeID);
        return -1;
    }

    // init decoder
    //AVCodecContext* VCodecC = FP_Context.VCodecC;
    FP_Ct.VCodecC = avcodec_alloc_context3(FP_Ct.VCodec);
    avcodec_parameters_to_context(FP_Ct.VCodecC, fmt_ctx->streams[FP_Ct.videostreamID]->codecpar);
    // sw
    FP_Ct.VCodecC->thread_count = 32;
    ret = avcodec_open2(FP_Ct.VCodecC, NULL, NULL);
    if(ret)
    {
        FFLOGE("can not open Video decoder:%d", FP_Ct.VideoCodeID);
        //avformat_close_input(&fmt_ctx);
        //env->ReleaseStringUTFChars(url_, url);
        return ret;
    }
    FFLOGE("open decoder VideoCodeID:%d, codec_type:%d", FP_Ct.VideoCodeID, FP_Ct.VCodecC->codec_type);

    /*////////////////////////////////////////////////////*/
    // audio
    // sw decoder
    //AVCodec* ACodec = FP_Context.ACodec;
    FP_Ct.ACodec = avcodec_find_decoder(fmt_ctx->streams[FP_Ct.audiostreamID]->codecpar->codec_id);
    if(!FP_Ct.ACodec)
    {
        FFLOGE("can not find Audio decoder:%d", FP_Ct.audiostreamID);
        //avformat_close_input(&fmt_ctx);
        //env->ReleaseStringUTFChars(url_, url);
        return ret;
    }

    // init decoder
    //AVCodecContext* ACodecC = FP_Context.ACodecC;
    FP_Ct.ACodecC = avcodec_alloc_context3(FP_Ct.ACodec);
    avcodec_parameters_to_context(FP_Ct.ACodecC, fmt_ctx->streams[FP_Ct.audiostreamID]->codecpar);
    //FP_Ct.ACodecC->thread_count = 1;
    ret = avcodec_open2(FP_Ct.ACodecC, NULL, NULL);
    if(ret)
    {
        FFLOGE("can not open Audio decoder:%d", FP_Ct.AudioCodeID);
        //avformat_close_input(&fmt_ctx);
        //env->ReleaseStringUTFChars(url_, url);
        return ret;
    }
    FFLOGE("open decoder AudioCodeID:%d, codec_type:%d", FP_Ct.AudioCodeID, FP_Ct.ACodecC->codec_type);
    return ret;
}
int Init_FP_SwAudiocover(FP_Context& FP_Ct, int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate)
{
    int ret = 0;
    //SwrContext * aSRctx = NULL;
    // init Audio SwrContext;
    //FP_Ct.aSRctx = swr_alloc();

    FP_Ct.aSRctx = swr_alloc_set_opts(FP_Ct.aSRctx,
                              // output config
                              /*av_get_default_channel_layout(FP_Ct.ACodecC->channels),*/ 2,
                              out_sample_fmt, out_sample_rate,
                              // input  config
                              av_get_default_channel_layout(FP_Ct.ACodecC->channels),
                              FP_Ct.ACodecC->sample_fmt, FP_Ct.ACodecC->sample_rate,
                              0, 0);

    ret = (int)swr_init(FP_Ct.aSRctx);

    if(!swr_is_initialized(FP_Ct.aSRctx))
    {
        FFLOGE("Init_FP_SwAudiocover failed");
        return ret;
    }

    return 0;
}
AVMediaType Use_FP_GetDecodeFrame(FP_Context& FP_Ct, AVFrame* outFrame)
{
    int ret = 0;
    AVPacket* pgk = av_packet_alloc();
    AVFrame* avFrame = av_frame_alloc();
    av_frame_unref(outFrame);

    for(;;)
    {
        ret = av_read_frame(FP_Ct.fmt_ctx, pgk);
        if(ret < 0)
        {
            FFLOGE("read stream eof, info:%s", av_err2str(ret));
            av_packet_unref(pgk);
            goto DecodeEND;
        }
        else
        {
            if(pgk->stream_index == FP_Ct.videostreamID)
            {
                //FFLOGE("read video stream videostreamID:%d, duration:%lld, pts:%lld, dts:%lld, size:%d, pos:%lld",
                //      pgk->stream_index, pgk->duration, pgk->pts, pgk->dts, pgk->size, pgk->pos);
                ret = avcodec_send_packet(FP_Ct.VCodecC, pgk);
                av_packet_unref(pgk);
                if(ret != 0)
                {
                    FFLOGE("avcodec_send_packet fail, ret:%d, info:%s", ret, av_err2str(ret));
                    continue;
                }
                for(;;)
                {
                    ret = avcodec_receive_frame(FP_Ct.VCodecC, avFrame);
                    if(ret != 0)
                    {
                        //FFLOGE("[Video]avcodec_receive_frame info:%s", av_err2str(ret));
                        break;
                    }
                    else
                    {
                        av_frame_move_ref(outFrame, avFrame);
                        return  AVMEDIA_TYPE_VIDEO;
                    }
                }
            }
            if(pgk->stream_index == FP_Ct.audiostreamID)
            {
                ret = avcodec_send_packet(FP_Ct.ACodecC, pgk);
                av_packet_unref(pgk);
                if(ret != 0)
                {
                    FFLOGE("avcodec_send_packet fail, ret:%d, info:%s", ret, av_err2str(ret));
                    continue;
                }
                for(;;)
                {
                    ret = avcodec_receive_frame(FP_Ct.ACodecC, avFrame);
                    if(ret != 0)
                    {
                        FFLOGE("[Audio]avcodec_receive_frame info:%s", av_err2str(ret));
                        break;
                    }
                    else
                    {
                        av_frame_move_ref(outFrame, avFrame);
                        return AVMEDIA_TYPE_AUDIO;
                    }
                }
            }
        }
    }

DecodeEND:
    if(pgk != NULL)
        av_packet_free(&pgk);
    //if(avFrame != NULL)
    //    return avFrame;
    return AVMEDIA_TYPE_UNKNOWN;
}

void Use_FP_ReleaseDecodeFrame(AVFrame* avFrame)
{
    if(avFrame != NULL)
        av_frame_free(&avFrame);
}

int  Use_FP_SwsScaleVFrame(FP_Context& FP_Ct, void* OutPutRGB, int SwsOutWidth, int SwsOutHeight)
{

    SwsContext * VSwsCt = FP_Ct.VSwsCt;
    AVFrame*     VFrame = FP_Ct.VFrame;
    int outHeiht = 0;
    uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
    data[0] =(uint8_t *)FP_Ct.Vrgba;
    int lines[AV_NUM_DATA_POINTERS] = {0};
    lines[0] = SwsOutWidth * 4;

    // init Video SwCoverContext
    VSwsCt = sws_getCachedContext(VSwsCt,VFrame->width, VFrame->height,
                                  (AVPixelFormat)VFrame->format,
                                  SwsOutWidth, SwsOutHeight,
                                  AV_PIX_FMT_RGBA, SWS_BICUBLIN,
                                  0, 0, 0);
    if(VSwsCt == NULL)
    {
        FFLOGE("sws_getCachedContext fail!");
        return -1;
    }
    outHeiht = sws_scale(VSwsCt, (const uint8_t **)VFrame->data, VFrame->linesize,
                          0, VFrame->height, data, lines);
    if(outHeiht == 0)
    {
        FFLOGE("Use_FP_SwsScaleFrame fail!, height:%d", outHeiht);
        return -1;
    }
    return 0;
}
int  Use_FP_SwCoverAFrame(FP_Context& FP_Ct, void* OutPutData)
{
    SwrContext * aSRctx = FP_Ct.aSRctx;
    AVFrame*     AFrame = FP_Ct.AFrame;
    int len = 0;
    uint8_t *out[2] = {0};
    out[0] = (uint8_t *)FP_Ct.Apcm;

    len = swr_convert(aSRctx, out, AFrame->nb_samples,
                      (const uint8_t **)AFrame->data,
                      AFrame->nb_samples);
    //FFLOGE("swr_convert len:%d", len);

    return 0;
}