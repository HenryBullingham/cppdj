# Copyright(C) 2020 Henry Bullingham
# This file is subject to the license terms in the LICENSE file
# found in the top - level directory of this distribution.


add_executable(cppdj_example cppdj_example.cpp)            
target_link_libraries(cppdj_example PUBLIC cppdj)
target_include_directories(cppdj_example PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET cppdj_example PROPERTY CXX_STANDARD 11)
