add_executable(
    vse-billing

    src/main.cpp
    src/billing_engine.cpp

    ../common/socket.cpp
)

target_include_directories(
    vse-billing
    PRIVATE
        include
        ../common
)

target_link_libraries(
    vse-billing
    PRIVATE
        pthread
)
