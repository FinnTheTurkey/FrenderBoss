cmake_minimum_required(VERSION 3.0.0)
project(FrenderPlayground VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
# set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(FrenderBoss main.cpp GLTFLoader.cc Player.cc Projectile.cc BallPhysics.cc)

add_subdirectory(Frender)
target_link_libraries(FrenderBoss Frender)
target_include_directories(FrenderBoss PUBLIC .)

add_compile_definitions(GLM_FORCE_CTOR_INIT)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(assimp)
target_link_libraries(FrenderBoss assimp)

# install(TARGETS assimp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
