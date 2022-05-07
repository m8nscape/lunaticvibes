## Build

Dependencies: 
- make
- gcc / mingw32-w64-gcc (MinGW) / mingw64-x86_64-gcc (Cygwin)
- yasm
- nasm
- libmfx (intel quick sync library, may hard to find and configure (disabled for now))
- libvpx

Command: 

- Windows (MinGW / Cygwin)
```
./configure --enable-cross-compile --enable-shared --arch=x86_64 --target-os=mingw64 --cross-prefix=x86_64-w64-mingw32- \
  --disable-everything \
  --enable-decoder=amv,flv,h264,h264_qsv,hevc,mpeg_vdpau,mpegvideo,mpeg1video,mpeg2video,mpeg4,mpeg4_vdpau,msmpeg4v1,msmpeg4v2,msmpeg4v3,vp8,vp9,webp,wmv1,wmv2,wmv3 \
  --enable-vdpau \
  --enable-demuxer=avi,dirac,flv,h264,hevc,m4v,matroska,mov,mpegvideo,mv,vc1,vc1t \
  --enable-parser=dirac,h264,hevc,mpeg4video,mpegvideo,vc1,vp8 \
  --enable-protocol=file \
  --enable-filter=fps,setpts

make -jN
```


should output binaries under LGPL 2.1

------

### notes

I removed --enable-libmfx from build command

BGAs around 2010 mostly use mpeg1 / mpeg2, while recent ones involves wmv2 / wmv3 / h264, etc
