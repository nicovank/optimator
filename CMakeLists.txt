cmake_minimum_required(VERSION 3.16)

include(FetchContent)

project(Optimator)

set(CMAKE_CXX_STANDARD 20)

find_package(Clang REQUIRED CONFIG)

message("-- Using LLVM/Clang version ${LLVM_PACKAGE_VERSION}.")

add_executable(optimator src/main.cpp)

target_include_directories(optimator PUBLIC include)

target_link_libraries(optimator PRIVATE clangAST clangASTMatchers clangFrontend clangTooling)

target_include_directories(optimator SYSTEM PRIVATE ${CLANG_INCLUDE_DIRS})

if(MSVC)
  target_compile_options(optimator PRIVATE /W4 /WX)
else()
  target_compile_options(optimator PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
