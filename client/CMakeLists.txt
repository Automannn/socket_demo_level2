cmake_minimum_required(VERSION 3.15)
project(socket_demo_client)


add_executable(socket_demo_client ClientMainCpp.cpp)
link_directories(${PROJECT_SOURCE_DIR}/libs) #添加动态连接库的路径
target_link_libraries(socket_demo_client -lws2_32 ) #添加ws2_32.so