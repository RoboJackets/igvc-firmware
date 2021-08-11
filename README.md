# Robojackets Swervi Repo

## Dependecies

* Python 3
* Protobuf Compiler (protoc)
* Python Protobuf
* Docker (optional)

## Download

Linux is recommended, as Swervi runs on Ubuntu. However, this should work on all platforms.

First, clone the directory and checkout this branch:

```
git clone https://github.com/RoboJackets/igvc-firmware.git
git checkout -b swervi_refactor origin/swervi_refactor
```

Now, download the submodules, [mbed_cmake](https://github.com/USCRPL/mbed-cmake) and [nano_pb](https://github.com/nanopb/nanopb), using:

```
git submodule init && git submodule update
```

## Protobuf Installation

Next, install the Protobuf Compiler (protoc) and Python Protobuf. 

**Windows**

Download the binaries [here](https://github.com/protocolbuffers/protobuf/releases/download/v3.17.3/protoc-3.17.3-win64.zip). Follow the installation instructions.

**Ubuntu**
```
sudo apt install -y protobuf-compiler python-protobuf
```

**Archlinux**
```
sudo pacman -Syy protobuf python-protobuf
```

## Installing `python3 pip`

There are several ways to install `pip`. The most reliable method of doing so is running:

**Windows**
```
py -m ensurepip --upgrade
```

**Linux**
```
python -m ensurepip --upgrade
```
Click [here](https://pip.pypa.io/en/stable/installation/) for alternate installation instrucions of `python pip`.

## Updating `arm-none-eabi-gcc`
Add `arm-none-eabi-gcc` to your path variable.

**Windows** 

(add details here)

**Linux**
1. Download [gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2](https://developer.arm.com/-/media/Files/downloads/gnu-rm/8-2018q4/gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2?revision=d830f9dd-cd4f-406d-8672-cca9210dd220?product=GNU%20Arm%20Embedded%20Toolchain,64-bit,,Linux,8-2018-q4-major).
2. Untar it and move it to some location. For example, if you want to place the output in /opt/ use:
```
tar -xvf /path/to/gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2/ -C /opt/
```
3. Add it to your `$PATH` variable. For instance, if you placed `gcc-arm-none-eabi-8-2018-q4-major-linux` in /opt/, add the following line to `.bashrc`:

```
export PATH="/opt/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH"
```

## Building

Look at comment at the top of `CMakeLists.txt`. Copy and paste one of the `cmake` commands. For instance, to generate the Makefiles for the blink test program, paste the following in the same directory as `CMakeLists.txt`:
```
cmake -D BUILD_PROGRAM="TEST_BLINK" -B OutputCMake/test/blink/
```
Navigate to the output directory of the `cmake` command; in the aboe example, navigate to `OutputCMake/test/blink/`. Then run:
```
make
```
If you are compiling a program for the mbed, `make` will have to compile all of the mbed libraries. To speed up compilation, use the `-j` argument for `make`. In general, if you have an `n` core processor, type
```
make -j n
```
to compile the project more quickly.

The mbed binary file `blink.bin` will appear in the `build/` directory.

## Development

Suppose you added a new test program to `src/` and your directory looks like this:
```
src\
    example\
        main.cpp
        communication\
            my_proto.proto
            my_file.cpp
            my_file.h
```
Follow these steps to build your program:
1. Give your program a name (e.g. `TEST_EXAMPLE`).
2. Create the directory `OutputCMake/test/example`. This will be the output directory for cmake.
3. In `CMakeLists.txt`, add a comment at the top of file for the `cmake` command associated with the `example` project. Be sure to specify the name of your program and the output directory for cmake, like so: 
```
#   cmake -D BUILD_PROGRAM="TEST_EXAMPLE" -B OutputCMake/test/example
```

4. Look for the first `if-else` block in `CMakeLists.txt`. Add the following `elseif` clause:
```
elseif (${BUILD_PROGRAM} STREQUAL TEST_EXAMPLE)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/example)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp
        ${SRC_DIR}/my_file.cpp)
    set(BUILD_PROTOBUF ON)
    set(BUILD_MBED ON)
```
In particular, the following must be set:
* `SRC_DIR`: the source directory of your program
* `PROGRAM_SRC`: a list of the .cpp files you wish to compile
* `BUILD_PROTOBUF`: whether or not you will generating protobuffers for your program. If you have a .proto file, set this to `ON`. Otherwise, leave it `OFF`.
* `BUILD_MBED`: whether or not you are creating a binary file for the mbed platform. If you are, leave this `ON`. If you are creating a test script for your computer, turn this `OFF`.

5. Scroll to the bottom of `CMakeLists.txt`. You now explicitly name your output file. If you are building a program for the mbed, add the following inside the `if-else` block inside the `if (${BUILD_MBED})` clause:
``` 
elseif (${BUILD_PROGRAM} STREQUAL TEST_EXAMPLE)
    add_mbed_executable(test_example ${PROGRAM_SRC} ${PROTO_FILES})
```
If your are building a test script for your computer, add the following inside the `if-else` block inside the `else` clause of `if (${BUILD_MBED})`:
```
elseif (${BUILD_PROGRAM} STREQUAL TEST_EXAMPLE)
        add_executable(test_example ${PROGRAM_SRC} ${PROTO_FILES}) 
        target_link_libraries(test_example -lstdc++ -lsupc++ -lm -lc -lgcc ${Boost_LIBRARIES})
```
6. All done! You can save `CMakeLists.txt` and run the cmake command you wrote in step 3.

## Troubleshooting `CMakeLists.txt`

* Every time you add / remove files from your project, you MUST add / remove files from the `set(PROGRAM_SRC )` statement!
* If you are debugging `CMakeLists.txt`, be sure to delete the ouput files if `cmake` fails.

## Using Docker

The included `Dockerfile` allows you to check that your program will build on Ubuntu. The `Dockerfile` can also be used to build programs if you cannot configure your own computer to do so. However, this is NOT recommended, as you must rebuild the Docker container every time you edit the source code.

To build the Docker container, run
```
docker build -ivt "$(src)"/build:/root/igvc-firmware/build swervi_mbed .
```
Note that the `-t "$(src)"/build:/root/igvc-firmware/build` binds the `build` directory on your host machine to the `build` directory inside the container. Therefore, when you build a program inside the container, the compiled binary file will reside in the `build/` directory on your host machine even after you close the container.

To open the Docker container, do
```
docker run -it swervi_mbed
```
The above command launches an interactive terminal inside the container running Ubuntu.
To exit the Docker container, you can either type `exit` in the interactive terminal or type in another terminal:
```
docker ps
```
Copy / Paste the `id` of the running container and type
```
docker stop [id]
```
to shut down the container.

