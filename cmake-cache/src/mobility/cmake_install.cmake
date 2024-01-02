# Install script for directory: /home/5g-ns3-simulator/src/mobility

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "default")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-mobility-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-mobility-default.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-mobility-default.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/5g-ns3-simulator/build/lib/libns3.37-mobility-default.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-mobility-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-mobility-default.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-mobility-default.so"
         OLD_RPATH "/home/5g-ns3-simulator/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-mobility-default.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/5g-ns3-simulator/src/mobility/helper/group-mobility-helper.h"
    "/home/5g-ns3-simulator/src/mobility/helper/mobility-helper.h"
    "/home/5g-ns3-simulator/src/mobility/helper/ns2-mobility-helper.h"
    "/home/5g-ns3-simulator/src/mobility/model/box.h"
    "/home/5g-ns3-simulator/src/mobility/model/constant-acceleration-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/constant-position-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/constant-velocity-helper.h"
    "/home/5g-ns3-simulator/src/mobility/model/constant-velocity-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/gauss-markov-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/geographic-positions.h"
    "/home/5g-ns3-simulator/src/mobility/model/hierarchical-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/position-allocator.h"
    "/home/5g-ns3-simulator/src/mobility/model/random-direction-2d-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/random-walk-2d-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/random-waypoint-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/rectangle.h"
    "/home/5g-ns3-simulator/src/mobility/model/steady-state-random-waypoint-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/waypoint-mobility-model.h"
    "/home/5g-ns3-simulator/src/mobility/model/waypoint.h"
    "/home/5g-ns3-simulator/build/include/ns3/mobility-module.h"
    )
endif()

