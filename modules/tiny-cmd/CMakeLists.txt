cmake_minimum_required(VERSION 3.15)
project(cmd LANGUAGES C VERSION 1.0.0 DESCRIPTION "Modular and customizable command prompt.")

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 99)

file(GLOB SOURCES src/*.c)
file(GLOB INCLUDES include/cmd/*.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION include/${PROJECT_NAME})

install(FILES ${INCLUDES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

add_executable(main main.c)

target_link_libraries(main ${PROJECT_NAME})

