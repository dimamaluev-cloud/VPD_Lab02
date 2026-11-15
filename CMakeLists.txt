cmake_minimum_required(VERSION 3.20)

project(VPD_Lab2 LANGUAGES C)

add_executable(vpd_lab2
    main.c
    device.c
    timer.c
)

target_compile_options(vpd_lab2 PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)