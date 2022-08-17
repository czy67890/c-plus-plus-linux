cmake_minimum_required(VERSION 2.8)
#工程名而不是执行文件名
PROJECT(CZY_Project)


#Set 用来设置变量名
SET(SRC_LIST main.cpp)

#AUX_SOURCE_DIRECTORY查找所有源文件
#上面的等价于 AUX_SOURCE_DIRECTORY(. DIR_LIB_SRCS)

#message用来打印信息
MESSAGE(STATUS "This IS Binary Dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This IS Source Dir" ${PROJECT_SOURCE_DIR})

#找头文件
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/inc")

#递归向下执行cmake编译
ADD_SUBDIRECTORY(b_and_b+tree)
ADD_COMPILE_OPTIONS(-fPIC)
#生成可执行二进制文件
ADD_EXECUTABLE(main ${SRC_LIST})

#将库文件链接到库
TARGET_LINK_LIBRARIES(main b_and_b+tree_lib)

#install 指定的目录或者文件到指定的目录
#默认安装到 /usr/local中



