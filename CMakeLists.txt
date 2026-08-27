cmake_minimum_required(VERSION 3.19)

project(IM客户端 LANGUAGES CXX)

# 查找 Qt6.5 必需的组件
find_package(Qt6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()


# =========================
# SQLite
# =========================

# 将 SQLite 源代码编译为静态库
add_library(sqlite3 STATIC
    sqlite/sqlite3.c
)

# 指定 SQLite 头文件所在目录
target_include_directories(sqlite3 PUBLIC
    sqlite
)


# =========================
# Qt 资源
# =========================

qt_add_resources(IMClient_RES
    资源.qrc
)


# =========================
# IM客户端主程序
# =========================

qt_add_executable(IM客户端
    WIN32
    MACOSX_BUNDLE

    main.cpp

    login_widget.h
    login_widget.ui
    ${IMClient_RES}

    proxy_setting.h
    proxy_setting.cpp
    proxy_setting.ui

    chat_widget.h
    chat_widget.cpp
    chat_widget.ui

    login_widget.cpp
)


# =========================
# 链接库
# =========================

target_link_libraries(IM客户端
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        sqlite3
)


# =========================
# 安装
# =========================

install(TARGETS IM客户端
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


# =========================
# Qt 部署脚本
# =========================

qt_generate_deploy_app_script(
    TARGET IM客户端
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
