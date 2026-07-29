cmake_minimum_required(VERSION 3.29.2)

project(VulkanTest VERSION 1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
  message("This is a ${CMAKE_SYSTEM_NAME} system")
  add_definitions(-Dcmakemacro="Unix")
elseif(WIN32)
  message("This is windows")
  add_definitions(-Dcmakemacro="Windows")
endif()

set(DISPLAY_SERVER "windows" CACHE STRING "Display manager in linux: x11 or wayland")
message("Display Server is : ${DISPLAY_SERVER}")
add_definitions(-DDISPLAY_SERVER="${DISPLAY_SERVER}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)
set(
  SOURCES 
  ${SOURCE_DIR}/main.cpp
  ${SOURCE_DIR}/Khidki.cpp
  ${SOURCE_DIR}/Engine.cpp
) 

set(INC_DIR ${CMAKE_SOURCE_DIR}/include)
set(
  HEADER_FILES 
  ${INC_DIR}/Khidki.hpp
)

set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(
  SHADERS
  ${SHADERS_DIR}/simple.vert 
  ${SHADERS_DIR}/simple.frag
)


if(WIN32)
  set(
    HEADER_FILES 
    ${HEADER_FILES}
    ${INC_DIR}/GLFW/glfw3.h
    ${INC_DIR}/GLFW/glfw3native.h
  )
endif()

# can use this to pass config settings from cmake to source tree
# configure_file(${INC_DIR}/Config.h.in ${INC_DIR}/Config.h)


add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER_FILES})

# generate compile_commands.json or .ccls for ccls 
if(UNIX OR MINGW) # Others (MSVC) does not generate this json
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
  )
else()
  file(WRITE ${CMAKE_SOURCE_DIR}/.ccls "clang++\n-I${CMAKE_SOURCE_DIR}/include")
endif()


#Vulkan
if(WIN32)
  #GLFW: NOTE: find_package does not works for libraries that are not installed
  set(Vulkan_PATH "C:/VulkanSDK/1.4.350.0/")
  set(ENV{VULKAN_SDK} ${Vulkan_PATH})
  find_package(Vulkan REQUIRED COMPONENTS glslc)
  #set(Vulkan_INCLUDE_DIRS "${Vulkan_PATH}/include") #shouldnt be needed
  include_directories(${Vulkan_INCLUDE_DIRS})

  set(GLFW_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/GLFW")
  include_directories(${GLFW_INCLUDE_DIRS})

  if(MINGW)
    link_directories(${CMAKE_SOURCE_DIR}/library/minGW)
    set(GLFW_LIBRARIES "${CMAKE_SOURCE_DIR}/library/minGW/glfw3.lib")
  else()
    link_directories(${CMAKE_SOURCE_DIR}/library/msvc)
    set(GLFW_LIBRARIES "${CMAKE_SOURCE_DIR}/library/msvc/glfw3.lib")
  endif()
  # Put all libraries into a variable
  set(LIBS ${GLFW_LIBRARIES} ${Vulkan_LIBRARIES})
  # Put all includes into a variable
  set(INCS ${INC_DIR} ${GLFW_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
elseif(UNIX)
  find_package(Vulkan REQUIRED COMPONENTS glslc)
  find_package(glfw3 REQUIRED)
  set(LIBS glfw ${Vulkan_LIBRARIES})
  set(INCS ${INC_DIR} ${GLFW_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
  if(${DISPLAY_SERVER} MATCHES "wayland")
    find_library(LIB_WAYLAND_CURSOR
      NAMES libwayland-cursor libwayland-cursor.so wayland-cursor 
      )
      #find_library(LIB_DL
      #NAMES libdl libdl.so 
      #)
      set(LIBS ${LIBS} ${LIB_WAYLAND_CURSOR} ${CMAKE_DL_LIBS})
  elseif(${DISPLAY_SERVER} MATCHES "x11")
    find_package(x11 REQUIRED)
    set(LIBS ${LIBS} ${X11_LIBRARIES})
    set(INCS ${INCS} ${X11_INCLUDE_DIR})
  endif()
endif()

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCS})

## Compile shaders
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/shaders")
file(MAKE_DIRECTORY "${SHADER_OUTPUT_DIR}")
set(SPIRV_BINARY_FILES)
foreach(shader ${SHADERS})
    get_filename_component(FILE_NAME ${shader} NAME)
    set(SPIRV_FILE "${SHADER_OUTPUT_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT "${SPIRV_FILE}"
        COMMAND Vulkan::glslc
                "${shader}"
                -o "${SPIRV_FILE}"
        DEPENDS "${shader}"
        VERBATIM
    )
    list(APPEND SPIRV_BINARY_FILES "${SPIRV_FILE}")
endforeach()

add_custom_target(Shaders
    ALL
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(${PROJECT_NAME} Shaders)


# Copy Directories to bin
#add_custom_target(copy-runtime-files ALL
#    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/inventory ${CMAKE_BINARY_DIR}/bin/inventory
#    DEPENDS ${PROJECT_NAME})


# use following command

## windows MinGW
# mkdir build 
# cd build
# cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DDISPLAY_SERVER=windows ..
# cd bin
# cmake --build ..

## windows MVSC (default) use the `developer terminal` 
# mkdir build
# cd build
# cmake -G "Visual Studio 17 2022" -A x64 -DDISPLAY_SERVER=windows ..
# cmake --build . (notice no cd bin this time for some reason)

## linux wayland
# mkdir build
# cd build
# cmake -G "Unix Makefiles" -DDISPLAY_SERVER=wayland .. 
# cd bin
# cmake --build ..
