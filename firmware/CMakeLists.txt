cmake_minimum_required(VERSION 3.5)

set(PROJECT_VER "0.2")

set(EXTRA_COMPONENT_DIRS
    target/esp32/main/
    $ENV{IDF_PATH}/examples/common_components/protocol_examples_common
    $ENV{IDF_PATH}/examples/system/console/components
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(crs19)
