#!/bin/bash

if [ $UID -ne 0 ]; then
    echo "must be run as root"
    exit 1
fi

cat /dev/video0 | nc -lkv4 5001
