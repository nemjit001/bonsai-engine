project("BonsaiBase"
    VERSION 0.0.0
    LANGUAGES CXX
    DESCRIPTION
    "The Bonsai Base library defines basic types and exposes low level interfaces for constructing applications."
)

add_library(BonsaiBase STATIC)
add_library(Bonsai::Base ALIAS BonsaiBase)
target_compile_features(BonsaiBase PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiBase)
target_sources(BonsaiBase
    PRIVATE
        src/Base/PlatformDefines.hpp
        src/Base/PlatformTypes.cpp
        src/Base/PlatformTypes.hpp
)
target_sources(BonsaiBase
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/Base/PlatformDefines.hpp
        src/Base/PlatformTypes.hpp
)
