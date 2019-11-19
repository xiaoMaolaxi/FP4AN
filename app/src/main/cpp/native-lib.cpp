#include <jni.h>
#include <string>
#include <android/log.h>
#include <iostream>
#include <Use_FFMPEG.h>

extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/jni.h"
#include <libavutil/dict.h>
#include "libavutil/attributes.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/bprint.h"
#include "libavutil/display.h"
#include "libavutil/mathematics.h"
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
#include "libavutil/pixdesc.h"
#include "libavutil/eval.h"
#include "libavutil/dict.h"
#include "libavutil/opt.h"
#include "libavutil/cpu.h"
#include "libavutil/ffversion.h"
#include "libavutil/version.h"
#include "libswscale/swscale.h"
#include "libswscale/version.h"
#include "libswresample/swresample.h"
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *res)
{
    av_jni_set_java_vm(vm, NULL);
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
    //int isHwDecode = true;
    int isHwDecode = false;
    FP_Context FP_Ct;
    //AVFormatContext *fmt_ctx = FP_Ct.fmt_ctx;
    //AVPacket* pgk = av_packet_alloc();

    long long start = GetNowMs();
    int frameCount = 0;
    int SwsOutHeight = 1920;
    int SwsOutWidth  = 1080;



    AVPacket* pgk = av_packet_alloc();
    AVFrame* avFrame = av_frame_alloc();

    ret = Init_FP_Context(url, FP_Ct, isHwDecode);
    if(ret != 0)
    {
        FFLOGE("Init_FP_Context fail, info:%s", av_err2str(ret));
        goto MainEND;
    }

    for(;;)
    {
        if(GetNowMs() - start >= 1000)
        {
            FFLOGE("now decode fps is %d",frameCount/3);
            start = GetNowMs();
            frameCount = 0;
        }

        ret = Use_FP_GetDecodeFrame(FP_Ct, avFrame);
        if(AVMEDIA_TYPE_VIDEO == ret)
        {
            FP_Ct.VFrame = avFrame;
            ret = Use_FP_SwsScaleVFrame(FP_Ct, FP_Ct.Vrgba, SwsOutWidth, SwsOutHeight);
            if(ret != 0)
            {
                FFLOGE("[Video]SwsScaleFrame fail, ret: %d", ret);
                goto MainEND;
            }
            frameCount++;
        }
        else if(AVMEDIA_TYPE_AUDIO == ret)
        {
                FP_Ct.AFrame = avFrame;
                ret = Use_FP_SwCoverAFrame(FP_Ct, FP_Ct.Apcm);
                if(ret != 0)
                {
                    FFLOGE("[Audio]SwsScaleFrame fail, ret: %d", ret);
                    goto MainEND;
                }
        }
        else{
            goto MainEND;
        }
    }

MainEND:
    if(FP_Ct.VFrame)
        Use_FP_ReleaseDecodeFrame(FP_Ct.VFrame);
    QuitAndRelease_FP(FP_Ct);
    //delete []rgba;
    env->ReleaseStringUTFChars(url_, url);
    return 0;
}