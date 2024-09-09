#!/bin/bash

if test -f vcpkg-configuration.json; then
    echo "Found 'vcpkg-configuration.json'"
    echo "Deleting 'vcpkg-configuration.json'"
    rm vcpkg-configuration.json
    if !(test -f vcpkg-configuration.json); then
        echo "succesfully deleted 'vcpkg-configuration.json'"
    else
        echo "couldn't delete 'vcpkg-configuration.json'"
    fi
fi

if test -f vcpkg.json; then
    echo "Found 'vcpkg.json'"
    echo "Deleting 'vcpkg.json'"
    rm vcpkg.json
    if !(test -f vcpkg.json); then
        echo "succesfully deleted 'vcpkg.json'"
    else
        echo "couldn't delete 'vcpkg.json'"
    fi
fi
echo "
Creating build dir
"
if test -d build; then
    echo "dir 'build' already exists"
    echo "deleting 'build'..."
    rm -rf build
    if !(test -d build); then
        echo "succesfully deleted 'build'"
    else
        echo "couldn't delete 'build'"
    fi
fi
mkdir build

vcpkg new --application
vcpkg add port fmt
vcpkg add port asio

echo "
Starting cmake
"
cmake -S . -B build --preset vcpkg
>building/ip.txt