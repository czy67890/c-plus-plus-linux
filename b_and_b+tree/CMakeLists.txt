SET(btree btree.c)

#将btree编成动态库，链接到库中
#CMake中默认的编库方法为静态库

ADD_LIBRARY(b_and_b+tree_lib SHARED  ${btree})

ADD_COMPILE_OPTIONS(-fPIC)
#install 到相应的目录，记住一定要make install
INSTALL(TARGETS
b_and_b+tree_lib 
LIBRARY DESTINATION 
lib3) 