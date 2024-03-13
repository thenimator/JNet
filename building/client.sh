#!/bin/bash
cd ..
echo "#define BUILDTYPE BuildType::Client" > src/buildType.hpp
cmake --build build
mkdir client
cp build/JNet client/JNet
ip=`cat building/ip.txt`
./client/JNet $ip