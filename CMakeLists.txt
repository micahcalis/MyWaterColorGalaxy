cmake_minimum_required(VERSION 3.20)
project(MyWatercolorGalaxy)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR)

if(LTO_SUPPORTED)
    message(STATUS "LTO/IPO is supported. Enabling for Release builds.")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
else()
    message(WARNING "LTO/IPO is not supported: ${LTO_ERROR}")
endif()

# Makes sure all configs go into the bin directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/bin")
endforeach()

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        main
)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        master
)

FetchContent_Declare(
    VMA
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(SDL3 glm VMA)

include_directories(external)

find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND) # Will use a bundled version if the sdk isn't present.
    if (WIN32)
        find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/vulkan)
        if (Vulkan_LIBRARY)
            set(Vulkan_FOUND ON)
            MESSAGE("Using bundled Vulkan Library")
        endif()
    endif()
endif()

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS ${Vulkan_LIBRARY})
endif()

# Dummy .cpp file handles the single file VMA_IMPL requirement
set(VMA_IMP_FILE "${CMAKE_BINARY_DIR}/vma_implementation.cpp")
if(NOT EXISTS "${VMA_IMP_FILE}")
    file(WRITE "${VMA_IMP_FILE}"
        "#define VMA_IMPLEMENTATION\n"
        "#include \"vk_mem_alloc.h\"\n")
endif()

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c")
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${VMA_IMP_FILE})

target_include_directories(${PROJECT_NAME} PRIVATE "src")

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    SDL3::SDL3
    glm::glm
    GPUOpen::VulkanMemoryAllocator
)

find_program(SLANGC_EXECUTABLE slangc REQUIRED)
add_custom_target(Shaders)

function(add_slang_shader_target TARGET)
    cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})
    list(GET SHADER_SOURCES 0 MAIN_SOURCE)
    get_filename_component(SHADER_BASE_NAME ${MAIN_SOURCE} NAME_WE)

    set(SHADERS_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders")
    set(OUTPUT_SPV "${SHADERS_OUTPUT_DIR}/${SHADER_BASE_NAME}.spv")

    add_custom_command(
        OUTPUT  ${OUTPUT_SPV}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_OUTPUT_DIR}
        COMMAND ${SLANGC_EXECUTABLE} ${MAIN_SOURCE}
                -target spirv -profile spirv_1_4 -emit-spirv-directly -o ${OUTPUT_SPV}
        WORKING_DIRECTORY ${SHADERS_OUTPUT_DIR}
        DEPENDS ${SHADER_SOURCES}
        COMMENT "Compiling Slang Shader: ${SHADER_BASE_NAME}.slang"
        VERBATIM
    )
    add_custom_target(${TARGET} DEPENDS ${OUTPUT_SPV})
endfunction()

file(GLOB ALL_SHADERS "${CMAKE_SOURCE_DIR}/assets/shaders/*.slang")
foreach(SHADER_FILE ${ALL_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    set(TARGET_NAME "Shader_${SHADER_NAME}")
    add_slang_shader_target(${TARGET_NAME} SOURCES ${SHADER_FILE})
    add_dependencies(Shaders ${TARGET_NAME})
endforeach()

add_dependencies(${PROJECT_NAME} Shaders)
