cmake_minimum_required(VERSION 3.28)
project("Bonsai Engine" VERSION 0.1.0)

add_executable(BonsaiEngine)
target_sources(BonsaiEngine
    PRIVATE
        src/Application.cpp
        src/Application.hpp
        src/main.cpp
)
target_compile_features(BonsaiEngine
    PRIVATE
        cxx_std_17
)
