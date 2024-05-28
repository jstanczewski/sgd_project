cmake_minimum_required(VERSION 3.24)
project(sgd)

set(CMAKE_CXX_STANDARD 17)

set(SDL2_INCLUDE_DIR "C:\\SDL2-devel-2.30.1-VC\\SDL2-2.30.1\\include")
set(SDL2_LIB_DIR "C:\\SDL2-devel-2.30.1-VC\\SDL2-2.30.1\\lib\\x64")
set(SDL2_IMAGE_INCLUDE_DIR "C:\\SDL2-devel-2.30.1-VC\\SDL2_image-2.8.2\\include")
set(SDL2_IMAGE_LIB_DIR "C:\\SDL2-devel-2.30.1-VC\\SDL2_image-2.8.2\\lib\\x64")
set(SDL2_TTF_INCLUDE_DIR "C:\\SDL2-devel-2.30.1-VC\\SDL2_ttf-2.22.0\\include")
set(SDL2_TTF_LIB_DIR "C:\\SDL2-devel-2.30.1-VC\\SDL2_ttf-2.22.0\\lib\\x64")

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIB_DIR}/SDL2.lib ${SDL2_LIB_DIR}/SDL2main.lib ${SDL2_IMAGE_LIB_DIR}/SDL2_image.lib ${SDL2_TTF_LIB_DIR}/SDL2_ttf.lib)