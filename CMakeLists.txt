cmake_minimum_required(VERSION 3.16)
project(libroscpp)
include(cmake/common.cmake)
include(cmake/set_rpath.cmake)
include(cmake/super_build.cmake)
include(cmake/find_opencv_helper.cmake)
set(CMAKE_CXX_STANDARD  14)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/sanitizers")



set(ROS_DISTRO $ENV{ROS_DISTRO})
set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
message( LD_LIBRARY_PATH = ${LD_LIBRARY_PATH})
message( ROS_DISTRO = ${ROS_DISTRO})
set(ROS_INSTALL_PATH /opt/ros/${ROS_DISTRO})

build_external_project(absl lib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/abseil-cpp-20230802.1.tar.gz -DABSL_BUILD_TESTING=OFF -DABSL_USE_GOOGLETEST_HEAD=ON -DCMAKE_CXX_STANDARD=14  -DABSL_ENABLE_INSTALL=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON)
find_package(absl REQUIRED)

build_external_project(toml11 lib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toml11-3.8.1.tar.gz  -DCMAKE_CXX_STANDARD=11 -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=ON)
find_package(toml11 REQUIRED)

# CImg_latest.zip
#build_external_project(CImg lib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CImg_latest.zip -DCMAKE_CXX_STANDARD=11 -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=ON)
#find_package(CImg REQUIRED)

add_library(lodepng STATIC ${CMAKE_SOURCE_DIR}/include/lodepng/lodepng.cpp)
set_pic(lodepng)
target_include_directories(lodepng PUBLIC ${CMAKE_SOURCE_DIR}/include/lodepng/ )


#--enable-static
build_external_project_command(tinycc lib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinycc-mob-20240330.zip "./configure --prefix=${CMAKE_BINARY_DIR}/force_tinycc/install --disable-static " "make -j6" " make install" )
#build_external_project_command(tinycc lib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinycc-mob_2023_1220.zip "./configure --prefix=${CMAKE_BINARY_DIR}/force_tinycc/install --enable-static --debug --extra-cflags='-std=c11 -Wpedantic -fsanitize=address,undefined -g' " "make -j6" " make install" )

set(tinycc_COMPILER_DIR ${tinycc_ROOT}/lib/tcc)
set(tinycc_LIBTCC_INCLUDE_DIR ${tinycc_ROOT}/include)
set(tinycc_TCCLIB_INCLUDE_DIR ${tinycc_ROOT}/lib/tcc/include)
set(tinycc_LIB ${tinycc_ROOT}/lib/libtcc.so)


# Eigen3
build_external_project(Eigen3 lib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/eigen-3.4.0.zip  -DCMAKE_BUILD_TYPE=Release )
message(Eigen3_ROOT : ${Eigen3_ROOT} )

set(Eigen3_ROOT ${Eigen3_ROOT}/share/eigen3/cmake)
find_package(Eigen3 3.4.0 REQUIRED HINTS ${Eigen3_ROOT})
message(new eigen: EIGEN3_INCLUDE_DIRS : ${EIGEN3_INCLUDE_DIRS} )

# bit7z-4.0.7.tar.gz
#build_external_project(bit7z lib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bit7z-4.0.7.tar.gz  -DCMAKE_BUILD_TYPE=Release )
#message(STATUS "bit7z_BUILD_DIR " ${bit7z_BUILD_DIR})
##find_package(bit7z REQUIRED)
#set(bit7z_INCLUDE_DIR ${bit7z_BUILD_DIR}/bit7z/include/ )
#set(bit7z_LIBRARY ${bit7z_BUILD_DIR}/bit7z/lib/x64/libbit7z64.a )

build_external_project(unarr lib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/unarr-1.1.1.tar.xz  -DCMAKE_BUILD_TYPE=Release  -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON )
find_package(unarr REQUIRED)


add_library(uuid4 SHARED ${CMAKE_SOURCE_DIR}/include/uuid4/uuid4.c)
target_include_directories(uuid4 PUBLIC ${CMAKE_SOURCE_DIR}/include/uuid4 )


add_subdirectory(message_center)



add_subdirectory(tcc)



set(BUILD_DDS ON)
if(BUILD_DDS)
    add_subdirectory(fastdds)
endif ()

set(BUILD_ROS1 ON)
if (BUILD_ROS1)
    add_subdirectory(ros1)
endif ()


set(BUILD_MQTT ON)
if(BUILD_MQTT)
    add_subdirectory(mqtt)
endif ()


add_subdirectory(demo)
add_subdirectory(perception)


##### install

#install_external_project(absl)
install_external_project(toml11)
install_external_project(tinycc)

#set(shared_dirs ${absl_ROOT}/lib ${toml11_ROOT}/lib ${tinycc_ROOT}/lib)
#message(STATUS "install shared_dirs : ${shared_dirs}")
#foreach(dir ${shared_dirs})
#    if(EXISTS "${dir}" AND IS_DIRECTORY "${dir}")
#        message("EXISTS：${dir}")
#        #Installing roslaunch Files or Other Resources
#        install(DIRECTORY ${dir}/
#                DESTINATION lib
#                PATTERN ".svn" EXCLUDE)
#    endif()
#endforeach()

#https://cmake.org/cmake/help/latest/command/cmake_parse_arguments.html#command:cmake_parse_arguments
macro(my_install)
    set(options OPTIONAL FAST)
    set(oneValueArgs DESTINATION RENAME)
    set(multiValueArgs TARGETS CONFIGURATIONS)
    cmake_parse_arguments(MY_INSTALL "${options}" "${oneValueArgs}"
            "${multiValueArgs}" ${ARGN} )

    message(STATUS "MY_INSTALL_OPTIONAL: ${MY_INSTALL_OPTIONAL}")
    message(STATUS "MY_INSTALL_FAST: ${MY_INSTALL_FAST}")
    message(STATUS "MY_INSTALL_DESTINATION: ${MY_INSTALL_DESTINATION}")
    message(STATUS "MY_INSTALL_RENAME: ${MY_INSTALL_RENAME}")
    message(STATUS "MY_INSTALL_TARGETS: ${MY_INSTALL_TARGETS}")
    message(STATUS "MY_INSTALL_CONFIGURATIONS: ${MY_INSTALL_CONFIGURATIONS}")

    message(STATUS "MY_INSTALL_UNPARSED_ARGUMENTS: ${MY_INSTALL_UNPARSED_ARGUMENTS}")
    message(STATUS "MY_INSTALL_KEYWORDS_MISSING_VALUES: ${MY_INSTALL_KEYWORDS_MISSING_VALUES}")
    message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

endmacro()
my_install(TARGETS foo bar DESTINATION bin OPTIONAL blub CONFIGURATIONS a b c )