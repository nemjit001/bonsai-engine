project("BonsaiBase"
    VERSION 0.0.0
    LANGUAGES CXX
)

add_library(BonsaiBase STATIC)
add_library(Bonsai::Base ALIAS BonsaiBase)
target_compile_features(BonsaiBase PRIVATE cxx_std_17)
target_sources(BonsaiBase
    PUBLIC
    src/PlatformTypes.cpp
    src/PlatformTypes.hpp
)
target_sources(BonsaiBase
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
    src/PlatformTypes.hpp
)
