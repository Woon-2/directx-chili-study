include(${CMAKE_SOURCE_DIR}/cmake/common_configuration.cmake)

function(WIN32APP_MINIMUM_CONFIGURATION targetName)

    COMMON_CONFIGURATION(${targetName})

    target_compile_options(${targetName}
    PRIVATE
        /MP     # compilation speed-up via multi processor
        /sdl    # security check
        /Ot     # maximum speed optimization
        /fp:fast    # rate speed over accuracy on floating points
    )

    target_compile_definitions(${targetName}
    PRIVATE
        UNICODE _UNICODE
        # disable unused window stuff, Keep Window.h light
        WIN32_LEAN_AND_MEAN
        NOGDICAPMASKS
        NOSYSMETRICS
        NOMENUS
        NOICONS
        NOSYSCOMMANDS
        NORASTEROPS
        OEMRESOURCE
        NOATOM
        NOCLIPBOARD
        NOCOLOR
        NOCTLMGR
        NODRAWTEXT
        NOKERNEL
        NONLS
        NOMEMMGR
        NOMETAFILE
        NOOPENFILE
        NOSCROLL
        NOSERVICE
        NOSOUND
        NOTEXTMETRIC
        NOWH
        NOCOMM
        NOKANJI
        NOHELP
        NOPROFILER
        NODEFERWINDOWPOS
        NOMCX
        NORPC
        NOPROXYSTUB
        NOIMAGE
        NOTAPE
        NOMINMAX
        # type safety
        # https://learn.microsoft.com/en-us/windows/win32/winprog/enabling-strict
        STRICT
    )

    target_link_options(${targetName}
    PRIVATE
        "/SUBSYSTEM:WINDOWS"
        "/ENTRY:WinMainCRTStartup"
    )

    # Release Options

    target_compile_definitions(${targetName}
    PRIVATE
        $<$<CONFIG:Release>:NDEBUG>     # Standard way to check release mode
    )

    target_compile_options(${targetName}
    PRIVATE
        $<$<CONFIG:Release>:/MT>        # Multi-threaded & Static libraries
    )

    # Debug Options

    target_compile_options(${targetName}
    PRIVATE
        $<$<CONFIG:Debug>:/MTd>         # Multi-threaded & Static libraries & debug options
    )

endfunction()