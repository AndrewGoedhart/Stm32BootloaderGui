cmake_minimum_required(VERSION 3.5)

project(BootloaderGui LANGUAGES CXX)
set(CMAKE_PREFIX_PATH "/home/andrew/Qt/5.12.12/gcc_64/lib/cmake")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS SerialPort REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)


if(ANDROID)
  add_library(BootloaderGui SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(BootloaderGui
    src/main.cpp
    src/mainwindow.cpp
    src/logView.cpp
    src/filelogger.cpp
    src/SerialSetup.cpp
    src/SerialSource.cpp
    src/softwareimage.cpp
    src/commands/command.cpp
    src/commands/commandstep.cpp
    src/commands/commandSequence.cpp
    src/mainwindow.ui
    src/logView.ui
    include/mainwindow.h
    include/filelogger.h
    include/logView.h
    include/SerialSetup.h
    include/SerialSource.h
    include/softwareimage.h
    include/commands/command.h
    include/commands/commandstep.h
    include/commands/commandSequence.h
    include/commands/RxPacket.h
    include/crcs/Crc.hpp
    include/crcs/Crc8.hpp
    include/crcs/Crc16.hpp
    include/crcs/Crc32.hpp
    include/typeConversion/Bits.hpp
    include/typeConversion/ConvertTypes.hpp
    include/typeConversion/Endian.hpp
  )
endif()

target_include_directories(BootloaderGui PUBLIC
    "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(BootloaderGui
    PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::SerialPort
 )
