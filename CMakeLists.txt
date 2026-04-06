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

# FetchContent_Declare(
#     SDL3
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#     GIT_TAG        main
# )
# set(SDL_TESTS OFF CACHE BOOL "" FORCE)
# set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
# set(SDL_SHARED OFF CACHE BOOL "" FORCE)
# set(SDL_STATIC ON CACHE BOOL "" FORCE)
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED)

# SDL3 Setup
set(SDL3_ROOT "C:/SDL3/SDL3-3.4.0")
set(SDL3_INCLUDE_DIR "${SDL3_ROOT}/include")
set(SDL3_LIBRARY "${SDL3_ROOT}/lib/x64/SDL3.lib")
set(SDL3_DLL "${SDL3_ROOT}/lib/x64/SDL3.dll")

if(NOT EXISTS "${SDL3_INCLUDE_DIR}")
    message(FATAL_ERROR "SDL3 Include not found at: ${SDL3_INCLUDE_DIR}")
endif()

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

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG        release
)

# FetchContent_Declare(
#     entt
#     GIT_REPOSITORY https://github.com/skypjack/entt.git
#     GIT_TAG        v3.16.0
# )

FetchContent_MakeAvailable(glm VMA tinyobjloader)

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

target_include_directories(${PROJECT_NAME} PRIVATE "src" "${SDL3_INCLUDE_DIR}")

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    "${SDL3_LIBRARY}"
    glm::glm
    tinyobjloader
    GPUOpen::VulkanMemoryAllocator
)

set(ASSETS_SRC "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_DST "${CMAKE_SOURCE_DIR}/bin/assets")

# 1. Write a mini CMake script to handle the smart copying
set(COPY_SCRIPT "${CMAKE_BINARY_DIR}/copy_assets.cmake")
file(WRITE "${COPY_SCRIPT}" 
    "file(COPY \"${ASSETS_SRC}/\" DESTINATION \"${ASSETS_DST}/\" PATTERN \"shaders\" EXCLUDE PATTERN \"*.ttf\" EXCLUDE)\n"
)

# 2. Run this script as an 'ALL' target (runs on every build automatically)
add_custom_target(SyncAssets ALL
    COMMAND ${CMAKE_COMMAND} -P "${COPY_SCRIPT}"
    COMMENT "Smart syncing assets (ignoring shader sources)..."
)

# --- YOU WERE MISSING THESE TWO LINES ---
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(SLANGC_EXECUTABLE slangc REQUIRED)
add_custom_target(Shaders) 
# ----------------------------------------

# 1. DEFINE YOUR MODULE DIRECTORY AND GRAB THE FILES
set(SLANG_MODULES_DIR "${CMAKE_SOURCE_DIR}/assets/shaders/modules")
file(GLOB_RECURSE SLANG_MODULE_FILES "${SLANG_MODULES_DIR}/*.slang")

function(add_slang_shader_target TARGET)
    cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})
    list(GET SHADER_SOURCES 0 MAIN_SOURCE)
    
    # --- NEW: Calculate the relative path to preserve your folder structure ---
    file(RELATIVE_PATH REL_SHADER_PATH "${CMAKE_SOURCE_DIR}/assets/shaders" "${MAIN_SOURCE}")
    get_filename_component(REL_DIR "${REL_SHADER_PATH}" DIRECTORY)
    get_filename_component(SHADER_BASE_NAME "${MAIN_SOURCE}" NAME_WE)

    # Output directory now dynamically includes 'graphics', 'compute', etc.
    set(SHADERS_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders/${REL_DIR}")
    
    set(OUTPUT_SPV "${SHADERS_OUTPUT_DIR}/${SHADER_BASE_NAME}.spv")
    set(OUTPUT_JSON "${SHADERS_OUTPUT_DIR}/${SHADER_BASE_NAME}.json") 
    set(COOKER_SCRIPT "${CMAKE_SOURCE_DIR}/tools/ShaderSettingsParser.py")

    add_custom_command(
        OUTPUT  ${OUTPUT_SPV} ${OUTPUT_JSON} 
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADERS_OUTPUT_DIR}"
        
        # RUN PYTHON FIRST: Generate the JSON
        COMMAND Python3::Interpreter "${COOKER_SCRIPT}" "${MAIN_SOURCE}" "${OUTPUT_JSON}"
        
        # RUN SLANGC SECOND: Generate the Bytecode
        COMMAND ${SLANGC_EXECUTABLE} "${MAIN_SOURCE}"
                -target spirv -fvk-use-entrypoint-name -profile spirv_1_4 -emit-spirv-directly 
                -matrix-layout-column-major
                -I "${SLANG_MODULES_DIR}" 
                -o "${OUTPUT_SPV}"
                -O3
                
        WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders"
        
        DEPENDS ${SHADER_SOURCES} ${COOKER_SCRIPT} ${SLANG_MODULE_FILES}
        COMMENT "Cooking & Compiling Shader: ${REL_SHADER_PATH}"
        VERBATIM
    )
    
    add_custom_target(${TARGET} DEPENDS ${OUTPUT_SPV} ${OUTPUT_JSON})
endfunction()

# 4. GLOB ALL SHADERS RECURSIVELY
file(GLOB_RECURSE ALL_RAW_SHADERS "${CMAKE_SOURCE_DIR}/assets/shaders/*.slang")

# 5. FILTER OUT THE MODULES FOLDER
set(ALL_SHADERS "")
foreach(SHADER_FILE ${ALL_RAW_SHADERS})
    # If the path DOES NOT contain "/modules/", add it to our build list
    string(FIND "${SHADER_FILE}" "/modules/" IS_MODULE)
    if(IS_MODULE EQUAL -1)
        list(APPEND ALL_SHADERS "${SHADER_FILE}")
    endif()
endforeach()

# 6. CREATE TARGETS
foreach(SHADER_FILE ${ALL_SHADERS})
    # Create a unique target name by replacing slashes with underscores
    # e.g. "graphics/mesh.slang" becomes "Shader_graphics_mesh"
    file(RELATIVE_PATH REL_SHADER_PATH "${CMAKE_SOURCE_DIR}/assets/shaders" "${SHADER_FILE}")
    string(REPLACE "/" "_" SAFE_TARGET_NAME "${REL_SHADER_PATH}")
    get_filename_component(SAFE_TARGET_NAME "${SAFE_TARGET_NAME}" NAME_WE)
    
    set(TARGET_NAME "Shader_${SAFE_TARGET_NAME}")
    add_slang_shader_target(${TARGET_NAME} SOURCES "${SHADER_FILE}")
    
    add_dependencies(Shaders ${TARGET_NAME})
endforeach()

# Finally, link the shader building to your main executable
add_dependencies(${PROJECT_NAME} Shaders)