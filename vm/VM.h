//
// Created by fores on 11/1/2023.
//

#ifndef UPDATE_SKELETON_PY_VM_H
#define UPDATE_SKELETON_PY_VM_H

#include <format>

#include "ByteCode.h"
#include "ContextFrame.h"
#include <string>
#include <boost/numeric/conversion/cast.hpp>
#include <optional>
#include <types.h>
#include <functions.h>
#include <interfacing/Argument.h>
#include "OperandType.h"

namespace sanema {
  using IPType = VMInstruction const *;


  class BindingCollection;

  class VM {
  public:
    explicit VM(unsigned int memory_size_mb = 5);
    VM(VM const&)=delete;
    VM const& operator=(VM const&)=delete;
    VM(VM &&)=default;
    VM & operator=(VM &&)=default;

    ~VM()=default;


    void run(ByteCode const &byte_code, BindingCollection &collection,IPType initial_ip);
    void run(ByteCode const &byte_code, BindingCollection &collection);
    IPType setup_run(ByteCode const &byte_code, BindingCollection &collection,std::optional<FunctionID> define_function);
    void add_external_argument(Argument const &args);
    template<class T>
    std::optional<T> get_value_stack() {
  //      std::cout<<"operand_stack_pointer="<<(void*)operand_stack_pointer<<"\n";
//      std::cout<<"offset:"<<operand_stack_pointer-operand_stack<<"\n";
      if (operand_stack_pointer == nullptr) return {};
      return *((T *) operand_stack_pointer);
    }
    void* get_stack_pointer();

    template<typename T>
    [[nodiscard]] T get_external_function_parameter() {

      T value = *((T *) external_function_parameters_addresss);
      auto offset=external_function_parameters_addresss-operand_stack_pointer;
      external_function_parameters_addresss += sizeof(T);
//      std::cout<<"getting parameter offset:"<<offset<<"\n";
      return value;
    }

    std::string const &get_string(StringReference const &reference);




    void push_string(std::string const &string_value);

    template<typename T>
    void push_function_return(T value) {
      push_return(value);
    }

    template<class T>
    void push_argument(T value) {

    }

    std::vector<sanema::ContextFrame> call_stack;
    ByteCode const *running_byte_code;
  private:
    unsigned char *external_function_return_address{nullptr};
    unsigned char *external_function_parameters_addresss{nullptr};
    unsigned char *operand_stack;
    std::vector<unsigned char> operand_stack_vector;
    unsigned char *operand_stack_pointer{nullptr};
    unsigned char *next_argument_address{nullptr};
    std::vector<std::string> string_stack;
    std::vector<IPType> ip_history;

    template<class type>
    inline void save_result_register(VMInstruction const *instruction, type value) {

      type *pointer = instruction->is_rresult_reference ? *(type **) (operand_stack_pointer + instruction->r_result)
                                                        : (type *) (operand_stack_pointer + instruction->r_result);

      *(pointer) = value;
    }

    template<class type, int index>
    inline type read_register(VMInstruction const *instruction) {
      static_assert(index == 1 || index == 2,
                    "Index value must be 1 or 2");
      if constexpr (index == 1) {

        return instruction->is_r1_reference ?
               *(*((type **) (operand_stack_pointer + instruction->registers16.r1)))
                                            :
               *((type *) (operand_stack_pointer + instruction->registers16.r1));
      } else if constexpr (index == 2) {
        return instruction->is_r2_reference ?
               *(*((type **) (operand_stack_pointer + instruction->registers16.r2)))
                                            :
               *((type *) (operand_stack_pointer + instruction->registers16.r2));
      }
    }

    template<class type>
    inline type read_constant_pool(VMInstruction const *instruction, OperandType const *constants_pool_pointer) {

      return static_cast<type>(*(constants_pool_pointer + instruction->registers16.r1));

    }


    template<class type>
    inline type read_local(local_register_t address) {
      return *((type *) address.address);
    }


    template<class type>
    inline void push_local(VMInstruction const *instruction) {
//      std::cout<<"pushing local:"<<read_register<type, 1>(instruction)<<", to:"<<instruction->r_result<<"\n";
      save_result_register(instruction,
                           read_register<type, 1>(instruction));

    }


    template<class type>
    inline void set_local(IPType instruction) {
//      std::cout<<"setting local:"<<read_register<type, 1>(instruction)<<", to:"<<instruction->r_result<<"\n";
      type value = read_register<type, 1>(instruction);
      save_result_register(instruction,
                           read_register<type, 1>(instruction));
    }


