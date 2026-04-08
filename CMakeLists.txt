# CMakeList.txt: Vulkan_ToyProject_Sea에 대한 CMake 프로젝트
cmake_minimum_required (VERSION 3.28)

# 지원되는 경우 MSVC 컴파일러에 대해 핫 다시 로드 사용하도록 설정합니다.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Vulkan_ToyProject_Sea")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")
option(ENABLE_CPP20_MODULE "Enable Vulkan C++20 Module" OFF)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(Stb REQUIRED)

# src/ 하위 소스 자동 수집
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.h"
)

# 이 프로젝트의 실행 파일에 소스를 추가합니다.
add_executable (Vulkan_ToyProject_Sea "Vulkan_ToyProject_Sea.cpp" "Vulkan_ToyProject_Sea.h" ${SRC_FILES})

# src/ 하위 include 경로 자동 수집
file(GLOB_RECURSE SRC_HEADERS "${CMAKE_CURRENT_LIST_DIR}/src/*.h")
set(SRC_INCLUDE_DIRS "")
foreach(HEADER ${SRC_HEADERS})
    get_filename_component(DIR ${HEADER} DIRECTORY)
    list(APPEND SRC_INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES SRC_INCLUDE_DIRS)

target_include_directories(Vulkan_ToyProject_Sea PRIVATE
    ${Stb_INCLUDE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/utils
    ${SRC_INCLUDE_DIRS}
)

target_compile_definitions(Vulkan_ToyProject_Sea PRIVATE
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
    SHADER_DIR="${CMAKE_CURRENT_LIST_DIR}/shader/"
)
target_link_libraries(Vulkan_ToyProject_Sea PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
    tinyobjloader::tinyobjloader
)

set_property(TARGET Vulkan_ToyProject_Sea PROPERTY CXX_STANDARD 20)

# PCH (precompiled header) 활성화
target_precompile_headers(Vulkan_ToyProject_Sea PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/Vulkan_ToyProject_Sea.h"
)

# Slang 셰이더 컴파일
find_program(SLANGC_EXECUTABLE slangc HINTS "$ENV{VULKAN_SDK}/bin")

function (add_slang_shader TARGET SOURCE OUTPUT_NAME)
  set (SHADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/shader)
  add_custom_command (
          OUTPUT ${SHADERS_DIR}
          COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
  )
  add_custom_command (
          OUTPUT  ${SHADERS_DIR}/${OUTPUT_NAME}
          COMMAND ${SLANGC_EXECUTABLE} ${SOURCE} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry main -o ${OUTPUT_NAME}
          WORKING_DIRECTORY ${SHADERS_DIR}
          DEPENDS ${SHADERS_DIR} ${SOURCE}
          COMMENT "Compiling ${OUTPUT_NAME}"
          VERBATIM
  )
  add_custom_target (${TARGET} DEPENDS ${SHADERS_DIR}/${OUTPUT_NAME})
endfunction()
