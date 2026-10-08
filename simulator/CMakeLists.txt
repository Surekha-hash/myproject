add_executable(
    call-simulator

    src/main.cpp

    ../common/socket.cpp
)

target_include_directories(
    call-simulator
    PRIVATE
        ../common
)

target_link_libraries(
    call-simulator
    PRIVATE
        pthread
)
