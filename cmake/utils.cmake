function(target_extended_warnings TARGET_NAME)
    if (WIN32)
        target_compile_options(${TARGET_NAME} PUBLIC /W4)
    else()
        target_compile_options(${TARGET_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow)
    endif()
endfunction()

function(target_track_runtime_dll_dependencies TARGET_NAME)
    if (WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
    endif()
endfunction()

