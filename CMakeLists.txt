cmake_minimum_required(VERSION 3.15)

project(final_project LANGUAGES CXX)

set(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src)
set(THIRD_PARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/external)

# copy data files to build directory
file(COPY "media/" DESTINATION "media")

#compile third party libraries
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glfw)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glad)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glm)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/imgui)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/stb)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/assimp)

# set src code
file(GLOB BASE_HDR ${SOURCE_PATH}/base/*.h)
file(GLOB BASE_SRC ${SOURCE_PATH}/base/*.cpp)
file(GLOB PROJECT_HDR ${SOURCE_PATH}/*.h)
file(GLOB PROJECT_SRC ${SOURCE_PATH}/*.cpp)

# generate program
add_executable(final_project ${BASE_HDR} ${BASE_SRC} ${PROJECT_HDR} ${PROJECT_SRC})

if(WIN32)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
elseif(UNIX)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Debug)
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)
endif()
endif()

# include third party headers
target_include_directories(
    final_project PUBLIC
    ${THIRD_PARTY_LIBRARY_PATH}/glfw/include
    ${THIRD_PARTY_LIBRARY_PATH}/glad/include
    ${THIRD_PARTY_LIBRARY_PATH}/glm
    ${THIRD_PARTY_LIBRARY_PATH}/imgui
    ${THIRD_PARTY_LIBRARY_PATH}/stb
    ${THIRD_PARTY_LIBRARY_PATH}/assimp
)

# link third party libraries
target_link_libraries(final_project PUBLIC glfw glad glm imgui stb assimp)