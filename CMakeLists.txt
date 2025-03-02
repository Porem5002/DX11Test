cmake_minimum_required(VERSION 3.14)

project(DX11Test)

set(LIBS "d3d11.lib" "dxguid.lib" "d3dcompiler.lib")
file(GLOB CS "*.c")
file(GLOB HS "include/*.h")

add_executable(DX11Test ${CS} ${HS})
target_link_libraries(DX11Test ${LIBS})