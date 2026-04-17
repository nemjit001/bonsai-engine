project("BonsaiRuntime"
    VERSION 0.0.0
    LANGUAGES CXX
    DESCRIPTION
    "The Bonsai Runtime library handles application startup in a platform agnostic manner."
)

add_library(BonsaiRuntime STATIC)
add_library(Bonsai::Runtime ALIAS BonsaiRuntime)
target_compile_features(BonsaiRuntime PRIVATE cxx_std_17)
target_sources(BonsaiRuntime
    PRIVATE
        src/Entrypoint.cpp
)
target_link_libraries(BonsaiRuntime
    PUBLIC
        Bonsai::Base
)
