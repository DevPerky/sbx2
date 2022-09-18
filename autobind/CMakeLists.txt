cmake_minimum_required(VERSION 3.10)

project(autobind)


add_executable(autobind src/main.cpp src/autobind-file.cpp src/binding-generator.cpp)

target_link_libraries(autobind tinyxml2)