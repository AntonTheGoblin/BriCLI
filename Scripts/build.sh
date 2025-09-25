#!/usr/bin/env bash
BUILD_DIR="./Build"
NUM_CORES=1

while getopts "b:j:" o; do
    case "${o}" in
        b) BUILD_DIR=${OPTARG};;
        j) NUM_CORES=${OPTARG};;
    esac
done

# Make sure our build directory exists
if [ ! -d $BUILD_DIR ]; then
    mkdir $BUILD_DIR
    BUILD_DIR=`realpath ${BUILD_DIR}`
fi

# Invoke CMake
cmake -S . -B $BUILD_DIR
cmake --build $BUILD_DIR -j${NUM_CORES}