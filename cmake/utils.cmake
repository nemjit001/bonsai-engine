# Enable extended warnings for a target.
function(target_enable_extended_warnings TARGET_NAME)
    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
    endif()
endfunction()

# Copy DLL dependencies for a target to the target file dir.
# This is only required for external dependencies not managed by vcpkg
function(target_copy_dll_dependencies TARGET_NAME)
    if (WIN32)
        foreach (DLL_TARGET IN LISTS ARGN)
            message(STATUS "${TARGET_NAME}: Registered ${DLL_TARGET} for copy on build")
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${DLL_TARGET}> $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMENT "Copying dll target ${DLL_TARGET}"
                VERBATIM
            )
        endforeach()
    endif()
endfunction()

# Copy a list of asset files to a target build directory.
function(target_copy_asset_files TARGET_NAME)
    add_custom_target(${TARGET_NAME}_AssetCopy ALL)
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_AssetCopy)

    foreach (ASSET_FILE IN LISTS ARGN)
        message(STATUS "${TARGET_NAME}: Registered asset file ${ASSET_FILE} for copy on build")
        add_custom_command(TARGET ${TARGET_NAME}_AssetCopy POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${ASSET_FILE} $<TARGET_FILE_DIR:${TARGET_NAME}>/${ASSET_FILE}
            COMMENT "Copying asset file ${ASSET_FILE}"
            VERBATIM
        )
    endforeach()
endfunction()
