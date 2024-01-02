# Install script for directory: /home/5g-ns3-simulator/contrib/bitrate-ctrl

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-bitrate-ctrl-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-bitrate-ctrl-default.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-bitrate-ctrl-default.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/5g-ns3-simulator/build/lib/libns3.37-bitrate-ctrl-default.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-bitrate-ctrl-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-bitrate-ctrl-default.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-bitrate-ctrl-default.so"
         OLD_RPATH "/home/5g-ns3-simulator/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-bitrate-ctrl-default.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/game-client.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/game-server.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/network-packet-header.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/network-packet.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/packet-receiver.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/packet-sender.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/packet-receiver-udp.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/packet-sender-udp.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/packet-receiver-tcp.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/packet-sender-tcp.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/video-decoder.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/video-encoder.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/fec-policy/packet-group.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/fec-policy/fec-policy.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/fec-policy/hairpin-policy.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/fec-policy/other-policy.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/fec-policy/fec-array.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/fec-policy/feconly-fec-array.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/fec-policy/webrtc-fec-array.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/congestion-control/rtc_base/checks.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/congestion-control/rtc_base/type_traits.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/congestion-control/rtc_base/numeric/safe_compare.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/congestion-control/rtc_base/numeric/safe_minmax.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/congestion-control/rtc_base/system/inline.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/congestion-control/sender-based-controller.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/congestion-control/nada-controller.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/congestion-control/gcc-controller.h"
    "/home/5g-ns3-simulator/contrib/bitrate-ctrl/model/utils.h"
    "/home/5g-ns3-simulator/build/include/ns3/bitrate-ctrl-module.h"
    )
endif()

