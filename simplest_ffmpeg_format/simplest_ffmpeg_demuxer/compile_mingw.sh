#! /bin/sh
#��򵥵Ļ���FFmpeg������Ƶ������ ----MinGW�����б���
#Simplest FFmpeg Demuxer ----Compile in MinGW 
#
#������ Lei Xiaohua
#leixiaohua1020@126.com
#�й���ý��ѧ/���ֵ��Ӽ���
#Communication University of China / Digital TV Technology
#http://blog.csdn.net/leixiaohua1020
#
#compile
g++ simplest_ffmpeg_demuxer.cpp -g -o simplest_ffmpeg_demuxer.exe \
-I /usr/local/include -L /usr/local/lib -lavformat -lavcodec -lavutil