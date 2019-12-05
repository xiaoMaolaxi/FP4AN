//
// Created by PLX on 2019/12/5.
//

#ifndef TESTFFMPEG_OPENGLTESTYUVPLAYER_H
#define TESTFFMPEG_OPENGLTESTYUVPLAYER_H

#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <string.h>

#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"ywl5320",FORMAT,##__VA_ARGS__);

void OpenGLtestYUVplayer(const char* url, ANativeWindow* nwin);

#endif //TESTFFMPEG_OPENGLTESTYUVPLAYER_H
