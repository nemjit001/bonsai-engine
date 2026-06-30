project("BonsaiRHI"
    VERSION 0.0.1
    DESCRIPTION "Render Hardware Interface for the Bonsai Engine."
    LANGUAGES CXX
)

# Create library target
add_library(BonsaiRHI SHARED)
add_library(Bonsai::RHI ALIAS BonsaiRHI)

# Configure library
target_sources(BonsaiRHI
    PRIVATE
        src/BonsaiRHI.cpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/BonsaiRHI.hpp
        src/BonsaiRHIExport.hpp
)
target_compile_features(BonsaiRHI PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiRHI)
target_link_libraries(BonsaiRHI
    PUBLIC
        Bonsai::Base
)
target_compile_definitions(BonsaiRHI
    PRIVATE
        BONSAI_RHI_EXPORT_SYMBOLS=1
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Add install target
install(
    TARGETS BonsaiRHI
    FILE_SET HEADERS
    FILE_SET generated_headers
)
