FROM ubuntu:bionic

# ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        build-essential \
        git \
        bzip2 \
        cmake \
        python \
        protobuf-compiler \
        python-protobuf \
        python-pip \
        clang-tidy \
        wget && \
    apt-get clean && \
    wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/8-2019q3/RC1.1/gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2 -O cortex_m.tar.bz2 && \
    tar -xjf cortex_m.tar.bz2 -C /opt/ && \
    rm cortex_m.tar.bz2 && \
    python -m pip install intelhex prettytable

ENV PATH "/usr/bin/ccache:/opt/gcc-arm-none-eabi-8-2019-q3-update/bin:$PATH"

WORKDIR /root/igvc-firmware
COPY . .
