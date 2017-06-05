#!/bin/bash

if [ $UID -ne 0 ]; then
    echo "Must be run as root"
    exit 1
fi

# set camera options with MPEG output, 24fps, rotate 180 degree 
# to adjust for upside-down camera in pan-tilt module
v4l2-ctl --set-fmt-video=width=640,height=480,pixelformat=5
v4l2-ctl -p 24
v4l2-ctl --set-ctrl=rotate=180
v4l2-ctl --set-ctrl video_bitrate=5000000

# pipe raw output stream to port 5001 via netcat
# (listen mode, connect repeatedly)
cat /dev/video0 | nc -lk 5001
