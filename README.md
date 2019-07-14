# RoboJackets IGVC Firmware [![Build Status](https://circleci.com/gh/RoboJackets/igvc-firmware/tree/master.svg?&style=shield)](https://circleci.com/gh/RoboJackets/igvc-firmware/tree/master)

Welcome to the RoboJackets IGVC firmware repo!

## Dependencies
* protobuf-compiler
* python-protobuf

For more details, check the [Dockerfile](Dockerfile).

## Building Documentation
Make sure to `git submodule init && git submodule update` as the repo uses git submodules for the `mbed-os` and `nanopb` dependencies.

```bash
mkdir build
cmake ..
make
```

There should be a `igvc-firmware.bin` file that was compiled. Drag that onto the mbed to flash the firmware.
