cmake_minimum_required(VERSION 3.10)
set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} 8)  # building with multiple CPU cores, saving life.
set(OpenGL_GL_PREFERENCE GLVND)
#set(ENV{CUDACXX} /usr/local/cuda/bin/nvcc) 

project(Titanic LANGUAGES C CXX CUDA) # use CXX, CUDA by default (since CUDA is a language, don't need cuda_add_executable)

set(TITAN_DIR ${CMAKE_CURRENT_LIST_DIR}/Titan)
set(CMAKE_CXX_STANDARD 14) # set C++ standard to C++14

set(TITAN_SOURCE_DIR ${TITAN_DIR}/src)
set(TITAN_INCLUDE_DIR ${TITAN_DIR}/include/Titan)

# set(TITAN_SOURCE_FILES ${TITAN_SOURCE_DIR}/vec.cu ${TITAN_SOURCE_DIR}/sim.cu ${TITAN_SOURCE_DIR}/shader.cpp
#         ${TITAN_SOURCE_DIR}/mass.cu ${TITAN_SOURCE_DIR}/spring.cu
#         ${TITAN_SOURCE_DIR}/object.cu ${TITAN_SOURCE_DIR}/graphics.cpp)
file( GLOB TITAN_SOURCE_FILES ${TITAN_SOURCE_DIR}/*.c*)
# set(TITAN_HEADER_FILES ${TITAN_INCLUDE_DIR}/sim.h ${TITAN_INCLUDE_DIR}/mass.h ${TITAN_INCLUDE_DIR}/object.h
#         ${TITAN_INCLUDE_DIR}/spring.h ${TITAN_INCLUDE_DIR}/vec.h ${TITAN_INCLUDE_DIR}/graphics.h ${TITAN_INCLUDE_DIR}/shader.h
#         ${TITAN_INCLUDE_DIR}/stlparser.h)
file( GLOB TITAN_HEADER_FILES ${TITAN_INCLUDE_DIR}/*.h*)

if (TITAN_SHARED_BUILD)
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(POSITION_INDEPENDENT_CODE ON)
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(POSITION_INDEPENDENT_CODE ON)

add_executable(Titan ${TITAN_SOURCE_FILES} ${TITAN_HEADER_FILES} ${CMAKE_CURRENT_LIST_DIR}/src/experiments/exp_8.cu )

set_target_properties(Titan PROPERTIES CUDA_SEPARABLE_COMPILATION ON) # needed for library compilation
set_target_properties(Titan PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON)

target_compile_definitions(Titan PUBLIC GRAPHICS)
target_compile_definitions(Titan PUBLIC CONSTRAINTS) # defines the CONSTRAINTS preprocessor variable (enables local constraints)

if (NOT TITAN_SHARED_BUILD)
    target_compile_definitions(Titan PUBLIC _MWAITXINTRIN_H_INCLUDED)
    #target_compile_features(Titan PUBLIC cxx_std_11) # same as above, may be unnecessary
endif()

find_package(CUDA REQUIRED) # find and include CUDA
if (CUDA_FOUND)
    message(STATUS "CUDA FOUND")
    target_include_directories(Titan PUBLIC ${CUDA_INCLUDE_DIRS})
    target_link_libraries(Titan PRIVATE cuda) # cudart
else()
    message(STATUS "CUDA NOT FOUND")
endif()

if (UNIX)
    find_package(OpenGL REQUIRED) # find and include OpenGL
    if (OpenGL_FOUND)
        message(STATUS "OPENGL FOUND")
        target_include_directories(Titan PRIVATE ${OpenGL_INCLUDE_DIRS})
        target_link_libraries(Titan PRIVATE ${OpenGL_LIBRARIES} GL)
    endif()
else()
    find_package(OPENGL REQUIRED) # find and include OpenGL
    if (OPENGL_FOUND)
        message(STATUS "OPENGL FOUND")
        target_include_directories(Titan PRIVATE ${OPENGL_INCLUDE_DIRS})
        target_link_libraries(Titan PRIVATE ${OPENGL_LIBRARIES})
    endif()
endif()

find_package(glfw3 CONFIG REQUIRED)
if (glfw3_FOUND)
    message(STATUS "GLFW FOUND")
    target_include_directories(Titan PRIVATE ${glfw3_INCLUDE_DIRS})
    target_link_libraries(Titan PRIVATE ${glfw3_LIBRARIES})
endif()

find_package(GLEW REQUIRED) # GLEW
if (GLEW_FOUND)
    message(STATUS "GLEW FOUND")
    target_include_directories(Titan PRIVATE ${GLEW_INCLUDE_DIRS})
    target_link_libraries(Titan PRIVATE ${GLEW_LIBRARIES} glfw)
endif()

find_package(glm CONFIG REQUIRED) # glm
if (glm_FOUND)
    message(STATUS "GLM FOUND")
    target_include_directories(Titan PRIVATE ${glm_INCLUDE_DIRS})
    target_link_libraries(Titan PRIVATE ${glm_LIBRARIES} glm)
endif()

target_include_directories(Titan PUBLIC
        $<BUILD_INTERFACE:${TITAN_DIR}/include/Titan>
        $<INSTALL_INTERFACE:include>)

set_target_properties(Titan PROPERTIES PUBLIC_HEADER "${TITAN_HEADER_FILES}")

# added to compile experiments
find_package(nlohmann_json REQUIRED)
# this package can be installed through source https://github.com/nlohmann/json

# set(TITANIC_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
# set(TITANIC_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

# file( GLOB TITANIC_SOURCE_FILES ${TITANIC_SOURCE_DIR}/*.c*)
# file( GLOB EXP_SOURCE_FILES ${TITANIC_SOURCE_DIR}/experiments/*.c* )
# foreach( sourcefile ${EXP_SOURCE_FILES} )
#     get_filename_component(name ${sourcefile} NAME_WE)
#     message("Add Executable: " ${name})
#     add_executable(${name} ${sourcefile} ${TITANIC_SOURCE_FILES})
#     # target_include_directories(${name} PUBLIC ${sourcefile})
#     target_include_directories(${name} PUBLIC ${TITANIC_INCLUDE_DIR})
#     target_link_libraries( ${name} Titan )
#     target_link_libraries( ${name} ${Boost_LIBRARIES} )
#     target_link_libraries( ${name} nlohmann_json::nlohmann_json)

# endforeach( sourcefile ${EXP_SOURCE_FILES} )

