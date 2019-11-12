//
// Created by PLX on 2019/11/10.
//

#ifndef TESTFFMPEG_INITFFMPEG_H
#define TESTFFMPEG_INITFFMPEG_H
#include <jni.h>
#include <string>
#include <android/log.h>
#include <iostream>

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
}

#define FFLOGE(...) __android_log_print(ANDROID_LOG_ERROR, "FFmpeg", __VA_ARGS__)
//当前时间戳 clock
static long long GetNowMs()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int sec = tv.tv_sec%360000;
    long long t = sec*1000+tv.tv_usec/1000;
    return t;
}

static void log_callback_test2(void *ptr, int level, const char *fmt, va_list vl)
{

    va_list vl2;
    char line[1024];

    static int print_prefix = 1;
    va_copy(vl2, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
    va_end(vl2);

    FFLOGE("%s", line);
}


static float r2d(AVRational r)
{
    //AVRational
    //int num; ///< Numerator
    //int den; ///< Denominator

    return (r.num == 0 || r.den == 0) ? 0 : (float)(r.num / r.den);
}

typedef struct FP_Context{
    const char* fileURL;
    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *tag = NULL;

    int width = 0;
    int height = 0;
    int videostreamID = 0;
    int VideoCodeID = 0;
    int Videopixformat = 0;
    AVCodec* VCodec;
    AVCodecContext* VCodecC;

    float fps = 0;
    int AudioCodeID = 0;
    int audiostreamID = 0;
    int Audiosampleformat = 0;
    int Audiochannels = 0;
    int Audiosample_rate = 0;
    int Audioframe_size = 0;
    AVCodec* ACodec;
    AVCodecContext* ACodecC;
} FP_Context;

int Init_FP_Context(const char* url, FP_Context& FP_Context);
void QuitAndRelease_FP(FP_Context& FP_Context);
int Init_FP_Codec(FP_Context& FP_Context, bool isHWdecode);
#endif //TESTFFMPEG_INITFFMPEG_H
