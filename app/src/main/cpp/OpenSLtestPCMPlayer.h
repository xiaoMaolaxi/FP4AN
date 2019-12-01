//
// Created by PLX on 2019/12/1.
//

#ifndef TESTFFMPEG_OPENSLTESTPCMPLAYER_H
#define TESTFFMPEG_OPENSLTESTPCMPLAYER_H
#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>

#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"ywl5320",FORMAT,##__VA_ARGS__);

void OpenSLtestPCMPlayer();
#endif //TESTFFMPEG_OPENSLTESTPCMPLAYER_H
