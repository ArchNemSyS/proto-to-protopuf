cmake_minimum_required(VERSION 3.5)

project(proto_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(proto_parser main.cpp
    parser.h parser.cpp
    )

include(GNUInstallDirs)
install(TARGETS proto_parser
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
