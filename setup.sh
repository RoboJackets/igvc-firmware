#!/bin/bash

## First, get the submodules, namely nanopb and mbed-cmake
git submodule init && git submodule update

# next, install boost libraries
wget https://sourceforge.net/projects/boost/files/boost/1.77.0/boost_1_77_0.tar.bz2/download -O boost_1_77_0.tar.gz && \
tar -xvf boost_1_77_0.tar.gz -C lib/
cd lib/boost_1_77_0
./bootstrap.sh && \
    chmod +x b2 && \
    ./b2 && \
    ./b2 install 