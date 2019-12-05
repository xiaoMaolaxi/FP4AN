# FP4AN
learn fp 4 an

# config compile system(ubuntu-18)
  use sys_install.sh to init compile system
  
  ``
  sudo sh install.sh
  ``
  
  use ffmpeg3.4 version and use make_Android_FFmpeg.sh for build ffmpeg for an
  
  of course, we use NDK version android-ndk-r14b
  
  copy make_Android_FFmpeg.sh to  FFmpeg source code dir
  
  ``
  sh make_Android_FFmpeg.sh
  ``
  
  ``
  sudo make -j32 && sudo make install
  ``
  
  than ffmpeg for an libs will generated in ./android/armv7-a,
  
  and copy libs to AS_proj/app/libs and heade file to AS_proj/include


# 2019-12-5
### add YUVplayer exp, use OpenGL play YUV420P format

# 2019-12-1
### add PCMplayer exp, use OpenSL play 4418000 samplerate

# 2019-11-25
### add video sw render

# 2019-11-19
### add audio sw resample

# 2019-11-17
### add video sw scale
 
