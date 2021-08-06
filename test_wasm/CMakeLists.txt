############################################################################
# Copyright (c) 2016, Sylvain Corlay, Johan Mabille, Martin Renou          #
# Copyright (c) 2016, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

# Unit tests
# ==========

cmake_minimum_required(VERSION 3.1)

message(STATUS "Forcing tests build type to Release")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if(nlohmann_json_FOUND)
  add_definitions(-DHAVE_NLOHMANN_JSON)
endif()




# set(EMSCRIPTEN_FEATURES "-s WASM=1")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FEATURES}")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSCRIPTEN_FEATURES}")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${EMSCRIPTEN_FEATURES}")


find_package(Threads)



if(nlohmann_json_FOUND)
    # Version up to 3.1.2 export the target `nlohmann_json`
    if(TARGET nlohmann_json)
      set(nlohmann_json_TARGET nlohmann_json)
    # Newer versions export the namespaced target `nlohmann_json::nlohmann_json`
    elseif(TARGET nlohmann_json::nlohmann_json)
      set(nlohmann_json_TARGET nlohmann_json::nlohmann_json)
    endif()
endif()

set(xeus_TARGET xeus-static)






# Test_kernel_shell tests
# =======================

set(fobar.cpp)
add_executable(fobar fobar.cpp
    test_interpreter.cpp)
target_link_libraries(fobar PRIVATE ${xeus_TARGET})# Threads::Threads)
# target_compile_features(test_kernel_shell PRIVATE cxx_std_11)

# set(CONNECTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/connection.json)

# configure_file(
#     "${XEUS_TEST_DIR}/test_kernel_shell.py"
#     "${CMAKE_CURRENT_BINARY_DIR}/"
#     COPYONLY)
