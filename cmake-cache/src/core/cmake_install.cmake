# Install script for directory: /home/5g-ns3-simulator/src/core

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-core-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-core-default.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-core-default.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/5g-ns3-simulator/build/lib/libns3.37-core-default.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-core-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-core-default.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.37-core-default.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/5g-ns3-simulator/src/core/model/int64x64-128.h"
    "/home/5g-ns3-simulator/src/core/helper/csv-reader.h"
    "/home/5g-ns3-simulator/src/core/helper/event-garbage-collector.h"
    "/home/5g-ns3-simulator/src/core/helper/random-variable-stream-helper.h"
    "/home/5g-ns3-simulator/src/core/model/abort.h"
    "/home/5g-ns3-simulator/src/core/model/ascii-file.h"
    "/home/5g-ns3-simulator/src/core/model/ascii-test.h"
    "/home/5g-ns3-simulator/src/core/model/assert.h"
    "/home/5g-ns3-simulator/src/core/model/attribute-accessor-helper.h"
    "/home/5g-ns3-simulator/src/core/model/attribute-construction-list.h"
    "/home/5g-ns3-simulator/src/core/model/attribute-container.h"
    "/home/5g-ns3-simulator/src/core/model/attribute-helper.h"
    "/home/5g-ns3-simulator/src/core/model/attribute.h"
    "/home/5g-ns3-simulator/src/core/model/boolean.h"
    "/home/5g-ns3-simulator/src/core/model/breakpoint.h"
    "/home/5g-ns3-simulator/src/core/model/build-profile.h"
    "/home/5g-ns3-simulator/src/core/model/calendar-scheduler.h"
    "/home/5g-ns3-simulator/src/core/model/callback.h"
    "/home/5g-ns3-simulator/src/core/model/command-line.h"
    "/home/5g-ns3-simulator/src/core/model/config.h"
    "/home/5g-ns3-simulator/src/core/model/default-deleter.h"
    "/home/5g-ns3-simulator/src/core/model/default-simulator-impl.h"
    "/home/5g-ns3-simulator/src/core/model/deprecated.h"
    "/home/5g-ns3-simulator/src/core/model/des-metrics.h"
    "/home/5g-ns3-simulator/src/core/model/double.h"
    "/home/5g-ns3-simulator/src/core/model/enum.h"
    "/home/5g-ns3-simulator/src/core/model/event-id.h"
    "/home/5g-ns3-simulator/src/core/model/event-impl.h"
    "/home/5g-ns3-simulator/src/core/model/fatal-error.h"
    "/home/5g-ns3-simulator/src/core/model/fatal-impl.h"
    "/home/5g-ns3-simulator/src/core/model/fd-reader.h"
    "/home/5g-ns3-simulator/src/core/model/global-value.h"
    "/home/5g-ns3-simulator/src/core/model/hash-fnv.h"
    "/home/5g-ns3-simulator/src/core/model/hash-function.h"
    "/home/5g-ns3-simulator/src/core/model/hash-murmur3.h"
    "/home/5g-ns3-simulator/src/core/model/hash.h"
    "/home/5g-ns3-simulator/src/core/model/heap-scheduler.h"
    "/home/5g-ns3-simulator/src/core/model/int-to-type.h"
    "/home/5g-ns3-simulator/src/core/model/int64x64-double.h"
    "/home/5g-ns3-simulator/src/core/model/int64x64.h"
    "/home/5g-ns3-simulator/src/core/model/integer.h"
    "/home/5g-ns3-simulator/src/core/model/length.h"
    "/home/5g-ns3-simulator/src/core/model/list-scheduler.h"
    "/home/5g-ns3-simulator/src/core/model/log-macros-disabled.h"
    "/home/5g-ns3-simulator/src/core/model/log-macros-enabled.h"
    "/home/5g-ns3-simulator/src/core/model/log.h"
    "/home/5g-ns3-simulator/src/core/model/make-event.h"
    "/home/5g-ns3-simulator/src/core/model/map-scheduler.h"
    "/home/5g-ns3-simulator/src/core/model/math.h"
    "/home/5g-ns3-simulator/src/core/model/names.h"
    "/home/5g-ns3-simulator/src/core/model/node-printer.h"
    "/home/5g-ns3-simulator/src/core/model/nstime.h"
    "/home/5g-ns3-simulator/src/core/model/object-base.h"
    "/home/5g-ns3-simulator/src/core/model/object-factory.h"
    "/home/5g-ns3-simulator/src/core/model/object-map.h"
    "/home/5g-ns3-simulator/src/core/model/object-ptr-container.h"
    "/home/5g-ns3-simulator/src/core/model/object-vector.h"
    "/home/5g-ns3-simulator/src/core/model/object.h"
    "/home/5g-ns3-simulator/src/core/model/pair.h"
    "/home/5g-ns3-simulator/src/core/model/pointer.h"
    "/home/5g-ns3-simulator/src/core/model/priority-queue-scheduler.h"
    "/home/5g-ns3-simulator/src/core/model/ptr.h"
    "/home/5g-ns3-simulator/src/core/model/random-variable-stream.h"
    "/home/5g-ns3-simulator/src/core/model/ref-count-base.h"
    "/home/5g-ns3-simulator/src/core/model/rng-seed-manager.h"
    "/home/5g-ns3-simulator/src/core/model/rng-stream.h"
    "/home/5g-ns3-simulator/src/core/model/scheduler.h"
    "/home/5g-ns3-simulator/src/core/model/show-progress.h"
    "/home/5g-ns3-simulator/src/core/model/simple-ref-count.h"
    "/home/5g-ns3-simulator/src/core/model/simulation-singleton.h"
    "/home/5g-ns3-simulator/src/core/model/simulator-impl.h"
    "/home/5g-ns3-simulator/src/core/model/simulator.h"
    "/home/5g-ns3-simulator/src/core/model/singleton.h"
    "/home/5g-ns3-simulator/src/core/model/string.h"
    "/home/5g-ns3-simulator/src/core/model/synchronizer.h"
    "/home/5g-ns3-simulator/src/core/model/system-path.h"
    "/home/5g-ns3-simulator/src/core/model/system-wall-clock-ms.h"
    "/home/5g-ns3-simulator/src/core/model/system-wall-clock-timestamp.h"
    "/home/5g-ns3-simulator/src/core/model/test.h"
    "/home/5g-ns3-simulator/src/core/model/time-printer.h"
    "/home/5g-ns3-simulator/src/core/model/timer-impl.h"
    "/home/5g-ns3-simulator/src/core/model/timer.h"
    "/home/5g-ns3-simulator/src/core/model/trace-source-accessor.h"
    "/home/5g-ns3-simulator/src/core/model/traced-callback.h"
    "/home/5g-ns3-simulator/src/core/model/traced-value.h"
    "/home/5g-ns3-simulator/src/core/model/trickle-timer.h"
    "/home/5g-ns3-simulator/src/core/model/tuple.h"
    "/home/5g-ns3-simulator/src/core/model/type-id.h"
    "/home/5g-ns3-simulator/src/core/model/type-name.h"
    "/home/5g-ns3-simulator/src/core/model/type-traits.h"
    "/home/5g-ns3-simulator/src/core/model/uinteger.h"
    "/home/5g-ns3-simulator/src/core/model/unused.h"
    "/home/5g-ns3-simulator/src/core/model/valgrind.h"
    "/home/5g-ns3-simulator/src/core/model/vector.h"
    "/home/5g-ns3-simulator/src/core/model/watchdog.h"
    "/home/5g-ns3-simulator/src/core/model/realtime-simulator-impl.h"
    "/home/5g-ns3-simulator/src/core/model/wall-clock-synchronizer.h"
    "/home/5g-ns3-simulator/build/include/ns3/config-store-config.h"
    "/home/5g-ns3-simulator/build/include/ns3/core-config.h"
    "/home/5g-ns3-simulator/build/include/ns3/core-module.h"
    )
endif()

