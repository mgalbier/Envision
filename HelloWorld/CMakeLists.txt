cmake_minimum_required(VERSION 3.2.2)

project(HelloWorld)
include(../CMake/common.cmake)

add_library(HelloWorld SHARED
	src/HelloWorldPrinter.h
	src/helloworld_api.h
	src/precompiled.h
	src/HelloWorldPlugin.h
	src/HelloWorldPrinter.cpp
	src/HelloWorldPlugin.cpp
)

envision_plugin(HelloWorld)