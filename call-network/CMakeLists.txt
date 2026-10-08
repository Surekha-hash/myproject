add_executable(
    vse-network

    src/main.cpp
    src/network_service.cpp

    ../common/socket.cpp
)

target_include_directories(
    vse-network
    PRIVATE
        include
        ../common
)

target_link_libraries(
    vse-network
    PRIVATE
        pthread
)
