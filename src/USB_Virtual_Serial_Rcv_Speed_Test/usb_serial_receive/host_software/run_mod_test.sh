#!/bin/sh
for blocksize in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 
do
    echo "running test with blocksize $blocksize"
    ./receive_test_mod $blocksize /dev/ttyACM0 >> $1
done

