#include <jni.h>
#include <string>
#include <android/log.h>
#include <iostream>
#include <Use_FFMPEG.h>


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
    int isHwDecode = true;
    FP_Context FP_Ct;
    //AVFormatContext *fmt_ctx = FP_Ct.fmt_ctx;
    //AVPacket* pgk = av_packet_alloc();

    long long start = GetNowMs();
    int frameCount = 0;
    int SwsOutHeight = 1920;
    int SwsOutWidth  = 1080;
    char *rgba = new char[1920*1080*4];

    ret = Init_FP_Context(url, FP_Ct, false);
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

        FP_Ct.VFrame = Use_FP_GetDecodeFrame(FP_Ct, AVMEDIA_TYPE_VIDEO);
        if(FP_Ct.VFrame != NULL)
        {
            ret = Use_FP_SwsScaleFrame(FP_Ct, rgba, SwsOutWidth, SwsOutHeight);
            if(ret != 0)
            {
                FFLOGE("SwsScaleFrame fail, ret: %d", ret);
                goto MainEND;
            }

            frameCount++;
        }
        if(FP_Ct.VFrame == NULL)
            goto MainEND;
    }

MainEND:
    if(FP_Ct.VFrame)
        Use_FP_ReleaseDecodeFrame(FP_Ct.VFrame);
    QuitAndRelease_FP(FP_Ct);
    delete []rgba;
    env->ReleaseStringUTFChars(url_, url);
    return 0;
}