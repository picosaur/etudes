#!/bin/bash

#export ANDROID_HOME=/Users/YourName/Library/Android/sdk
#export ANDROID_NDK_HOME=/Users/YourName/Library/Android/sdk/ndk/28.0.12674087

export ANDROID_HOME=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/25.1.8937393

BUILD_DIR="build_android"

if [ -d "$BUILD_DIR" ]; then rm -Rf $BUILD_DIR; fi

mkdir $BUILD_DIR && cd $BUILD_DIR
../external/hello_imgui/tools/android/cmake_arm-android.sh ../
