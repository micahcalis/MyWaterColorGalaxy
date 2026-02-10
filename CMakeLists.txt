cmake_minimum_required(VERSION 3.20)
project(MyWatercolorGalaxy)

# --- 1. Project Settings ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# [FIX] Force all outputs to "bin", regardless of Debug/Release config
# This prevents MSBuild/Visual Studio from creating a "Debug" subfolder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")

# --- 2. Find Dependencies ---
find_package(Vulkan REQUIRED)

# Explicitly find Slang Compiler
set(SLANGC_EXECUTABLE "C:/VulkanSDK/1.4.341.0/Bin/slangc.exe")
if(NOT EXISTS "${SLANGC_EXECUTABLE}")
    find_program(SLANGC_EXECUTABLE slangc REQUIRED)
endif()

# SDL2 Setup
set(SDL2_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include/SDL2")
set(SDL2_LIBRARY "$ENV{VULKAN_SDK}/Lib/SDL2.lib")

# --- 3. Define the Executable ---
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c")
add_executable(Game ${SOURCE_FILES})

# --- 4. Include & Link ---
target_include_directories(Game PRIVATE 
    "src"
    "${Vulkan_INCLUDE_DIRS}"
    "$ENV{VULKAN_SDK}/Include"
    "${SDL2_INCLUDE_DIR}"
)

target_link_libraries(Game PRIVATE 
    Vulkan::Vulkan 
    "${SDL2_LIBRARY}"
)

# --- 5. Shader Compilation (Smart Auto-Detection) ---

# 1. Create a "Master" target that will group all individual shaders
add_custom_target(Shaders)

function(add_slang_shader_target TARGET)
    cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})

    # Get the base name of the shader file (e.g., "HelloTriangle" from "HelloTriangle.slang")
    # We assume the first source file is the main one
    list(GET SHADER_SOURCES 0 MAIN_SOURCE)
    get_filename_component(SHADER_BASE_NAME ${MAIN_SOURCE} NAME_WE)

    # Set output paths (bin/assets/shaders)
    set(SHADERS_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders")
    
    # [FIX] Use the shader name for the output file, not hardcoded "shaders.spv"
    set(OUTPUT_SPV "${SHADERS_OUTPUT_DIR}/${SHADER_BASE_NAME}.spv")

    # --- AUTOMATIC ENTRY POINT DETECTION ---
    file(READ ${MAIN_SOURCE} SHADER_CONTENT)

    # 1. Find the Vertex Shader name
    string(REGEX MATCH "\\[shader\\(\\\"vertex\\\"\\)\\][ \t\r\n]*[A-Za-z0-9_]+[ \t\r\n]+([A-Za-z0-9_]+)" VERTEX_MATCH "${SHADER_CONTENT}")
    
    if(CMAKE_MATCH_1)
        set(VERT_ENTRY ${CMAKE_MATCH_1})
    else()
        message(FATAL_ERROR "Could not find [shader(\"vertex\")] in ${MAIN_SOURCE}")
    endif()

    # 2. Find the Fragment Shader name
    string(REGEX MATCH "\\[shader\\(\\\"fragment\\\"\\)\\][ \t\r\n]*[A-Za-z0-9_]+[ \t\r\n]+([A-Za-z0-9_]+)" FRAGMENT_MATCH "${SHADER_CONTENT}")
    
    if(CMAKE_MATCH_1)
        set(FRAG_ENTRY ${CMAKE_MATCH_1})
    else()
        message(FATAL_ERROR "Could not find [shader(\"fragment\")] in ${MAIN_SOURCE}")
    endif()

    # --- COMPILE COMMAND ---
    add_custom_command(
            OUTPUT  ${OUTPUT_SPV}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_OUTPUT_DIR}
            COMMAND ${SLANGC_EXECUTABLE} ${MAIN_SOURCE} 
                    -target spirv 
                    -profile spirv_1_4 
                    -emit-spirv-directly 
                    -fvk-use-entrypoint-name 
                    -entry ${VERT_ENTRY} 
                    -entry ${FRAG_ENTRY} 
                    -o ${OUTPUT_SPV}
            WORKING_DIRECTORY ${SHADERS_OUTPUT_DIR}
            DEPENDS ${SHADER_SOURCES}
            COMMENT "Compiling Slang Shader: ${SHADER_BASE_NAME}.slang -> ${SHADER_BASE_NAME}.spv"
            VERBATIM
    )
    
    add_custom_target(${TARGET} DEPENDS ${OUTPUT_SPV})
endfunction()

# 1. Find all .slang files
file(GLOB ALL_SHADERS "${CMAKE_SOURCE_DIR}/assets/shaders/*.slang")

# 2. Loop through each file
foreach(SHADER_FILE ${ALL_SHADERS})
    
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    set(TARGET_NAME "Shader_${SHADER_NAME}")

    # Call the function
    add_slang_shader_target(${TARGET_NAME} SOURCES ${SHADER_FILE})

    # [FIX] Link the individual shader target to the master "Shaders" target
    add_dependencies(Shaders ${TARGET_NAME})

endforeach()

# [FIX] Now "Shaders" actually exists, so Game can depend on it
add_dependencies(Game Shaders)


# --- 6. Final Polish (DLL Copy) ---
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$ENV{VULKAN_SDK}/Bin/SDL2.dll"
    "$<TARGET_FILE_DIR:Game>"
    COMMENT "Copying SDL2.dll to bin folder...")