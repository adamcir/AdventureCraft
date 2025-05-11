cmake_minimum_required(VERSION 3.10)
project(Game)

# Najít SDL2 a SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(Game main.c)

# Přidat include adresáře
target_include_directories(Game PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
)

# Linkování knihoven
target_link_libraries(Game PRIVATE
        ${SDL2_LIBRARIES}
        SDL2_image
)
