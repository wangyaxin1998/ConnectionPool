cmake_minimum_required(VERSION 3.0)
project(ConnectionPool)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
aux_source_directory(./src SRC_LIST)

#设置头文件搜索路径
include_directories("C:/Program Files/MySQL/MySQL Server 5.7/include" ./include )

# 将添加到库文件搜索路径
link_directories("C:/Program Files/MySQL/MySQL Server 5.7/lib")

#设置可执行文件最终存储的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#生成可执行文件
add_executable(main ${SRC_LIST} ./example/main.cpp)



#server这个目标程序，需要链接库文件
target_link_libraries(main libmysql)