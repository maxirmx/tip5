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
)
