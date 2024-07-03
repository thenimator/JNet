cmake_minimum_required(VERSION 3.7.2)

project(JNet)



file(GLOB sources CONFIGURE_DEPENDS src/*.hpp src/*.cpp src/*/*.cpp src/*/*.hpp)


if(!WIN32)
    add_executable(JNet ${sources})
else()
    add_executable(JNet WIN32 ${sources})
endif()


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)




# ------------------ boost asio ------------------

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.54.0 REQUIRED system)
#find_package(Boost 1.50 REQUIRED COMPONENTS asio)
message(STATUS "Boost version: ${Boost_VERSION}")

# This is needed if your Boost version is newer than your CMake version
# or if you have an old version of CMake (<3.5)
#if(NOT TARGET Boost::asio)
#    add_library(Boost::asio IMPORTED INTERFACE)
#    set_property(TARGET Boost::asio PROPERTY
#        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
#    set_property(TARGET Boost::asio PROPERTY
#        INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
#endif()FINDBOO




# ------------------ link libraries ------------------



target_include_directories(JNet PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(JNet ${Boost_LIBRARIES})