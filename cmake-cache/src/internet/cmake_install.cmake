# Install script for directory: /home/5g-ns3-simulator/src/internet

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-internet-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-internet-default.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-internet-default.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/5g-ns3-simulator/build/lib/libns3.37-internet-default.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-internet-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-internet-default.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-internet-default.so"
         OLD_RPATH "/home/5g-ns3-simulator/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-internet-default.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/5g-ns3-simulator/src/internet/helper/internet-stack-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/internet-trace-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv4-address-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv4-global-routing-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv4-interface-container.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv4-list-routing-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv4-routing-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv4-static-routing-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv6-address-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv6-interface-container.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv6-list-routing-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv6-routing-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ipv6-static-routing-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/neighbor-cache-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/rip-helper.h"
    "/home/5g-ns3-simulator/src/internet/helper/ripng-helper.h"
    "/home/5g-ns3-simulator/src/internet/model/arp-cache.h"
    "/home/5g-ns3-simulator/src/internet/model/arp-header.h"
    "/home/5g-ns3-simulator/src/internet/model/arp-l3-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/arp-queue-disc-item.h"
    "/home/5g-ns3-simulator/src/internet/model/candidate-queue.h"
    "/home/5g-ns3-simulator/src/internet/model/global-route-manager-impl.h"
    "/home/5g-ns3-simulator/src/internet/model/global-route-manager.h"
    "/home/5g-ns3-simulator/src/internet/model/global-router-interface.h"
    "/home/5g-ns3-simulator/src/internet/model/icmpv4-l4-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/icmpv4.h"
    "/home/5g-ns3-simulator/src/internet/model/icmpv6-header.h"
    "/home/5g-ns3-simulator/src/internet/model/icmpv6-l4-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/ip-l4-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-address-generator.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-end-point-demux.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-end-point.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-global-routing.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-header.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-interface-address.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-interface.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-l3-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-list-routing.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-packet-filter.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-packet-info-tag.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-packet-probe.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-queue-disc-item.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-raw-socket-factory.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-raw-socket-impl.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-route.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-routing-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-routing-table-entry.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4-static-routing.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv4.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-address-generator.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-end-point-demux.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-end-point.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-extension-demux.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-extension-header.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-extension.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-header.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-interface-address.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-interface.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-l3-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-list-routing.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-option-header.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-option.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-packet-filter.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-packet-info-tag.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-packet-probe.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-pmtu-cache.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-queue-disc-item.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-raw-socket-factory.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-route.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-routing-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-routing-table-entry.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6-static-routing.h"
    "/home/5g-ns3-simulator/src/internet/model/ipv6.h"
    "/home/5g-ns3-simulator/src/internet/model/loopback-net-device.h"
    "/home/5g-ns3-simulator/src/internet/model/ndisc-cache.h"
    "/home/5g-ns3-simulator/src/internet/model/rip-header.h"
    "/home/5g-ns3-simulator/src/internet/model/rip.h"
    "/home/5g-ns3-simulator/src/internet/model/ripng-header.h"
    "/home/5g-ns3-simulator/src/internet/model/ripng.h"
    "/home/5g-ns3-simulator/src/internet/model/rtt-estimator.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-bbr.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-bic.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-congestion-ops.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-cubic.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-dctcp.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-header.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-highspeed.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-htcp.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-hybla.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-illinois.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-l4-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-ledbat.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-linux-reno.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-lp.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-option-rfc793.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-option-sack-permitted.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-option-sack.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-option-ts.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-option-winscale.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-option.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-prr-recovery.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-rate-ops.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-recovery-ops.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-rx-buffer.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-scalable.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-socket-base.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-socket-factory.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-socket-state.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-socket.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-tx-buffer.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-tx-item.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-vegas.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-veno.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-westwood.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-yeah.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-bbr-debug.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-copa.h"
    "/home/5g-ns3-simulator/src/internet/model/tcp-copa2.h"
    "/home/5g-ns3-simulator/src/internet/model/udp-header.h"
    "/home/5g-ns3-simulator/src/internet/model/udp-l4-protocol.h"
    "/home/5g-ns3-simulator/src/internet/model/udp-socket-factory.h"
    "/home/5g-ns3-simulator/src/internet/model/udp-socket.h"
    "/home/5g-ns3-simulator/src/internet/model/windowed-filter.h"
    "/home/5g-ns3-simulator/build/include/ns3/internet-module.h"
    )
endif()

