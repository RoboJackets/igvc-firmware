# RoboJackets IGVC Firmware [![Build Status](https://circleci.com/gh/RoboJackets/igvc-firmware/tree/master.svg?&style=shield)](https://circleci.com/gh/RoboJackets/igvc-firmware/tree/master)

Welcome to the RoboJackets IGVC firmware repo!

## Dependencies
* Updated `arm-none-eabi-gcc`
* protobuf-compiler
* python-protobuf

The second two can be installed with
```bash
sudo apt install -y protobuf-compiler python-protobuf
```

For more details, check the [Dockerfile](Dockerfile).

### Updating `arm-none-eabi-gcc`
For some reason, it seems like the version of `arm-none-eabi-ld` on Ubuntu 18.04 has some linking problems.
The problem seems to go away on newer versions of `arm-non-eabi-gcc` though.

**Steps to update `arm-none-eabi-gcc`**:
1. Download [gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2](https://developer.arm.com/-/media/Files/downloads/gnu-rm/8-2018q4/gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2?revision=d830f9dd-cd4f-406d-8672-cca9210dd220?product=GNU%20Arm%20Embedded%20Toolchain,64-bit,,Linux,8-2018-q4-major)
2. Untar it and move it to some location (eg. `/opt/gcc-arm-none-eabi-8-2018-q4-major`)
3. Add it to your `$PATH` variable, eg. add the following line to `.bashrc`:
   ```bash
   export PATH="/opt/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH"
   ```

This should solve the `ld` issues

## Building Documentation
Make sure to `git submodule init && git submodule update` as the repo uses git submodules for the `mbed-os` and `nanopb` dependencies.

```bash
mkdir build
cmake ..
make
```

There should be a `igvc-firmware.bin` file that was compiled. Drag that onto the mbed to flash the firmware.
