cmake_minimum_required(VERSION 3.17)
project(CheesEng C)

set(CMAKE_C_STANDARD 99)

add_executable(CheesEng coord.c move.c piece.c position.c main.c evaluation.c)