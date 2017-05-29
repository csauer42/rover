#!/bin/bash

if [ $UID -ne 0 ]; then
    echo "Must be run as root"
    exit 1
fi

v4l2-ctl --set-fmt-video=width=640,height=480,pixelformat=5
v4l2-ctl -p 24
v4l2-ctl --set-ctrl=rotate=180
v4l2-ctl --set-ctrl video_bitrate=5000000

