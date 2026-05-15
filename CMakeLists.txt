cmake_minimum_required(VERSION 3.16)
project(FlightBench VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# macOS: locate libc++ headers when CommandLineTools ships them inside the SDK
if(APPLE)
    execute_process(
        COMMAND xcrun --sdk macosx --show-sdk-path
        OUTPUT_VARIABLE MACOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(LIBCXX_INCLUDE "${MACOS_SDK_PATH}/usr/include/c++/v1")
    if(EXISTS "${LIBCXX_INCLUDE}")
        add_compile_options(-I${LIBCXX_INCLUDE})
    endif()
endif()

include_directories(include)

# ── Library targets (each subsystem as its own static lib) ──────────────────

add_library(fb_simulation
    src/simulation/SimEngine.cpp
)
target_include_directories(fb_simulation PUBLIC include)

add_library(fb_sensors
    src/sensors/AltitudeSensor.cpp
    src/sensors/AirspeedSensor.cpp
    src/sensors/HeadingSensor.cpp
)
target_include_directories(fb_sensors PUBLIC include)
target_link_libraries(fb_sensors PUBLIC fb_simulation)

add_library(fb_flight_computer
    src/flight_computer/FlightComputer.cpp
)
target_include_directories(fb_flight_computer PUBLIC include)

add_library(fb_telemetry
    src/telemetry/TelemetryLogger.cpp
)
target_include_directories(fb_telemetry PUBLIC include)
target_link_libraries(fb_telemetry PUBLIC fb_simulation)

add_library(fb_test_harness
    src/test_harness/TestRunner.cpp
    src/test_harness/ReportGenerator.cpp
)
target_include_directories(fb_test_harness PUBLIC include)
target_link_libraries(fb_test_harness PUBLIC fb_simulation fb_sensors fb_telemetry)

# ── Main executable ──────────────────────────────────────────────────────────

add_executable(flightbench src/main.cpp)
target_include_directories(flightbench PRIVATE include ${CMAKE_SOURCE_DIR})
target_link_libraries(flightbench PRIVATE
    fb_simulation
    fb_sensors
    fb_flight_computer
    fb_test_harness
    fb_telemetry
)

# ── Install ──────────────────────────────────────────────────────────────────

install(TARGETS flightbench DESTINATION bin)
