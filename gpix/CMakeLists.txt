cmake_minimum_required(VERSION 3.19)

project(test_gpix CXX)

file(GLOB_RECURSE SOURCES
    main.cxx
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
target_link_libraries(${PROJECT_NAME} PUBLIC
    ${GTKMM_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    ${GTKMM_INCLUDE_DIRS}
)
