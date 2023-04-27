cmake_minimum_required(VERSION 3.0.2)
project(serial_COM)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  serial
)




catkin_package(

)


include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)




# 添加可执行文件
add_executable(main src/main.cpp)

# 将serialport库链接到可执行文件
target_link_libraries(main ${catkin_LIBRARIES})

# 添加可执行文件
add_executable(test_ src/test_1.cpp)

# 将serialport库链接到可执行文件
target_link_libraries(test_ ${catkin_LIBRARIES})
