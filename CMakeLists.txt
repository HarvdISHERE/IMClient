cmake_minimum_required(VERSION 3.19)
project(IMClient LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_resources(IMClient_RES
    resource.qrc
)

qt_add_executable(IMClient
    WIN32 MACOSX_BUNDLE
    main.cpp

    login_widget.h
    login_widget.ui
    ${IMClient_RES}
    proxy_setting.h proxy_setting.cpp proxy_setting.ui
    chat_widget.h chat_widget.cpp chat_widget.ui
    login_widget.cpp


)

target_link_libraries(IMClient
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

install(TARGETS IMClient
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET IMClient
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
