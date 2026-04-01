cmake_minimum_required(VERSION 3.21)

project(MyProject VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

if (TARGET_MCU_VENDOR STREQUAL "st")
    set(EMIL_INCLUDE_MBEDTLS Off CACHE BOOL "Include MbedTLS as part of EmIL" FORCE)
endif()

FetchContent_Declare(
    emil
    GIT_REPOSITORY https://github.com/philips-software/embeddedinfralib.git
    GIT_TAG        main
)

FetchContent_Declare(
    halst
    GIT_REPOSITORY https://github.com/philips-software/amp-hal-st.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(emil halst)

add_executable(myprogram "src/Main.cpp")

target_link_libraries(myprogram PUBLIC
    infra.event
)

if (TARGET_MCU_VENDOR STREQUAL "st")
    target_link_libraries(myprogram PUBLIC
        services.util
        hal_st.stm32fxxx
        hal_st.instantiations
    )

    halst_target_default_linker_scripts(myprogram)
    halst_target_default_init(myprogram)
else()
    target_link_libraries(myprogram PUBLIC
        hal.unix
    )
endif()
