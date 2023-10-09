function(ACTIVATE_GITLOG)
    if(NOT DEFINED GIT_FOUND)
        find_package(Git)
    endif()

    if(NOT GIT_FOUND)
        message(STATUS "from gitlog.cmake: Cannot find Git executable.")
        message(STATUS "Custom target 'gitlog' will not work. (it has no side effect to other components.)")
        return()
    endif()

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
        message(STATUS "from gitlog.cmake: Cannot find .git directory from ${CMAKE_CURRENT_SOURCE_DIR}.")
        message(STATUS "Custom target 'gitlog' will not work. (it has no side effect to other components.)")
        return()
    endif()

    add_custom_target(gitlog
        COMMENT "Git Comment Log:"
        # note: do not wrap the COMMAND with ""
        COMMAND ${GIT_EXECUTABLE} log --pretty=reference -6
    )
endfunction()