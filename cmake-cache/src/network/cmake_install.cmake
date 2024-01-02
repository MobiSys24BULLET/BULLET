# Install script for directory: /home/5g-ns3-simulator/src/network

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-network-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-network-default.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-network-default.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/5g-ns3-simulator/build/lib/libns3.37-network-default.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-network-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-network-default.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-network-default.so"
         OLD_RPATH "/home/5g-ns3-simulator/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-network-default.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/5g-ns3-simulator/src/network/helper/application-container.h"
    "/home/5g-ns3-simulator/src/network/helper/delay-jitter-estimation.h"
    "/home/5g-ns3-simulator/src/network/helper/net-device-container.h"
    "/home/5g-ns3-simulator/src/network/helper/node-container.h"
    "/home/5g-ns3-simulator/src/network/helper/packet-socket-helper.h"
    "/home/5g-ns3-simulator/src/network/helper/simple-net-device-helper.h"
    "/home/5g-ns3-simulator/src/network/helper/trace-helper.h"
    "/home/5g-ns3-simulator/src/network/model/address.h"
    "/home/5g-ns3-simulator/src/network/model/application.h"
    "/home/5g-ns3-simulator/src/network/model/buffer.h"
    "/home/5g-ns3-simulator/src/network/model/byte-tag-list.h"
    "/home/5g-ns3-simulator/src/network/model/channel-list.h"
    "/home/5g-ns3-simulator/src/network/model/channel.h"
    "/home/5g-ns3-simulator/src/network/model/chunk.h"
    "/home/5g-ns3-simulator/src/network/model/header.h"
    "/home/5g-ns3-simulator/src/network/model/net-device.h"
    "/home/5g-ns3-simulator/src/network/model/nix-vector.h"
    "/home/5g-ns3-simulator/src/network/model/node-list.h"
    "/home/5g-ns3-simulator/src/network/model/node.h"
    "/home/5g-ns3-simulator/src/network/model/packet-metadata.h"
    "/home/5g-ns3-simulator/src/network/model/packet-tag-list.h"
    "/home/5g-ns3-simulator/src/network/model/packet.h"
    "/home/5g-ns3-simulator/src/network/model/socket-factory.h"
    "/home/5g-ns3-simulator/src/network/model/socket.h"
    "/home/5g-ns3-simulator/src/network/model/tag-buffer.h"
    "/home/5g-ns3-simulator/src/network/model/tag.h"
    "/home/5g-ns3-simulator/src/network/model/trailer.h"
    "/home/5g-ns3-simulator/src/network/test/header-serialization-test.h"
    "/home/5g-ns3-simulator/src/network/utils/address-utils.h"
    "/home/5g-ns3-simulator/src/network/utils/bit-deserializer.h"
    "/home/5g-ns3-simulator/src/network/utils/bit-serializer.h"
    "/home/5g-ns3-simulator/src/network/utils/crc32.h"
    "/home/5g-ns3-simulator/src/network/utils/data-rate.h"
    "/home/5g-ns3-simulator/src/network/utils/drop-tail-queue.h"
    "/home/5g-ns3-simulator/src/network/utils/dynamic-queue-limits.h"
    "/home/5g-ns3-simulator/src/network/utils/error-channel.h"
    "/home/5g-ns3-simulator/src/network/utils/error-model.h"
    "/home/5g-ns3-simulator/src/network/utils/ethernet-header.h"
    "/home/5g-ns3-simulator/src/network/utils/ethernet-trailer.h"
    "/home/5g-ns3-simulator/src/network/utils/flow-id-tag.h"
    "/home/5g-ns3-simulator/src/network/utils/generic-phy.h"
    "/home/5g-ns3-simulator/src/network/utils/inet-socket-address.h"
    "/home/5g-ns3-simulator/src/network/utils/inet6-socket-address.h"
    "/home/5g-ns3-simulator/src/network/utils/ipv4-address.h"
    "/home/5g-ns3-simulator/src/network/utils/ipv6-address.h"
    "/home/5g-ns3-simulator/src/network/utils/llc-snap-header.h"
    "/home/5g-ns3-simulator/src/network/utils/lollipop-counter.h"
    "/home/5g-ns3-simulator/src/network/utils/mac16-address.h"
    "/home/5g-ns3-simulator/src/network/utils/mac48-address.h"
    "/home/5g-ns3-simulator/src/network/utils/mac64-address.h"
    "/home/5g-ns3-simulator/src/network/utils/mac8-address.h"
    "/home/5g-ns3-simulator/src/network/utils/net-device-queue-interface.h"
    "/home/5g-ns3-simulator/src/network/utils/output-stream-wrapper.h"
    "/home/5g-ns3-simulator/src/network/utils/packet-burst.h"
    "/home/5g-ns3-simulator/src/network/utils/packet-data-calculators.h"
    "/home/5g-ns3-simulator/src/network/utils/packet-probe.h"
    "/home/5g-ns3-simulator/src/network/utils/packet-socket-address.h"
    "/home/5g-ns3-simulator/src/network/utils/packet-socket-client.h"
    "/home/5g-ns3-simulator/src/network/utils/packet-socket-factory.h"
    "/home/5g-ns3-simulator/src/network/utils/packet-socket-server.h"
    "/home/5g-ns3-simulator/src/network/utils/packet-socket.h"
    "/home/5g-ns3-simulator/src/network/utils/packetbb.h"
    "/home/5g-ns3-simulator/src/network/utils/pcap-file-wrapper.h"
    "/home/5g-ns3-simulator/src/network/utils/pcap-file.h"
    "/home/5g-ns3-simulator/src/network/utils/pcap-test.h"
    "/home/5g-ns3-simulator/src/network/utils/queue-fwd.h"
    "/home/5g-ns3-simulator/src/network/utils/queue-item.h"
    "/home/5g-ns3-simulator/src/network/utils/queue-limits.h"
    "/home/5g-ns3-simulator/src/network/utils/queue-size.h"
    "/home/5g-ns3-simulator/src/network/utils/queue.h"
    "/home/5g-ns3-simulator/src/network/utils/radiotap-header.h"
    "/home/5g-ns3-simulator/src/network/utils/sequence-number.h"
    "/home/5g-ns3-simulator/src/network/utils/simple-channel.h"
    "/home/5g-ns3-simulator/src/network/utils/simple-net-device.h"
    "/home/5g-ns3-simulator/src/network/utils/sll-header.h"
    "/home/5g-ns3-simulator/build/include/ns3/network-module.h"
    )
endif()

