cmake_minimum_required(VERSION 3.26)
project(sanema)

set(CMAKE_CXX_STANDARD 23)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED  )

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()


add_library(sanema
        compiler/Backend.cpp
        compiler/Backend.h
        compiler/ByteCodeCompiler.cpp
        compiler/ByteCodeCompiler.h
        parsing/SanemaParser.cpp
        parsing/SanemaParser.hpp
        vm/ByteCode.cpp
        vm/ByteCode.h
        vm/ContextFrame.cpp
        vm/ContextFrame.h
        vm/opcodes.h
        vm/VM.cpp
        vm/VM.h
        binding/FieldBinding.cpp
        binding/FieldBinding.h
        binding/MethodBinding.cpp
        binding/MethodBinding.h
        binding/TypeBinding.cpp
        binding/TypeBinding.h
        binding/Value.h
        binding/Value.cpp
        types.cpp
        literals.cpp
        functions.cpp
        expressions.cpp
        built-in/built_in_functions.h
        common/FunctionCollection.cpp
        common/FunctionCollection.h
        built-in/built_in_functions.cpp
        test/test.cpp
        test/basic_tests.cpp
        test/helpers.cpp
        test/helpers.h
        built-in/strings/strings.h
        binding/BindingCollection.h
        binding/FunctionBinding.h
        binding/Value.cpp
        built-in/strings/strings.cpp
        SanemaScriptSystem.cpp
        SanemaScriptSystem.h
        SanemaScriptSystem.h
        ScriptID.cpp
        ScriptID.h
        ScriptEntry.cpp
        ScriptEntry.h
        SanemaScriptSystemImpl.cpp
        SanemaScriptSystemImpl.h
        SanemaScriptSystemImpl.h
        binding/BindingCollection.cpp
        binding/BindingCollection.cpp
        built-in/print.cpp
        built-in/print.h
        built-in/generators.h
        built-in/generators.h
        built-in/generators.cpp
        built-in/generators.cpp
        test/multiply_tests.cpp
        vm/OperandType.cpp
        vm/OperandType.h
        common/TypeCollection.cpp
        common/TypeCollection.h
        common/TypeCollection.h
        vm/VMInstruction.cpp
        vm/VMInstruction.h
        vm/VMInstruction.h
        interfacing/Argument.cpp
        interfacing/Argument.h)

target_include_directories(sanema PUBLIC  "./")

add_executable(sanema_main   sanema_main.cpp)
target_link_libraries (sanema_main PRIVATE sanema)



find_package(Catch2 3 REQUIRED)

find_package(Angelscript CONFIG REQUIRED)
target_link_libraries(sanema_main PRIVATE Angelscript::angelscript)
target_include_directories(sanema_main PRIVATE ${ANGELSCRIPT_INCLUDE_DIR})
find_package(Lua REQUIRED)
target_include_directories(sanema_main PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(sanema_main PRIVATE ${LUA_LIBRARIES})
# These tests need their own main
add_executable(sanema_tests test/test.cpp
        test/helpers.cpp
        test/helpers.h
        test/string_manitpulation.cpp
        test/multiply_tests.cpp
        test/divide_tests.cpp
        test/parameter_passing.cpp
        test/sanema_function_calling.cpp
        test/benchmarks.cpp
        test/execute_function_from_native.cpp
        test/chained_operators.cpp
        test/type_binding.cpp
        test/type_binding.cpp
)
target_link_libraries(sanema_tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries (sanema_tests PRIVATE sanema)

include(CTest)
include(Catch)
catch_discover_tests(sanema_tests)
