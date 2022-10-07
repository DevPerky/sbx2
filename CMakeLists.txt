cmake_minimum_required(VERSION 3.20)
message(STATUS ${CMAKE_SYSTEM_NAME})
project(SBX2)

set(CMAKE_BUILD_TYPE Debug)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

cmake_path(SET SbxAppBindings   "${CMAKE_CURRENT_SOURCE_DIR}/SbxAppBindings.xml" )
cmake_path(SET SbxDrawBindings  "${CMAKE_CURRENT_SOURCE_DIR}/SbxDrawBindings.xml")
cmake_path(SET LuaDefinitions   "${CMAKE_CURRENT_SOURCE_DIR}/lua-definitions")

cmake_path(SET AppModuleDir     "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/app")
cmake_path(SET DrawModuleDir    "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/draw")

add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/tinyxml2)
add_subdirectory(dependencies/Lua)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/glad)

find_package(OpenGL REQUIRED)

add_executable(sbx 
    src/main.cpp
    src/modules/app/app.cpp
    src/modules/app/AB-App-implementation.c
    src/modules/draw/AB-Draw-implementation.c
    src/modules/draw/draw.cpp
    src/modules/draw/primitive.cpp
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
target_link_libraries(sbx glad)
target_include_directories(sbx PUBLIC dependencies/glad/include)
target_include_directories(sbx PUBLIC dependencies/glm)
target_include_directories(sbx PUBLIC src)

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