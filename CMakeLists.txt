cmake_minimum_required(VERSION 3.26)
project(sanema)

set(CMAKE_CXX_STANDARD 20)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED  )

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(sanema
        compiler/Backend.cpp
        compiler/Backend.h
        compiler/ByteCodeCompiler.cpp
        compiler/ByteCodeCompiler.h
        parsing/SanemaParser.cpp
        parsing/SanemaParser.hpp
        parsing/ValidationStage.cpp
        parsing/ValidationStage.h
        vm/ByteCode.cpp
        vm/ByteCode.h
        vm/ContextFrame.cpp
        vm/ContextFrame.h
        vm/opcodes.h
        vm/VM.cpp
        vm/VM.h
        binding/FieldBinding.cpp
        binding/FieldBinding.h
        binding/FunctionBinding.cpp
        binding/FunctionBinding.h
        binding/TypeBinding.cpp
        binding/TypeBinding.h
        binding/Value.cpp
        binding/Value.h
        sanema_main.cpp
        types.cpp
        literals.cpp
        functions.cpp
        expressions.cpp
        built-in/built_in_functions.h
        parsing/FunctionCollection.cpp
        parsing/FunctionCollection.h
        built-in/built_in_functions.cpp)
target_include_directories(sanema PUBLIC  "./")