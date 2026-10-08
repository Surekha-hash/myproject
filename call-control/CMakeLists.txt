add_executable(
    vse-control

    src/main.cpp
    src/call_controller.cpp

    ../common/socket.cpp
)

target_include_directories(
    vse-control
    PRIVATE
        include
        ../common
)

target_link_libraries(
    vse-control
    PRIVATE
        pthread
)
