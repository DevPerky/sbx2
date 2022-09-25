cmake_minimum_required(VERSION 3.20)
message(STATUS ${CMAKE_SYSTEM_NAME})
project(SBX2)

set(CMAKE_BUILD_TYPE Debug)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

cmake_path(CONVERT "../SbxAppBindings.xml" TO_NATIVE_PATH_LIST SbxAppBindings)
cmake_path(CONVERT "../SbxDrawBindings.xml" TO_NATIVE_PATH_LIST SbxDrawBindings)
cmake_path(CONVERT "../lua-definitions" TO_NATIVE_PATH_LIST LuaDefinitions)

cmake_path(CONVERT "../src/modules/app" TO_NATIVE_PATH_LIST AppModuleDir)
cmake_path(CONVERT "../src/modules/draw" TO_NATIVE_PATH_LIST DrawModuleDir)

add_subdirectory(glfw)
add_subdirectory(tinyxml2)
add_subdirectory(Lua)
add_subdirectory(glm)

find_package(OpenGL REQUIRED)

add_executable(sbx 
    src/main.cpp
    src/modules/app/app.cpp
    src/modules/app/AB-App-implementation.c
    src/modules/draw/AB-Draw-implementation.c
    src/modules/draw/draw.cpp
)

add_executable(autobind 
    autobind/src/main.cpp
    autobind/src/autobind-file.cpp
    autobind/src/binding-generator.cpp
    autobind/src/code-writer.cpp
    autobind/src/definitions-generator.cpp
)

target_link_libraries(sbx lua_static)
target_link_libraries(sbx glfw)
target_link_libraries(sbx OpenGL::GL)
target_include_directories(sbx PUBLIC glm)

target_link_libraries(autobind tinyxml2)


add_custom_command(
    TARGET sbx
    PRE_BUILD 
    COMMAND autobind "${SbxAppBindings}" "${AppModuleDir}" "${LuaDefinitions}"
)

add_custom_command(
    TARGET sbx
    PRE_BUILD
    COMMAND autobind "${SbxDrawBindings}" "${DrawModuleDir}" "${LuaDefinitions}"
)