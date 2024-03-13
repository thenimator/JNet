#!/bin/bash
cd ..
echo "#define BUILDTYPE BuildType::Server" > src/buildType.hpp
cmake --build build
mkdir server
cp build/JNet server/JNet
sudo ./server/JNet