# Copy target runtime dll dependencies to the target output directory.
function(target_copy_runtime_dlls TARGET_NAME)
    if (WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMENT "Copying DLL dependencies"
            VERBATIM
        )
    endif()
endfunction()

# Copy asset files to the target output directory.
function(target_copy_resource_files TARGET_NAME)
    foreach(RESOURCE_PATH IN LISTS ARGN)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_PATH} $<TARGET_FILE_DIR:${TARGET_NAME}>/${RESOURCE_PATH}
            COMMENT "Copying resource file: ${RESOURCE_PATH}"
            VERBATIM
        )
    endforeach()    
endfunction()

# Enable extended warnings for a target.
function(target_extended_warnings TARGET_NAME)
    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wshadow)
    endif()
endfunction()
