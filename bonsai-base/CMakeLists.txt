project("BonsaiBase"
    VERSION 0.0.1
    DESCRIPTION "Platform Independence Layer for the Bonsai Engine."
    LANGUAGES CXX
)

add_library(BonsaiBase)
add_library(Bonsai::Base ALIAS BonsaiBase)
target_sources(BonsaiBase
    PRIVATE
        src/Base/Allocator.cpp
        src/Base/Assert.cpp
        src/Base/DebugOutput.cpp
        src/Base/Memory.cpp
        src/Base/Timer.cpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/BonsaiBase.hpp
        src/Base/Allocator.hpp
        src/Base/Assert.hpp
        src/Base/DebugOutput.hpp
        src/Base/Memory.hpp
        src/Base/PlatformDefines.hpp
        src/Base/PlatformTypes.hpp
        src/Base/Timer.hpp
        src/Containers/DynamicArray.hpp
        src/Containers/HashMap.hpp
        src/Containers/StaticArray.hpp
        src/Containers/String.hpp
        src/Containers/StringView.hpp
)
target_compile_features(BonsaiBase PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiBase)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
