cmake_minimum_required(VERSION 3.20)
project(test)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) #可以自动创建bin目录
set(SDL2_DIR "D:/SDL/SDL2-devel-2.30.0-mingw/SDL2-2.30.0/x86_64-w64-mingw32")
set (CMAKE_CXX_FLAGS "-Wall" )

aux_source_directory(. SRC_CUR)

# file(GLOB SRC_SUB src/*.cpp)
aux_source_directory(src SRC_SUB)

include_directories(${SDL2_DIR}/include/SDL2)
include_directories(include)

link_directories(${SDL2_DIR}/lib)
link_libraries(mingw32 SDL2main SDL2)

add_executable(test WIN32 ${SRC_CUR} ${SRC_SUB})
















# include_directories(include)
# file(GLOB SRC_CUR ./src/*.cpp)
# add_library(test STATIC ${SRC_CUR})

# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) #可以自动创建lib目录 全局变量动静态都行

# # 指定单个目标的输出目录 ARCHIVE_OUTPUT_DIRECTORY是静态库库文件的输出目录 LIBRARY_OUTPUT_DIRECTORY是动态库库文件的输出目录 RUNTIME_OUTPUT_DIRECTORY是可执行文件的输出目录
# # set_target_properties(test PROPERTIES LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) 

# # 静态库不能包含main.cpp
# include_directories(include)
# link_directories(lib)
# link_libraries(libtest.a)
# add_executable(app main.cpp)


