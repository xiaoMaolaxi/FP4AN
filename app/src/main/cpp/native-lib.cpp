#include <jni.h>
#include <string>
#include <android/log.h>
#include <iostream>
#include <InitFFMPEG.h>


extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *res)
{
    av_jni_set_java_vm(vm, 0);
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_testffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ \n";
    hello += avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_testffmpeg_MainActivity_open(JNIEnv *env, jobject instance, jstring url_,
                                              jobject handle) {
    const char *url = env->GetStringUTFChars(url_, 0);

    FFLOGE("line:%d, native c++ got file url:%s", __LINE__, url);

    int ret;
    FP_Context FP_Ct;
    ret = Init_FP_Context(url, FP_Ct);
    if(ret != 0)
    {
        FFLOGE("Init_FP_Context fail, info:%s", av_err2str(ret));
    }

    AVFormatContext *fmt_ctx = FP_Ct.fmt_ctx;
    AVPacket* pgk = av_packet_alloc();
    AVFrame* avFrame = av_frame_alloc();
    long long start = GetNowMs();
    int frameCount = 0;
    SwsContext * VSwsCt = NULL;
    int SwsOutHeight = 1920;
    int SwsOutWidth  = 1080;
    char *rgba = new char[1920*1080*4];
    for(;;)
    {
        if(GetNowMs() - start >= 1000)
        {
            FFLOGE("now decode fps is %d",frameCount/3);
            start = GetNowMs();
            frameCount = 0;
        }
        ret = av_read_frame(fmt_ctx, pgk);
        if(ret < 0)
        {
            FFLOGE("read stream eof, info:%s", av_err2str(ret));
            av_packet_unref(pgk);
            break;
        }
        else
        {
            if(pgk->stream_index != FP_Ct.videostreamID)
            {
                //FFLOGE("read video stream videostreamID:%d, duration:%lld, pts:%lld, dts:%lld, size:%d, pos:%lld",
                //        pgk->stream_index, pgk->duration, pgk->pts, pgk->dts, pgk->size, pgk->pos);
                continue;
            }
            else
            {
                FFLOGE("read video stream videostreamID:%d, duration:%lld, pts:%lld, dts:%lld, size:%d, pos:%lld",
                        pgk->stream_index, pgk->duration, pgk->pts, pgk->dts, pgk->size, pgk->pos);
                ret = avcodec_send_packet(FP_Ct.VCodecC, pgk);
                frameCount++;
                av_packet_unref(pgk);
                if(ret != 0)
                {
                    FFLOGE("avcodec_send_packet fail, ret:%d, info:%s", ret, av_err2str(ret));
                }
                else
                {
                    ret = avcodec_receive_frame(FP_Ct.VCodecC, avFrame);
                    if(ret != 0)
                    {
                        //FFLOGE("avcodec_receive_frame info:%s", av_err2str(ret));
                    }
                    else
                    {
                        /*FFLOGE("avcodec_receive_frame info:%s, pts:%lld, height:%d, width:%d, crop_bottom:%d, crop_top:%d, crop_left:%d, crop_right:%d"
                                 , av_err2str(ret), avFrame->pts, avFrame->height, avFrame->width,
                               avFrame->crop_bottom, avFrame->crop_top, avFrame->crop_left,  avFrame->crop_right);*/
                        VSwsCt = sws_getCachedContext(VSwsCt,
                                avFrame->width, avFrame->height, (AVPixelFormat)avFrame->format,
                                SwsOutWidth, SwsOutHeight,
                                AV_PIX_FMT_RGBA, SWS_BICUBLIN,
                                0, 0, 0);
                        if(VSwsCt == NULL)
                        {
                            FFLOGE("sws_getCachedContext fail!");
                            break;
                        }
                        else
                        {
                            uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
                            data[0] =(uint8_t *)rgba;
                            int lines[AV_NUM_DATA_POINTERS] = {0};
                            lines[0] = SwsOutWidth * 4;
                            int h = sws_scale(VSwsCt,
                                    (const uint8_t **)avFrame->data, avFrame->linesize,
                                    0, avFrame->height, data,lines);
                            FFLOGE("after sws_scale SwsOutHeight:%d!", h);
                        }

                    }
                }
            }

        }
    }
    QuitAndRelease_FP(FP_Ct);
    delete []rgba;
    env->ReleaseStringUTFChars(url_, url);
    return 0;
}