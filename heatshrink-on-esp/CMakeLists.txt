# Minimum CMake version required
cmake_minimum_required(VERSION 3.5)

# Include ESP-IDF project build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Name of the project
project(heatshrink-on-esp)
spiffs_create_partition_image(spiffs ${CMAKE_SOURCE_DIR}/spiffs FLASH_IN_PROJECT)
