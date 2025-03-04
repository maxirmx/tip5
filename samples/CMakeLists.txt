include(FetchContent)
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG        v2.5.0
)
FetchContent_MakeAvailable(cli11)

add_executable(tip5xx_sample
    src/main.cpp
)

set_target_properties(tip5xx_sample PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(tip5xx_sample
    PRIVATE
        tip5xx::tip5xx
        CLI11::CLI11
)
