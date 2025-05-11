cmake_minimum_required(VERSION 3.31)
project(Game C)

set(CMAKE_C_STANDARD 11)

# Najít SDL2 package
find_package(SDL2 REQUIRED)

# Přidat include directories pro SDL2
include_directories(${SDL2_INCLUDE_DIRS})

# Změnit z knihovny na spustitelný soubor
add_executable(Game main.c)

# Přilinkovat SDL2 knihovny
target_link_libraries(Game ${SDL2_LIBRARIES})
