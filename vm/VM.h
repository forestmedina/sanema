//
// Created by fores on 11/1/2023.
//

#ifndef UPDATE_SKELETON_PY_VM_H
#define UPDATE_SKELETON_PY_VM_H

#include "ByteCode.h"
#include "ContextFrame.h"
#include <string>
#include <boost/numeric/conversion/cast.hpp>
#include <optional>
#include <types.h>
#include <functions.h>
#include "OperandType.h"

namespace sanema {
  using IPType = std::uint8_t const *;
  enum class ExecuteResult {
    OK,
    END,
    ERROR
  };
  enum class OperandTypeEnum : std::uint64_t {
    Boolean = 1,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    SINT8,
    SINT16,
    SINT32,
    SINT64,
    FLOAT,
    DOUBLE,
    STRING_REF
  };


  class BindingCollection;

  class VM {
  public:

    explicit VM(int memory_size_mb = 300);

    void run(ByteCode const &byte_code, BindingCollection &collection);

    template<class T>
    std::optional<T> get_value_stack() {
      if (operand_stack_pointer== nullptr) return {};
      return pop<T>();
    }

    void prepare_function_parameters(std::uint32_t n);

    [[nodiscard]] sanema::OperandType get_external_function_parameter(size_t index) const;

    std::string const &get_string(StringReference const &reference);

    ByteCode const *running_byte_code;

    void push_function_return(OperandType value);

    void push_string(std::string const &string_value);

    std::vector<sanema::ContextFrame> call_stack;
  private:
    std::vector<OperandType> external_function_parameters;
    std::array<OperandType,1000> operand_stack;
    OperandType* operand_stack_pointer{nullptr};

    std::vector<std::string> string_stack;
    std::vector<std::uint8_t> stack_memory;


    template<class type>
   inline type pop() {
      type t = (*(type*)operand_stack_pointer);
      operand_stack_pointer--;
      return t;
    }

    template<class type>
    inline type read_local(address_t address) {
      return *((type*)address.address);
    }


    template<class type>
    inline void push_local(IPType &ip) {
      auto address = pop<address_t>();

//      std::cout<<"Pushing address: "<<address<<" value:"<<value<<"\n";
      push(read_local<type>(address));
    }

    template<class type>
    inline void pop_to_local(IPType &ip) {
      auto address = read_from_bytecode<address_t>(ip);
      sanema::ContextFrame &context_frame = call_stack.back();
      auto value = pop<type>();
    }

    template<class type>
    inline void set_local(IPType &ip) {
      sanema::ContextFrame &context_frame = call_stack.back();
      auto value = pop<type>();
      auto address2 = pop<address_t>();
//      std::cout << "Setting local value=" << value << " address=" << address2 << "\n";
//      context_frame.write<type>(address2,
//                                value);
      *((type*)address2.address)=value;
    }


    template<class type>
    void push_const(IPType &ip) {
      auto value = read_from_bytecode<type>(ip);
//      std::cout << "constant value " << value << "\n";
      push(value);
    }

    template<class type>
    inline void push(type t) {
      operand_stack_pointer++;
      *((type*) operand_stack_pointer)= t;

    }

    inline void swap_last_two() {
      std::swap(*operand_stack_pointer,
                *(operand_stack_pointer-1));
    }

    template<typename type>
    inline void multiply() {
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      auto result = value1 * value2;
      push(result);
    }

    template<typename type>
    inline void add() {

       auto value2 = pop<type>();
      auto value1 = pop<type>();
      auto result = value1 + value2;
      // std::cout<<"adding "<<value1<<" + " <<value2<<"\n";
      push(result);

    }

    template<typename type>
    inline void subtract() {
     auto value2 = pop<type>();
      auto value1 = pop<type>();
      auto result = value1 - value2;
      push(result);
    }

    template<typename type>
    inline void divide() {
      auto value2 = pop_function_parameter_value<type>();
      auto value1 = pop_function_parameter_value<type>();
      auto result = value1 / value2;
      push(result);
    }

    template<typename type>
    inline void negate() {
      auto value = pop_function_parameter_value<type>();
      auto result = -value;
      push(result);
    }

    template<typename type>
    inline void greater() {
      auto value2 = pop_function_parameter_value<type>();
      auto value1 = pop_function_parameter_value<type>();
      bool result = value1 > value2;
      push(result);
    }

    template<typename type>
    inline void less() {
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      bool result = value1 < value2;
      push(result);
    }

    template<typename type>
    inline void equal() {
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      bool result = value1 == value2;
      push(result);
    }

    template<typename type>
    inline void greater_equal() {
      auto value2 = pop_function_parameter_value<type>();
      auto value1 = pop_function_parameter_value<type>();
      bool result = value1 >= value2;
      push(result);
    }

    template<class T>
    inline T pop_function_parameter_value(FunctionParameter::Modifier modifier = FunctionParameter::Modifier::VALUE) {
      switch (modifier) {
        case FunctionParameter::Modifier::VALUE:
          return pop<T>();
          break;
        case FunctionParameter::Modifier::MUTABLE:
        case FunctionParameter::Modifier::CONST:
          return read_local<T>(pop<address_t>());
          break;
      }
      throw std::runtime_error("We reached the end of pop_function_parameter_value without returning something");
      return T{};
    }

    template<typename type>
    inline void less_equal() {
      auto value2 = pop_function_parameter_value<type>();
      auto value1 = pop_function_parameter_value<type>();
      bool result = value1 <= value2;
      push(result);
    }
  };

  template<typename T>
  T get_function_parameter_from_vm(VM &vm, size_t index, sanema::FunctionParameter::Modifier modifier) {
    auto value = vm.get_external_function_parameter(index);
    T final_value;
    switch (modifier) {
      case sanema::FunctionParameter::Modifier::VALUE:
        final_value = static_cast<T>(value);
        break;
      case sanema::FunctionParameter::Modifier::MUTABLE:
      case sanema::FunctionParameter::Modifier::CONST:
        auto address = static_cast<address_t>(value);
        final_value = *((T*)address.address);
        break;
    }
    return final_value;
  }

  template<>
  std::string
  get_function_parameter_from_vm<std::string>(VM &vm, size_t index, sanema::FunctionParameter::Modifier modifier);

  template<>
  std::string const &get_function_parameter_from_vm<std::string const &>(VM &vm, size_t index,
                                                                         sanema::FunctionParameter::Modifier modifier);

  template<>
  std::string &
  get_function_parameter_from_vm<std::string &>(VM &vm, size_t index, sanema::FunctionParameter::Modifier modifier);

  template<typename T>
  void push_function_return_to_vm(VM &vm, T value) {
    vm.push_function_return(OperandType(value));
  }
  template<typename RETURN_TYPE, typename ...ARGS>
  RETURN_TYPE call_function(ByteCode const &byte_code, BindingCollection &collection,std::string identifier,ARGS... args){
  }

  template<>
  void push_function_return_to_vm<std::string>(VM &vm, std::string value);

  template<>
  void push_function_return_to_vm<std::string const &>(VM &vm, std::string const &value);

  template<>
  void push_function_return_to_vm<std::string &>(VM &vm, std::string &value);

  inline void VM::push_function_return(OperandType value) {
    push(value);
  }


}
#endif //UPDATE_SKELETON_PY_VM_H
