cmake_minimum_required(VERSION 3.16)
project(PhoneToolbox VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动包含当前目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# 查找libusb
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${LIBUSB_INCLUDE_DIRS}
)

# 创建源文件列表
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp"
)

# 创建头文件列表（仅用于IDE显示）
file(GLOB_RECURSE HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.h"
)

# 打印调试信息
message(STATUS "Found sources: ${SOURCES}")
message(STATUS "Found headers: ${HEADERS}")

# 资源文件
qt_add_resources(QRC_FILES resources/resources.qrc)

# 创建可执行文件
add_executable(PhoneToolbox ${SOURCES} ${HEADERS} ${QRC_FILES})

# 链接库
target_link_libraries(PhoneToolbox 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Network
    ${LIBUSB_LIBRARIES}
    pthread
)

# 设置属性
set_target_properties(PhoneToolbox PROPERTIES
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE FALSE
)

# Qt6 自动包含moc文件
set_target_properties(PhoneToolbox PROPERTIES
    AUTOMOC ON
)

# 安装规则（可选）
install(TARGETS PhoneToolbox DESTINATION bin)
