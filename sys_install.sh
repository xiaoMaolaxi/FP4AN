#! /bin/bash
#-y mean yes

# vim
#sudo apt-get install vim -y

# gcc
#sudo apt-get-install gcc -y

# cmake 
#sudo apt-get install cmake -y

#####
# for build ffmpeg and SDL2.2
#####
PKGarray=("vim" \
	  "gcc" \
	  "cmake" \
          "autoconf" \
	  "automake" \
	  "build-essential" \
	  "libass-dev" \
	  "build-essential" \
	  "libass-dev" \ 
	  "libfreetype6-dev" \
	  "libtheora-dev" \
	  "libtool" \ 
	  "libvorbis-dev" \
	  "pkg-config" \ 
	  "texinfo" \
	  "zlib1g-dev" \
	  "unzip" \
	  "yasm" \
	  "libx264-dev" \
	  "libmp3lame-dev" \
	  "libopus-dev" \
	  "libx265-dev" \
	  "libsdl2-dev" \
	  "libva-dev" \
	  "libvdpau-dev" \
	  "libxcb1-dev" \
	  "libxcb-shm0-dev" \
	  "libxcb-xfixes0-dev" \
	  "git" \
	  "libass-dev" \
	  "libva-dev" \
	  "libvdpau-dev" \
	  "nasm" \
	  "yasm" \
	  "libvpx-dev" \
	  "libfdk-aac-dev" \
	  "libspeex-dev" \
	  "libxvidcore-dev" \
	  "libomxil-bellagio-dev" \
	  "libc6-dev-i386")

# PKGarray length
#echo ${#PKGarray[@]}
# PKGarray all
#echo ${PKGarray[@]}

for tmp in  ${PKGarray[@]}
do 
    #echo $tmp
    #if dpkg-query -l $tmp; then
    if dpkg -s $tmp&>/dev/null; then
        echo $tmp already install
    else
	sudo apt-get install $tmp -y
	#echo $tmp
    fi
done
