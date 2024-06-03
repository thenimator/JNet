#!/bin/bash
cd ..
echo "#define BUILDTYPE BuildType::Client" > src/buildType.hpp
rm -f client/JNet
rm -f build/JNet
cmake --build build
mkdir client
cp build/JNet client/JNet
ip=`cat building/ip.txt`
./client/JNet $ip