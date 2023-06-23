function(LINK_RESOURCE targetName)
    # Set Resource Path
    target_compile_definitions(${targetName}
    PRIVATE
        RESOURCE_PATH=${CMAKE_SOURCE_DIR}/resource
    )
endfunction()