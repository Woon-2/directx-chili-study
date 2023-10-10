function(add_gitlog LOG_COUNT)
    if(NOT DEFINED GIT_FOUND)
        find_package(Git)
    endif()

    if(NOT GIT_FOUND)
        message(STATUS "from gitlog.cmake: Cannot find Git executable.")
        message(STATUS "Custom target 'gitlog' is not generated. (it has no side effect to other components.)")
        return()
    endif()

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
        message(STATUS "from gitlog.cmake: Cannot find .git directory from ${CMAKE_CURRENT_SOURCE_DIR}.")
        message(STATUS "Custom target 'gitlog' will not work. (it has no side effect to other components.)")
        return()
    endif()

    if(NOT TARGET gitlog)
        add_custom_target(gitlog
            COMMENT "Git Comment Log:"
            COMMAND "${GIT_EXECUTABLE}" log --pretty=reference -${LOG_COUNT}
        )
    endif()
endfunction()