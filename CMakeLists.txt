# mbed-cmake requires at least CMake 3.12 
cmake_minimum_required(VERSION 3.12)
# set(CMAKE_CXX_FLAGS "-g")
# set(CMAKE_BUILD_TYPE Debug)

# Set BUILD_PROGRAM in the Command Line:
#   cmake -D BUILD_PROGRAM="MAIN" -B OutputCMake/main/
#   cmake -D BUILD_PROGRAM="TEST_BLINK" -B OutputCMake/test/blink/
#   cmake -D BUILD_PROGRAM="TEST_ETHERNET_CLIENT" -B OutputCMake/test/EthernetTest/mbed/
#   cmake -D BUILD_PROGRAM="TEST_ETHERNET_SERVER" -B OutputCMake/test/EthernetTest/computer/
#   cmake -D BUILD_PROGRAM="TEST_LIDAR_LITE_MBED" -B OutputCMake/test/LidarLite/mbed/
#   cmake -D BUILD_PROGRAM="TEST_LIDAR_LITE_COMP" -B OutputCMake/test/LidarLite/computer/
#   cmake -D BUILD_PROGRAM="TEST_CAN" -B OutputCMake/test/CANTest/mbed/
#   cmake -D BUILD_PROGRAM="TEST_SWERVE_DRIVE_MBED" -B OutputCMake/test/SwerveDriveModule/mbed/
#   cmake -D BUILD_PROGRAM="TEST_SWERVE_DRIVE_COMP" -B OutputCMake/test/SwerveDriveModule/comp/

# You can also set BUILD_PROGRAM by uncommenting one of the following:

# set(BUILD_PROGRAM MAIN)
# set(BUILD_PROGRAM TEST_BLINK)
# set(BUILD_PROGRAM TEST_ETHERNET_CLIENT)
# set(BUILD_PROGRAM TEST_ETHERNET_SERVER)
# set(BUILD_PROGRAM TEST_LIDAR_LITE_MBED)
# set(BUILD_PROGRAM TEST_LIDAR_LITE_COMP)
# set(BUILD_PROGRAM TEST_CAN)

option(BUILD_PROTOBUF   "Build Protobuffer" OFF)
option(BUILD_MBED       "Build program for mbed" OFF)

if (${BUILD_PROGRAM} STREQUAL TEST_ETHERNET_SERVER)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/test/EthernetTest/EthernetTestServer)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp
            ${SRC_DIR}/ParseProtobuf.cpp
            ${SRC_DIR}/EthernetSocket.cpp
            )
    set(PROTOS ${SRC_DIR}/count.proto)
    set(BUILD_PROTOBUF ON)
    set(BUILD_MBED OFF)
elseif (${BUILD_PROGRAM} STREQUAL TEST_ETHERNET_CLIENT)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/test/EthernetTest/EthernetTestClient)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp
            ${SRC_DIR}/ParseProtobufMbed.cpp
            ${SRC_DIR}/communication/EthernetSocket.cpp
            )
    set(PROTOS ${SRC_DIR}/count.proto)
    set(BUILD_PROTOBUF ON)
    set(BUILD_MBED ON)
elseif (${BUILD_PROGRAM} STREQUAL TEST_LIDAR_LITE_MBED)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/test/LidarLite/mbed)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp
            ${SRC_DIR}/lidar_lite/LidarLite.cpp
            ${SRC_DIR}/lidar_lite/LidarLiteController.cpp
            ${SRC_DIR}/communication/ParseProtobufMbed.cpp
            )
    set(PROTOS ${SRC_DIR}/communication/lidar_lite.proto)
    set(BUILD_PROTOBUF ON)
    set(BUILD_MBED ON)
elseif (${BUILD_PROGRAM} STREQUAL TEST_LIDAR_LITE_COMP)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/test/LidarLite/computer)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp
            ${SRC_DIR}/communication/ParseProtobuf.cpp
            ${SRC_DIR}/communication/EthernetSocket.cpp
            )
    set(PROTOS ${SRC_DIR}/communication/lidar_lite.proto)
    set(BUILD_PROTOBUF ON)
    set(BUILD_MBED OFF)
elseif (${BUILD_PROGRAM} STREQUAL TEST_CAN)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/test/CANTest/mbed)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp)
    set(BUILD_PROTOBUF OFF)
    set(BUILD_MBED ON)
elseif (${BUILD_PROGRAM} STREQUAL TEST_SWERVE_DRIVE_MBED)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/test/SwerveDriveModule/mbed)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp)
    set(BUILD_PROTOBUF OFF)
    set(BUILD_MBED ON)
elseif (${BUILD_PROGRAM} STREQUAL TEST_SWERVE_DRIVE_COMP)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/test/SwerveDriveModule/computer)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp
            ${SRC_DIR}/communication/ParseProtobuf.cpp
            ${SRC_DIR}/communication/EthernetSocket.cpp
            )
    set(PROTOS ${SRC_DIR}/communication/swerve_commands.proto)
    set(BUILD_PROTOBUF ON)
    set(BUILD_MBED OFF)
else() 
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/test/blink)    
    set(PROGRAM_SRC ${SRC_DIR}/main.cpp)
    set(BUILD_PROTOBUF OFF)
    set(BUILD_MBED ON)
endif()


