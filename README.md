# FP4AN
learn fp 4 an

# 2019-11-12
  build with android studio
  use ffmpeg3.4 version and use make_Android_FFmpeg.sh for build ffmpeg for an
  
  of course, we use NDK version android-ndk-r14b
  
  in FFmpeg source code dir
  
  ``
  sh make_Android_FFmpeg.sh
  ``
  
  ``
  sudo make -j32 && sudo make install
  ``
  
  than ffmpeg for an libs will generated in ./android/armv7-a,
  
  than copy libs to AS_proj/app/libs and heade file to AS_proj/include
  
  
 