    template<class type>
    void push_const(IPType instruction, OperandType const *const_pool_pointer) {
//      std::cout<<"pushing constant:"<<read_constant_pool<type>(instruction,
//                                                              const_pool_pointer)<<", to:"<<instruction->r_result<<"\n";
      save_result_register(instruction,
                           read_constant_pool<type>(instruction,
                                                    const_pool_pointer));
    }

    inline std::uint32_t get_operand_pointer_offset() {
      return (operand_stack_pointer - operand_stack);
    }

    template<class type>
    inline void push_return(type t) {
      *reinterpret_cast<type *>(external_function_return_address) = t;
    }


    template<typename type>
    inline void multiply(IPType instruction) {
//      std::cout<<"multiplying "<<read_register<type, 1>(instruction)<<" and "<<read_register<type, 2>(instruction)<<"\n";
//      std::cout<<"  ="<<(read_register<type, 1>(instruction) * read_register<type, 2>(instruction))<<"\n";
      save_result_register(instruction,
                           read_register<type, 1>(instruction) * read_register<type, 2>(instruction));
    }

    template<typename type>
    inline void add(VMInstruction const *instruction) {
      save_result_register(instruction,
                           read_register<type, 1>(instruction) + read_register<type, 2>(instruction));
    }

    template<typename type>
    inline void subtract(VMInstruction const *instruction) {
      save_result_register(instruction,
                           read_register<type, 1>(instruction) - read_register<type, 2>(instruction));

    }

    template<typename type>
    inline void divide(VMInstruction const *instruction) {
      save_result_register(instruction,
                           read_register<type, 1>(instruction) / read_register<type, 2>(instruction));
    }

    template<typename type>
    inline void negate(VMInstruction const *instruction) {
      save_result_register(instruction,
                           -read_register<type, 1>(instruction));
    }

    template<typename type>
    inline void greater(VMInstruction const *instruction) {
      save_result_register<bool>(instruction,
                                 read_register<type, 1>(instruction) > read_register<type, 2>(instruction));
    }

    template<typename type>
    inline void less(VMInstruction const *instruction) {
      save_result_register<bool>(instruction,
                                 read_register<type, 1>(instruction) < read_register<type, 2>(instruction));
    }

    template<typename type>
    inline void equal(VMInstruction const *instruction) {
      save_result_register<bool>(instruction,
                                 read_register<type, 1>(instruction) == read_register<type, 2>(instruction));
    }

    template<typename type>
    inline void greater_equal(VMInstruction const *instruction) {
      save_result_register<bool>(instruction,
                                 read_register<type, 1>(instruction) >= read_register<type, 2>(instruction));
    }


    template<typename type>
    inline void less_equal(VMInstruction const *instruction) {
      save_result_register<bool>(instruction,
                                 read_register<type, 1>(instruction) <= read_register<type, 2>(instruction));
    }
  };


  template<typename T>
  T get_function_parameter_from_vm(VM &vm, sanema::FunctionParameterCompleted::Modifier modifier) {
    T final_value;
    switch (modifier) {
      case sanema::FunctionParameterCompleted::Modifier::VALUE: {
        auto value = vm.get_external_function_parameter<T>();

        final_value = static_cast<T>(value);
      }
        break;
      case sanema::FunctionParameterCompleted::Modifier::MUTABLE:
      case sanema::FunctionParameterCompleted::Modifier::CONST:
        auto pointer = vm.get_external_function_parameter<T*>();;
        final_value = *(pointer);
        break;
    }
    return final_value;
  }

  template<>
  std::string
  get_function_parameter_from_vm<std::string>(VM &vm, sanema::FunctionParameterCompleted::Modifier modifier);

  template<>
  std::string const &get_function_parameter_from_vm<std::string const &>(VM &vm,
                                                                         sanema::FunctionParameterCompleted::Modifier modifier);



  template<typename T>
  void push_function_return_to_vm(VM &vm, T value) {
    vm.push_function_return(value);
  }

  template<typename RETURN_TYPE, typename... ARGS>
  RETURN_TYPE call_function(ByteCode const &byte_code, BindingCollection &collection, std::string identifier,
                            ARGS... args) {
  }

  template<>
  void push_function_return_to_vm<std::string>(VM &vm, std::string value);

  template<>
  void push_function_return_to_vm<std::string const &>(VM &vm, std::string const &value);

  template<>
  void push_function_return_to_vm<std::string &>(VM &vm, std::string &value);
}
#endif //UPDATE_SKELETON_PY_VM_H
