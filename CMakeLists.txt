cmake_minimum_required(VERSION 3.10)
project(AdventureCraft)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
# Najít SDL2 a SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)


add_executable(AdventureCraft main.c
        games.h
        games.c)

# Přidat include adresáře
target_include_directories(AdventureCraft PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}

)

# Linkování knihoven
target_link_libraries(AdventureCraft PRIVATE
        SDL2
        SDL2_image
        SDL2_ttf

)

file(COPY ${CMAKE_SOURCE_DIR}/textures DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/fonts DESTINATION ${CMAKE_BINARY_DIR})
