cmake_minimum_required(VERSION 3.16)

project(inventory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(inventory src/main.cpp
    src/item.h src/item.cpp
    src/inventory.h src/inventory.cpp
    src/player.h src/player.cpp
    src/game.h src/game.cpp
    src/charCheck.h
    src/random.h src/random.cpp
    src/shop.h src/shop.cpp
    src/gridbase.h src/gridbase.cpp
    src/equipment.h src/equipment.cpp
    src/displaylist.h src/displaylist.cpp
    src/music.h src/music.cpp
)
target_include_directories(inventory PRIVATE include)

include(GNUInstallDirs)
install(TARGETS inventory
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

