cmake_minimum_required(VERSION 3.15)
project(ChatroomServer)

# 使用 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 Boost 路径（M 系 Mac 默认安装路径）
set(BOOST_ROOT "/opt/homebrew")
find_package(Boost REQUIRED COMPONENTS system thread)

# 包含头文件路径
include_directories(
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/server/headers
)

# 设置源码目录
file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/server/sources/*.cpp"
)

# 生成可执行文件
add_executable(chat_server ${SERVER_SOURCES})

# 链接 Boost 依赖库
target_link_libraries(chat_server ${Boost_LIBRARIES} pthread)
