//
// Created by PLX on 2019/11/10.
//
#include <Use_FFMPEG.h>
#include "../../../../include/libavformat/avformat.h"
#include "Use_FFMPEG.h"
#include "../../../../include/libavcodec/avcodec.h"

int Init_FP_Context(const char* url, FP_Context& FP_Context)
{

    int ret;
    FP_Context.fileURL = url;
    AVFormatContext **fmt_ctx = &FP_Context.fmt_ctx;

    av_register_all();
    avcodec_register_all();
    avformat_network_init();

    ret = avformat_open_input(&(*fmt_ctx), url, NULL, NULL);
    if(ret != 0)
        goto ERR_END;
    FFLOGE("avformat_open_input, %s", FP_Context.fileURL);

    ret = avformat_find_stream_info((*fmt_ctx), NULL);
    if(ret != 0)
        goto ERR_END;

    // default use sw decode
    ret = Init_FP_Codec(FP_Context, false);
    if(ret != 0)
        goto ERR_END;
    //av_log_set_callback(log_callback_test2);
    //av_dump_format(fmt_ctx, 0, url, 0);
    //av_dump_format(fmt_ctx, 1, url, 0);
    FFLOGE("fmt_ctx->duration:%lld, fmt_ctx->nb_streams:%d", (*fmt_ctx)->duration, (*fmt_ctx)->nb_streams);
    FFLOGE("Init_FP_Context done, ret:%s", av_err2str(ret));

    if(!avcodec_is_open(FP_Context.VCodecC))
    {
        FFLOGE("1VCodecC not open, info:%s", av_err2str(ret));
        return 0;
    }
    return ret;

ERR_END:
        FFLOGE("we got err msg, %s", av_err2str(ret));
        QuitAndRelease_FP(FP_Context);
        return ret;
}
void QuitAndRelease_FP(FP_Context& FP_Context)
{
    FFLOGE("QuitAndRelease_FP enter");
    if(FP_Context.fmt_ctx != NULL)
        avformat_close_input(&FP_Context.fmt_ctx);
    if(FP_Context.VCodecC != NULL)
        avcodec_free_context(&FP_Context.VCodecC);
    if(FP_Context.ACodecC != NULL)
        avcodec_free_context(&FP_Context.ACodecC);
    FFLOGE("QuitAndRelease_FP done");
}
int Init_FP_Codec(FP_Context& FP_Context, bool isHWdecode)
{
    int ret = 0;
    AVFormatContext *fmt_ctx = FP_Context.fmt_ctx;
    if(FP_Context.fmt_ctx == NULL)
    {
        FFLOGE("not init FP_Context yeah , %s", __FUNCTION__);
        return 0;
    }
    for(int i = 0; i < fmt_ctx->nb_streams; i++)
    {
        AVStream* as = fmt_ctx->streams[i];
        if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            FP_Context.videostreamID = i;
            FP_Context.fps = r2d(as->avg_frame_rate);
            FP_Context.width = as->codecpar->width;
            FP_Context.height = as->codecpar->height;
            FP_Context.VideoCodeID = as->codecpar->codec_id;
            FP_Context.Videopixformat = as->codecpar->format;  //AVPixelFormat
            FFLOGE("video stream find, fmt_ctx->streams videostreamID:%d", i);
            FFLOGE("fps:%f, width:%d, height:%d, VideoCodeID:%d, pixformat:%d",
                   FP_Context.fps, FP_Context.width, FP_Context.height,
                   FP_Context.VideoCodeID, FP_Context.Videopixformat);
        }
        else if(as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            FP_Context.audiostreamID = i;
            FP_Context.Audiochannels = as->codecpar->channels;
            FP_Context.Audiosample_rate = as->codecpar->sample_rate;
            FP_Context.AudioCodeID = as->codecpar->codec_id;
            FP_Context.Audiosampleformat = as->codecpar->format;   //AVSampleFormat
            FP_Context.Audioframe_size = as->codecpar->frame_size;
            FFLOGE("audio stream find, fmt_ctx->streams audiostreamID:%d", i);
            FFLOGE("Audiochannels:%d, Audiosample_rate:%d, AudioCodeID:%d, Audiosampleformat:%d, Audioframe_size:%d",
                   FP_Context.Audiochannels, FP_Context.Audiosample_rate, FP_Context.AudioCodeID,
                   FP_Context.Audiosampleformat, FP_Context.Audioframe_size);
        }
    }
    // open decoder
    /*////////////////////////////////////////////////////*/
    // videio
    // sw decoder
    //AVCodec** VCodec = &FP_Context.VCodec;
    if(isHWdecode == false)
        FP_Context.VCodec = avcodec_find_decoder(fmt_ctx->streams[FP_Context.videostreamID]->codecpar->codec_id);
    // hw decoder
    else
        FP_Context.VCodec = avcodec_find_decoder_by_name("H264_mediacodec");
    if(!FP_Context.VCodec)
    {
        FFLOGE("can not find Video decoder:%d", FP_Context.VideoCodeID);
        return -1;
    }

    // init decoder
    //AVCodecContext* VCodecC = FP_Context.VCodecC;
    FP_Context.VCodecC = avcodec_alloc_context3(FP_Context.VCodec);
    avcodec_parameters_to_context(FP_Context.VCodecC, fmt_ctx->streams[FP_Context.videostreamID]->codecpar);
    // sw
    // VCodecC->thread_count = 32;
    ret = avcodec_open2(FP_Context.VCodecC, NULL, NULL);
    if(ret)
    {
        FFLOGE("can not open Video decoder:%d", FP_Context.VideoCodeID);
        //avformat_close_input(&fmt_ctx);
        //env->ReleaseStringUTFChars(url_, url);
        return ret;
    }
    FFLOGE("open decoder VideoCodeID:%d, codec_type:%d", FP_Context.VideoCodeID, FP_Context.VCodecC->codec_type);

    /*////////////////////////////////////////////////////*/
    // audio
    // sw decoder
    //AVCodec* ACodec = FP_Context.ACodec;
    FP_Context.ACodec = avcodec_find_decoder(fmt_ctx->streams[FP_Context.audiostreamID]->codecpar->codec_id);
    if(!FP_Context.ACodec)
    {
        FFLOGE("can not find Audio decoder:%d", FP_Context.audiostreamID);
        //avformat_close_input(&fmt_ctx);
        //env->ReleaseStringUTFChars(url_, url);
        return ret;
    }

    // init decoder
    //AVCodecContext* ACodecC = FP_Context.ACodecC;
    FP_Context.ACodecC = avcodec_alloc_context3(FP_Context.ACodec);
    avcodec_parameters_to_context(FP_Context.ACodecC, fmt_ctx->streams[FP_Context.audiostreamID]->codecpar);
    // ACodecC->thread_count = 1;
    ret = avcodec_open2(FP_Context.ACodecC, NULL, NULL);
    if(ret)
    {
        FFLOGE("can not open Audio decoder:%d", FP_Context.AudioCodeID);
        //avformat_close_input(&fmt_ctx);
        //env->ReleaseStringUTFChars(url_, url);
        return ret;
    }
    FFLOGE("open decoder AudioCodeID:%d, codec_type:%d", FP_Context.AudioCodeID, FP_Context.ACodecC->codec_type);
    return ret;
}

