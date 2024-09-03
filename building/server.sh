#!/bin/bash
cd ..
echo "WARNING! This is deprecated!"
echo "#define BUILDTYPE BuildType::Server" > src/buildType.hpp
rm -f server/JNet
rm -f build/JNet
cmake --build build
mkdir server
cp build/JNet server/JNet
sudo ./server/JNet