# docker exec -it swervi_firmware bash
# docker build -t swervi_firmware .
# docker run --rm -it --name="swervi_firmware" -v ${PWD}:/root/igvc-firmware swervi_firmware

# FROM ubuntu:bionic
FROM ubuntu:focal

ENV DEBIAN_FRONTEND noninteractive

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
        python3-pip \
        clang-tidy \
        wget && \
    apt-get clean && \
    # downloading the mbed compiler
    wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/8-2019q3/RC1.1/gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2 -O cortex_m.tar.bz2 && \
    tar -xjf cortex_m.tar.bz2 -C /opt/ && \
    rm cortex_m.tar.bz2 && \

    # cd /opt/boost_1_77_0 && \
    # ./bootstrap.sh && \
    # ./b2 && \
    # ./b2 install && \
    # cd / && \
    # rm boost_1_77_0.tar.gz && \

    # setup python pip
    python3 -m pip install intelhex prettytable

ENV PATH "/usr/bin/ccache:/opt/gcc-arm-none-eabi-8-2019-q3-update/bin:$PATH"

WORKDIR /root/igvc-firmware

# install python3 protobuf
RUN apt-get install -y \
        python3-protobuf

# download boost libraries
# RUN wget https://sourceforge.net/projects/boost/files/boost/1.77.0/boost_1_77_0.tar.bz2/download -O boost_1_77_0.tar.gz && \
#     tar -xvf boost_1_77_0.tar.gz -C /opt/ 

# RUN /opt/boost_1_77_0/bootstrap.sh

# RUN /opt/boost_1_77_0/b2 install
    # /opt/boost_1_77_0/b2
# RUN ./opt/boost_1_77_0/b2
    # ./b2
    # ./b2 install
    # cd / && \
    # rm boost_1_77_0.tar.gz