AVFrame* Use_FP_GetDecodeFrame(FP_Context& FP_Context, AVMediaType PkgType)
{
    int ret = 0;
    AVPacket* pgk = av_packet_alloc();
    AVFrame* avFrame = av_frame_alloc();

    if(AVMEDIA_TYPE_AUDIO == PkgType)
    {
        FFLOGE("sorry not support audio yeah");
        return NULL;
    }
    if(AVMEDIA_TYPE_VIDEO == PkgType)
    {
        for(;;)
        {
            ret = av_read_frame(FP_Context.fmt_ctx, pgk);
            if(ret < 0)
            {
                FFLOGE("read stream eof, info:%s", av_err2str(ret));
                av_packet_unref(pgk);
                goto DecodeEND;
            }
            else
            {
                if(pgk->stream_index != FP_Context.videostreamID)
                {
                    //FFLOGE("read video stream videostreamID:%d, duration:%lld, pts:%lld, dts:%lld, size:%d, pos:%lld",
                    //        pgk->stream_index, pgk->duration, pgk->pts, pgk->dts, pgk->size, pgk->pos);
                    continue;
                }
                else
                {
                    FFLOGE("read video stream videostreamID:%d, duration:%lld, pts:%lld, dts:%lld, size:%d, pos:%lld",
                           pgk->stream_index, pgk->duration, pgk->pts, pgk->dts, pgk->size, pgk->pos);
                    ret = avcodec_send_packet(FP_Context.VCodecC, pgk);
                    av_packet_unref(pgk);
                    if(ret != 0)
                    {
                        FFLOGE("avcodec_send_packet fail, ret:%d, info:%s", ret, av_err2str(ret));
                    }
                    else
                    {
                        ret = avcodec_receive_frame(FP_Context.VCodecC, avFrame);
                        if(ret != 0)
                        {
                            FFLOGE("avcodec_receive_frame info:%s", av_err2str(ret));
                        }
                        else
                        {
                            goto DecodeEND;
                        }
                    }
                }
            }
        }
    }
DecodeEND:
    av_packet_free(&pgk);
    if(avFrame != NULL)
        return avFrame;
    return NULL;
}

void Use_FP_ReleaseDecodeFrame(AVFrame* avFrame)
{
    av_frame_free(&avFrame);
}