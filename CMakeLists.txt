cmake_minimum_required(VERSION 3.24)

option(CMAKE_COMPILE_WARNING_AS_ERROR "Enable warnings-as-error" ON)

project(stm32-devcontainer-template LANGUAGES C CXX ASM VERSION 1.0.0) # x-release-please-version

# Add debug flags
set(CMAKE_C_FLAGS_DEBUG "-g3 -gdwarf-2 -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -gdwarf-2 -O0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_directory_properties(PROPERTY USE_FOLDERS ON)

add_subdirectory(libs/amp-embedded-infra-lib)
add_subdirectory(libs/amp-hal-st)
add_subdirectory(src)

# Workaround for arm-none-eabi-gcc + mbedtls bignum_core inline asm in non-optimized builds.
if(TARGET mbedcrypto)
    target_compile_options(mbedcrypto PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-O2>
    )
endif()

# Add Google Test and enable testing only for the host system
if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(libs/googletest)
    enable_testing()
    add_subdirectory(tests)
endif()

emil_clangformat_directories(stm32-devcontainer-template DIRECTORIES ./src)
emil_folderize_all_targets()