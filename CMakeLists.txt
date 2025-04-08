cmake_minimum_required(VERSION 3.4...3.18)
project(MonteCarlo)

add_subdirectory(pybind11)
pybind11_add_module(MonteCarlo MCOptionPricer.cpp)