cmake_minimum_required(VERSION 3.20)
project(S2TConverter LANGUAGES CXX)

add_library(S2TConverter SHARED src/plugin.cpp S2TConverter.def)

target_compile_features(S2TConverter PRIVATE cxx_std_17)
target_compile_definitions(S2TConverter PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(S2TConverter PRIVATE user32)

if (MSVC)
    target_compile_options(S2TConverter PRIVATE /W4 /permissive-)
endif()

set_target_properties(
    S2TConverter
    PROPERTIES
        OUTPUT_NAME "S2TConverter"
        PREFIX ""
)
