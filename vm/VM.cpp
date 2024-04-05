//
// Created by fores on 11/1/2023.
//

#include "VM.h"
#include "opcodes.h"
#include <types.h>
#include <binding/BindingCollection.h>

//Generate function calls for each operation of the specified type
#define GENERATE_OPERATION(OP_TYPE, type, OPERATIONENUM, OPERATION_FUNCTION)  case OPCODE::OP_##OPERATIONENUM##OP_TYPE: {OPERATION_FUNCTION<type>();}break;
#define GENERATE_PUSH(OPTYPE, type)  case OPCODE::OP_PUSH_##OPTYPE##_CONST: {push_const<type>(ip);}break;

sanema::IPType  sanema::VM::setup_run(const sanema::ByteCode &byte_code, sanema::BindingCollection &collection,std::optional<FunctionID> function_id) {
  running_byte_code = &byte_code;
  operand_stack_pointer = operand_stack;
  external_function_return_address = operand_stack;
  external_function_parameters_addresss = operand_stack;
  next_argument_address = operand_stack_pointer ;
  string_stack.clear();
  call_stack.clear();
  IPType ip = byte_code.code_data.data();
  if(function_id.has_value()){
    auto final_function=byte_code.function_collection.get_function_by_id(function_id.value());

    if(final_function!= nullptr) {
      auto function_address = final_function->address;
      auto size=get_type_size(final_function->type);
      next_argument_address = operand_stack_pointer + size;
      ip = byte_code.code_data.data() + function_address;
    }
  }
  call_stack.emplace_back(operand_stack_pointer);
  auto end_address = byte_code.code_data.data() + byte_code.code_data.size();
  return ip;
}
void sanema::VM::add_external_argument(const sanema::Argument &arg) {
  match(arg.value,
        [this](auto value) {
          auto offset=sizeof (value);
          *((decltype(value)*)next_argument_address)=value;
          next_argument_address += offset;
        });
}
void sanema::VM::run(ByteCode const &byte_code, BindingCollection &binding_collection,IPType initial_ip) {
  IPType ip = initial_ip;

  ip_history.reserve(1000);
  ip_history.clear();
  bool should_continue = true;
  for (;;) {

    IPType instruction = ip;
    ip_history.emplace_back(ip);
    // std::cout << "Ip offset: " << (ip - byte_code.code_data.data()) << " ; ";
    // std::cout << "Executing opcode: " << opcode_to_string(instruction->opcode) << "\n";
    // std::cout << "  R32: " << instruction->register32.r1 << "\n";
    // std::cout << "  R1: " << instruction->registers16.r1 << "\n";
    // std::cout << "  R2: " << instruction->registers16.r2 << "\n";
    // std::cout << "  RESULT: " << instruction->r_result << "\n";


    ++ip;
    switch (instruction->opcode) {
      case OPCODE::OP_RESERVE_STACK_SPACE: {
        //   TODO we may need to implement this, so we can check that the stack does not growth out of bound

      }
        break;
      case OPCODE::OP_TRUE: {
      }
        break;
      case OPCODE::OP_FALSE: {
      }
        break;

      case OPCODE::OP_ADD_SINT64: {
        add<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_MULTIPLY_SINT64: {
        multiply<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_DIVIDE_SINT64: {
        divide<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_SUBTRACT_SINT64: {
        subtract<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_NEGATE_SINT64: {
        negate<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_SINT64: {
        greater<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_LESS_SINT64: {
        less<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_SINT64: {
        greater_equal<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_LESS_EQUAL_SINT64: {
        less_equal<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_EQUAL_SINT64: {
        equal<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_SINT64: {
        push_local<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_SINT64: {

      }
        break;
      case OPCODE::OP_SET_LOCAL_SINT64: {
        set_local<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_SINT64_CONST: {
        push_const<std::int64_t>(instruction,
                                 byte_code.const_pool.data());
      }
        break;
      case OPCODE::OP_ADD_SINT32: {
        add<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_MULTIPLY_SINT32: {
        multiply<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_DIVIDE_SINT32: {
        divide<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_SUBTRACT_SINT32: {
        subtract<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_NEGATE_SINT32: {
        negate<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_SINT32: {
        greater<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_LESS_SINT32: {
        less<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_SINT32: {
        greater_equal<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_LESS_EQUAL_SINT32: {
        less_equal<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_EQUAL_SINT32: {
        equal<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_SINT32: {
        push_local<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_SINT32: {

      }
        break;
      case OPCODE::OP_SET_LOCAL_SINT32: {
        set_local<std::int32_t>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_SINT32_CONST: {
        push_const<std::int32_t>(instruction,
                                 byte_code.const_pool.data());
      }
        break;
      case OPCODE::OP_ADD_SINT16: {
        add<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_MULTIPLY_SINT16: {
        multiply<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_DIVIDE_SINT16: {
        divide<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_SUBTRACT_SINT16: {
        subtract<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_NEGATE_SINT16: {
        negate<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_SINT16: {
        greater<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_LESS_SINT16: {
        less<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_SINT16: {
        greater_equal<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_LESS_EQUAL_SINT16: {
        less_equal<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_EQUAL_SINT16: {
        equal<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_SINT16: {
        push_local<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_SINT16: {
      }
        break;
      case OPCODE::OP_SET_LOCAL_SINT16: {
        set_local<std::int16_t>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_SINT16_CONST: {
        push_const<std::int16_t>(instruction,
                                 byte_code.const_pool.data());
      }
        break;
      case OPCODE::OP_ADD_SINT8: {
        add<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_MULTIPLY_SINT8: {
        multiply<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_DIVIDE_SINT8: {
        divide<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_SUBTRACT_SINT8: {
        subtract<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_NEGATE_SINT8: {
        negate<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_SINT8: {
        greater<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_LESS_SINT8: {
        less<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_SINT8: {
        greater_equal<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_LESS_EQUAL_SINT8: {
        less_equal<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_EQUAL_SINT8: {
        equal<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_SINT8: {
        push_local<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_SINT8: {
      }
        break;
      case OPCODE::OP_SET_LOCAL_SINT8: {
        set_local<std::int8_t>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_SINT8_CONST: {
        push_const<std::int8_t>(instruction,
                                byte_code.const_pool.data());
      }
        break;
      case OPCODE::OP_ADD_FLOAT: {
        add<float>(instruction);
      }
        break;
      case OPCODE::OP_MULTIPLY_FLOAT: {
        multiply<float>(instruction);
      }
        break;
      case OPCODE::OP_DIVIDE_FLOAT: {
        divide<float>(instruction);
      }
        break;
      case OPCODE::OP_SUBTRACT_FLOAT: {
        subtract<float>(instruction);
      }
        break;
      case OPCODE::OP_NEGATE_FLOAT: {
        negate<float>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_FLOAT: {
        greater<float>(instruction);
      }
        break;
      case OPCODE::OP_LESS_FLOAT: {
        less<float>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_FLOAT: {
        greater_equal<float>(instruction);
      }
        break;
      case OPCODE::OP_LESS_EQUAL_FLOAT: {
        less_equal<float>(instruction);
      }
        break;
      case OPCODE::OP_EQUAL_FLOAT: {
        equal<float>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_FLOAT: {
        push_local<float>(instruction);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_FLOAT: {
      }
        break;
      case OPCODE::OP_SET_LOCAL_FLOAT: {
        set_local<float>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_FLOAT_CONST: {
        push_const<float>(instruction,
                          byte_code.const_pool.data());
      }
        break;
      case OPCODE::OP_ADD_DOUBLE: {
        add<double>(instruction);
      }
        break;
      case OPCODE::OP_MULTIPLY_DOUBLE: {
        multiply<double>(instruction);
      }
        break;
      case OPCODE::OP_DIVIDE_DOUBLE: {
        divide<double>(instruction);
      }
        break;
      case OPCODE::OP_SUBTRACT_DOUBLE: {
        subtract<double>(instruction);
      }
        break;
      case OPCODE::OP_NEGATE_DOUBLE: {
        negate<double>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_DOUBLE: {
        greater<double>(instruction);
      }
        break;
      case OPCODE::OP_LESS_DOUBLE: {
        less<double>(instruction);
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_DOUBLE: {
        greater_equal<double>(instruction);
      }
        break;
      case OPCODE::OP_LESS_EQUAL_DOUBLE: {
        less_equal<double>(instruction);
      }
        break;
      case OPCODE::OP_EQUAL_DOUBLE: {
        equal<double>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_DOUBLE: {
        push_local<double>(instruction);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_DOUBLE: {
      }
        break;
      case OPCODE::OP_SET_LOCAL_DOUBLE: {
        set_local<double>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_DOUBLE_CONST: {
        push_const<double>(instruction,
                           byte_code.const_pool.data());
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_STRING: {
        push_local<sanema::StringReference>(instruction);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_STRING: {
      }
        break;
      case OPCODE::OP_SET_LOCAL_STRING: {
        set_local<sanema::StringReference>(instruction);
      }
        break;
      case OPCODE::OP_PUSH_STRING_CONST: {
        auto id_string = instruction->registers16.r1;
//        std::cout<<"pushing string:"<<StringReference(StringLocation::LiteralPool,
//                                             id_string)<<", to:"<<instruction->r_result<<"\n";
//        std::cout<<get_string(StringReference(StringLocation::LiteralPool,
//                                             id_string))<<"\n";
        save_result_register(instruction,
                             StringReference(StringLocation::LiteralPool,
                                             id_string));
      }
        break;
      case OPCODE::OP_JUMP: {
        auto offset = instruction->registers16.r1;
        ip += offset;
      }
        break;
      case OPCODE::OP_CALL_EXTERNAL_FUNCTION: {
        auto function_id = instruction->register32.r1;
        auto &function = binding_collection.get_function_by_id(function_id);
        external_function_return_address = operand_stack_pointer + instruction->r_result;
        external_function_parameters_addresss = operand_stack_pointer + instruction->r_result;
        function.call(*this);
      }
        break;
      case OPCODE::OP_CALL: {
        auto function_address = instruction->register32.r1;
        auto parameters_size = instruction->r_result;
        IPType new_ip = byte_code.code_data.data() + function_address;
        operand_stack_pointer += parameters_size;
        auto &last_call_stack = call_stack.back();
        last_call_stack.ip = ip;
        call_stack.emplace_back(operand_stack_pointer);
        ip = new_ip;
      }
        break;

      case OPCODE::OP_JUMP_IF_FALSE: {
        auto offset = instruction->registers16.r1;
        auto value = read_register<bool, 2>(instruction);
        if (!value) {
          ip += offset;
        }
      }
        break;
      case OPCODE::OP_RETURN: {
        should_continue = call_stack.size() > 1;
        call_stack.pop_back();
        if (should_continue) {
          ip = call_stack.back().ip;
          operand_stack_pointer = call_stack.back().get_begin_address();
        } else {
          return;
        }
        break;
      }
      case OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL: {
        auto local_address = instruction->registers16.r1;
        auto global_address = call_stack.back().get_begin_address() + local_address;
        save_result_register<std::uint8_t *>(instruction,
                                             global_address);
      }
        break;
      case OPCODE::OP_PUSH_EXTERNAL_FIELD_ADDRESS: {
        //TODO we need to reimplement external field access
//        auto field_id = pop<std::uint64_t>();
//        auto type_id = pop<std::uint64_t>();
//        auto object_address = pop<local_register_t>();
//        auto &type = binding_collection.get_type_by_id(type_id);
//        auto field_pointer = type.get_field_address((void *) object_address.address,
//                                                    field_id);
//        push(local_register_t{(std::uint8_t *) field_pointer});
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_BOOL: {
        push_local<bool>(instruction);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_BOOL: {

      }
        break;
      case OPCODE::OP_SET_LOCAL_BOOL: {
        set_local<std::int64_t>(instruction);
      }
        break;
      case OPCODE::OP_NOT: {

      }
        break;
    }
  }
}


sanema::VM::VM(unsigned int  memory_size_mb) : running_byte_code(nullptr) {
  auto megabytes_to_bytes = [](std::uint64_t size) { return (size * 1024) * 1024; };
  operand_stack_vector.resize(megabytes_to_bytes(memory_size_mb));
  operand_stack = operand_stack_vector.data();
  call_stack.reserve(1000);

}



std::string const &sanema::VM::get_string(const sanema::StringReference &reference) {
  switch (reference.location) {
    case StringLocation::LiteralPool:
      if (running_byte_code) {
        return running_byte_code->string_literals.at(reference.ref);
      }
      break;
    case StringLocation::LocalStack:
      return string_stack[reference.ref];
      break;
  }
  string_stack.emplace_back("");
  return string_stack[0];
}

void sanema::VM::push_string(std::string const &string_value) {
  string_stack.emplace_back(string_value);
  StringReference reference{StringLocation::LocalStack, boost::numeric_cast<std::uint32_t>(string_stack.size() - 1)};
  push_return(reference);
}

void sanema::VM::run(const sanema::ByteCode &byte_code, sanema::BindingCollection &collection) {
  run(byte_code, collection, setup_run(byte_code,collection,std::nullopt));
}




template<>
std::string sanema::get_function_parameter_from_vm<std::string>(VM &vm,
                                                                sanema::FunctionParameter::Modifier modifier) {
  auto reference = get_function_parameter_from_vm<StringReference>(vm,
                                                                   modifier);
  return vm.get_string(reference);
}

template<>
std::string const &sanema::get_function_parameter_from_vm<std::string const &>(VM &vm,
                                                                               sanema::FunctionParameter::Modifier modifier) {
  auto reference = get_function_parameter_from_vm<StringReference>(vm,
                                                                   modifier);
  return vm.get_string(reference);
}

template<>
void sanema::push_function_return_to_vm<std::string>(sanema::VM &vm, std::string value) {
  vm.push_string(value);
}

template<>
void sanema::push_function_return_to_vm<std::string const &>(sanema::VM &vm, std::string const &value) {
  vm.push_string(value);
}

template<>
void sanema::push_function_return_to_vm<std::string &>(sanema::VM &vm, std::string &value) {
  vm.push_string(value);
}
