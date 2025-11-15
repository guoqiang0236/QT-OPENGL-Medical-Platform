#设置cmake的最低版本要求
cmake_minimum_required(VERSION 3.20)

#设置c++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置项目名称和版本
project(QtGuiApp LANGUAGES CXX)

# 在 VS 工程中显示 assets 文件夹
file(GLOB ASSETS "./assets" )
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})
file(GLOB_RECURSE ASSET_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
message(STATUS "Assets folder path: ${CMAKE_CURRENT_SOURCE_DIR}/assets")
add_custom_target(AssetsInIDE SOURCES ${ASSET_FILES})
foreach(ASSET_FILE IN LISTS ASSET_FILES)
    file(RELATIVE_PATH ASSET_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${ASSET_FILE}")
    get_filename_component(ASSET_GROUP "${ASSET_REL_PATH}" DIRECTORY)
    string(REPLACE "/" "\\" ASSET_GROUP "${ASSET_GROUP}")
    source_group("${ASSET_GROUP}" FILES "${ASSET_FILE}")
endforeach()

# 查找所需要的库
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets Quick QuickWidgets)
set (GLM_DIR "${CMAKE_SOURCE_DIR}/Packages/glm-1.0.1-light")
set (OpenCV_DIR "${CMAKE_SOURCE_DIR}/Packages/OpenCV4100")
find_package(OpenCV REQUIRED)
set (ASSIMP_DIR "${CMAKE_SOURCE_DIR}/Packages/assimp")
set(DCMTK_ROOT ${CMAKE_SOURCE_DIR}/Packages/DCMTK3.6.9)
set(DCMTK_LIBS
    iphlpapi
    ws2_32
    wsock32
    netapi32
    ofstd
    oflog
    dcmdata
    dcmnet  
    dcmimage  
    dcmimgle 
    i2d   
)
# 启用 AUTOMOC（QT关键！）
set(CMAKE_AUTOMOC ON)   # 自动运行 moc，处理 Q_OBJECT 宏，生成元对象代码（信号槽机制依赖）
set(CMAKE_AUTOUIC ON)   # 自动处理 .ui 文件，生成对应的界面代码
set(CMAKE_AUTORCC ON)   # 自动处理 .qrc 资源文件，生成资源访问代码

# 收集源文件（显式列出头文件）
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "source/*.cpp"
    "source/*.h"
)

foreach(FILE_PATH IN LISTS SRC_FILES)
    file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${FILE_PATH}")
    get_filename_component(FILE_DIR "${REL_PATH}" DIRECTORY)
    source_group("${FILE_DIR}" FILES "${FILE_PATH}")
endforeach()

qt_add_resources(RESOURCES resources.qrc)




# 创建可执行文件，根据构建类型动态设置 WIN32 选项
#add_executable(QtGuiApp WIN32 ${SRC_FILES} ${RESOURCES} res/icon/apprc.rc)  # Release 模式：无控制台
add_executable(QtGuiApp ${SRC_FILES} ${RESOURCES}  res/icon/apprc.rc)

# 添加预编译头支持
target_precompile_headers(QtGuiApp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/pch.h")

# 库头文件目录
target_include_directories(QtGuiApp
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
        ${GLM_DIR}
        ${DCMTK_ROOT}/include
        ${ASSIMP_DIR}/include
)

# 库lib目录
target_link_directories(QtGuiApp
    PRIVATE
        "${CMAKE_SOURCE_DIR}/lib"
        ${OpenCV_LIB_DIR}
        ${DCMTK_ROOT}/lib
         ${ASSIMP_DIR}/lib
)

# 链接lib库
target_link_libraries(QtGuiApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::Quick
    Qt6::QuickWidgets
     ${OpenCV_LIBS}
     ${DCMTK_LIBS}
     assimp-vc143-mt.lib
     zlibstatic.lib
)

# 确保 MSVC 编译器以 UTF-8 方式读取源文件
if(MSVC)
    add_compile_options(/source-charset:utf-8)
endif()

# 复制 opencv_world*.dll 到可执行文件目录
add_custom_command(TARGET QtGuiApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${OpenCV_DIR}/x64/vc17/bin/opencv_world4100d.dll"
    "${OpenCV_DIR}/x64/vc17/bin/opencv_world4100.dll"
    $<TARGET_FILE_DIR:QtGuiApp>
)

# 复制 assimp 和 zlib 的 DLL 到可执行文件目录
add_custom_command(TARGET QtGuiApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ASSIMP_DIR}/bin/assimp-vc143-mt.dll"
        "${ASSIMP_DIR}/bin/assimp-vc143-mtd.dll"
        $<TARGET_FILE_DIR:QtGuiApp>
)
