add_executable(
        serialization_demo
        Serialization.cpp
)

target_include_directories(
        serialization_demo
        PRIVATE
        ${PROJECT_SOURCE_DIR}
)