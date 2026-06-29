project("BonsaiRHI"
    VERSION 0.0.1
    DESCRIPTION "Render Hardware Interface for the Bonsai Engine."
    LANGUAGES CXX
)

# Create library target
add_library(BonsaiRHI SHARED)
add_library(Bonsai::RHI ALIAS BonsaiRHI)

# Generate export headers
include(GenerateExportHeader)
generate_export_header(BonsaiRHI
    BASE_NAME           "BONSAI_RHI"
    EXPORT_FILE_NAME    "BonsaiRHIExport.hpp"
    INCLUDE_GUARD_NAME  "BONSAI_RHI_EXPORT_HPP"
)

# Configure library
target_sources(BonsaiRHI
    PRIVATE
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/BonsaiRHI.hpp
    PUBLIC
    FILE_SET generated_headers
    TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/BonsaiRHIExport.hpp
)
target_include_directories(BonsaiRHI PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(BonsaiRHI PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiRHI)
target_link_libraries(BonsaiRHI
    PUBLIC
        Bonsai::Base
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
