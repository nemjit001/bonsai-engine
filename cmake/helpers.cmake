# Copy target runtime dll dependencies to the target output directory.
function(target_copy_runtime_dlls TARGET_NAME)
    if (WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND     ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMENT     "Copying DLL dependencies"
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    endif()
endfunction()

# Copy asset files to the target output directory.
function(target_copy_resource_files TARGET_NAME)
    # Set up resource copy commands
    foreach(RESOURCE_PATH IN LISTS ARGN)
        set(RESOURCE_INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_PATH}")
        set(RESOURCE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${RESOURCE_PATH}")

        add_custom_command(OUTPUT ${RESOURCE_OUTPUT_PATH}
            COMMAND     ${CMAKE_COMMAND} -E copy ${RESOURCE_INPUT_PATH} ${RESOURCE_OUTPUT_PATH}
            COMMENT     "Copying resource file: ${RESOURCE_PATH}"
            DEPENDS     ${RESOURCE_INPUT_PATH}
            VERBATIM
        )

        list(APPEND RESOURCE_FILE_DEPENDENCIES ${RESOURCE_OUTPUT_PATH})
    endforeach()

    # Set up top-level copy target
    add_custom_target("${TARGET_NAME}_CopyResourceFiles"
        COMMENT "${TARGET_NAME}: Copying target resource files"
        DEPENDS ${RESOURCE_FILE_DEPENDENCIES}
    )
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_CopyResourceFiles)
endfunction()

# Enable extended warnings for a target.
function(target_extended_warnings TARGET_NAME)
    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
    endif()
endfunction()
