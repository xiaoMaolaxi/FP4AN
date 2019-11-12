#指定NDK路径
export NDK=/home/kk/work/for-AN/Android_NDK/android-ndk-r14b

#架构下的so库和头文件
export PLATFORM=$NDK/platforms/android-21/arch-arm

#交叉编译工具 谁来用，用在哪
export TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64

export CPU=armv7-a

export PREFIX=../android-ffmpeg3.4-SO/$CPU 

export TMPDIR=../temp

./configure \
    --prefix=$PREFIX \
    --target-os=android \
    --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
    --arch=arm \
    --cpu=armv7-a  \
    --sysroot=$PLATFORM \
    --extra-cflags="-I$PLATFORM/usr/include -fPIC -DANDROID -mfpu=neon -mfloat-abi=softfp " \
    --cc=$TOOLCHAIN/bin/arm-linux-androideabi-gcc \
    --nm=$TOOLCHAIN/bin/arm-linux-androideabi-nm \
    --enable-shared \
    --enable-runtime-cpudetect \
    --enable-gpl \
    --enable-small \
    --enable-cross-compile \
    --enable-asm \
    --enable-neon \
    --enable-jni \
    --enable-mediacodec \
    --enable-decoder=h264_mediacodec \
    --enable-hwaccel=h264_mediacodec \
    --disable-debug \
    --disable-static \
    --disable-doc \
    --disable-ffmpeg \
    --disable-ffplay \
    --disable-ffprobe \
    --disable-postproc \
    --disable-avdevice \
    --disable-symver \
    --disable-stripping  \
    --disable-ffserver \