if (${BUILD_MBED})

    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
    set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    set(MBED_CMAKE_SOURCE_DIR ${LIB_DIR}/mbed-cmake)

    # install additional dependecies
    execute_process(COMMAND python3 -m  pip install -r
        ${MBED_CMAKE_SOURCE_DIR}/mbed-src/requirements.txt)

    # configure mbed library for LPC1768
    execute_process(COMMAND python3 ${MBED_CMAKE_SOURCE_DIR}/configure_for_target.py LPC1768)

    # load project upload methods (must be before mbed-cmake is loaded)
    # -------------------------------------------------------------

    set(UPLOAD_METHOD_DEFAULT MBED)
    set(MBED_UPLOAD_ENABLED TRUE)
    include(${LIB_DIR}/mbed-cmake-config/cmake/MbedOSConfig-LPC1768.cmake)

    # load compilers and flags
    # -------------------------------------------------------------

    include(${MBED_CMAKE_SOURCE_DIR}/mbed-cmake.cmake)

    set(boost_system_FOUND TRUE)
endif()

# project(swervi_mbed LANGUAGES NONE)
# project(swervi_mbed)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
# set(CMAKE_CXX_STANDARD 14)

project(swervi_mbed)
    
# Boost libs
# if (NOT ${BUILD_MBED} AND NOT boost_system_FOUND)
if (NOT boost_system_FOUND)
    # set(boost_system_FOUND TRUE)
    find_package(Boost 1.76.0 REQUIRED COMPONENTS system)
    include_directories(${Boost_INCLUDE_DIRS})
    message("boost lib: ${Boost_LIBRARIES}")
    message("boost inc: ${Boost_INCLUDE_DIR}")
endif()

if (${BUILD_PROTOBUF}) 
    
    # NanoPB Config ~~~~~~~~~
    
    message(STATUS "Creating NanoPB target...")
    set(NANOPB_SRC_ROOT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/lib/nanopb)
    set(CMAKE_MODULE_PATH ${NANOPB_SRC_ROOT_FOLDER}/extra)
    find_package(Nanopb REQUIRED)
    
    include_directories(${NANOPB_INCLUDE_DIRS})
    nanopb_generate_cpp(PROTO_GENERATED_SRCS PROTO_HDRS ${PROTOS})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    # Find the cpp files in the nanopb repo though and mark those as not generated, otherwise make clean removes them
    file(GLOB PROTO_SRCS "${NANOPB_SRC_ROOT_FOLDER}/*.c" "${NANOPB_SRC_ROOT_FOLDER}/*.h")
    
    set_source_files_properties(${PROTO_GENERATED_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)
    set_source_files_properties(${PROTO_SRCS} PROPERTIES GENERATED TRUE)
    
    message(STATUS "Creating NanoPB target...Done!")

    #~~~~~~~~~~~~~~~~~~~~~~~
    
    set(PROTO_FILES ${PROTO_GENERATED_SRCS} ${PROTO_HDRS})
endif()


if (${BUILD_MBED}) 

    # recurse to subdirectories
    # -------------------------------------------------------------
    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    
    
    if (${BUILD_PROGRAM} STREQUAL TEST_ETHERNET_CLIENT)
        add_mbed_executable(test_ethernet_client ${PROGRAM_SRC} ${PROTO_FILES})
    elseif (${BUILD_PROGRAM} STREQUAL TEST_BLINK)
        add_mbed_executable(blink ${PROGRAM_SRC} ${PROTO_FILES})
    elseif (${BUILD_PROGRAM} STREQUAL TEST_LIDAR_LITE_MBED)
        add_mbed_executable(test_lidar_lite_mbed ${PROGRAM_SRC} ${PROTO_FILES})
    elseif (${BUILD_PROGRAM} STREQUAL TEST_CAN)
        add_mbed_executable(test_can ${PROGRAM_SRC} ${PROTO_FILES})
    elseif (${BUILD_PROGRAM} STREQUAL TEST_SWERVE_DRIVE_MBED)
        add_mbed_executable(test_swerve_drive_mbed ${PROGRAM_SRC} ${PROTO_FILES})
    endif()

    # build report
    # -------------------------------------------------------------

    mbed_cmake_print_build_report()
    mbed_cmake_finalize()

else()
 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
    set(CMAKE_CXX_STANDARD 14)

    # project(swervi_mbed LANGUAGES NONE)
 
    if (${BUILD_PROGRAM} STREQUAL TEST_ETHERNET_SERVER)
        add_executable(test_ethernet_server ${PROGRAM_SRC} ${PROTO_FILES}) 

        # -lnosys: used by ARM mbed specifically
        target_link_libraries(test_ethernet_server -lstdc++ -lsupc++ -lm -lc -lgcc ${Boost_LIBRARIES}) 
    elseif (${BUILD_PROGRAM} STREQUAL TEST_LIDAR_LITE_COMP)
        add_executable(test_lidar_lite_comp ${PROGRAM_SRC} ${PROTO_FILES}) 

        # -lnosys: used by ARM mbed specifically
        target_link_libraries(test_lidar_lite_comp -lstdc++ -lsupc++ -lm -lc -lgcc ${Boost_LIBRARIES})
    elseif (${BUILD_PROGRAM} STREQUAL TEST_SWERVE_DRIVE_COMP)
        add_executable(test_swerve_drive_comp ${PROGRAM_SRC} ${PROTO_FILES}) 

        # -lnosys: used by ARM mbed specifically
        target_link_libraries(test_swerve_drive_comp -lstdc++ -lsupc++ -lm -lc -lgcc ${Boost_LIBRARIES})
    endif()
    
    
 
endif()


