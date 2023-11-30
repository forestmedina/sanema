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

void sanema::VM::run(ByteCode const &byte_code, BindingCollection &binding_collection) {
  running_byte_code = &byte_code;
  operand_stack_pointer=operand_stack;
  IPType ip = byte_code.code_data.data();
  call_stack.emplace_back(stack_memory.data());
  auto end_address = byte_code.code_data.data() + byte_code.code_data.size();
  bool should_continue = true;
  for (;;) {
    // std::cout << "Ip offset: " << (ip - byte_code.code_data.data()) << " ; ";
    auto opcode = static_cast<OPCODE>(*ip);
    // std::cout << "Executing opcode: " << opcode_to_string(opcode) << "\n";
    ++ip;
    switch (opcode) {
      case OPCODE::OP_POP: {
        pop<uint64_t>();
      }
        break;
      case OPCODE::OP_RESERVE_STACK_SPACE: {
        auto size = read_from_bytecode<std::uint64_t>(ip);
        sanema::ContextFrame &context_frame = call_stack.back();
//        std::cout << "reserving space =" << size;
        context_frame.reserve(size);
      }
        break;
      case OPCODE::OP_TRUE: {
        push<bool>(true);
      }
        break;
      case OPCODE::OP_FALSE: {
        push<bool>(false);
      }
        break;

      case OPCODE::OP_ADD_SINT64: {
        add<std::int64_t>();
      }
        break;
      case OPCODE::OP_MULTIPLY_SINT64: {
        multiply<std::int64_t>();
      }
        break;
      case OPCODE::OP_DIVIDE_SINT64: {
        divide<std::int64_t>();
      }
        break;
      case OPCODE::OP_SUBTRACT_SINT64: {
        subtract<std::int64_t>();
      }
        break;
      case OPCODE::OP_NEGATE_SINT64: {
        negate<std::int64_t>();
      }
        break;
      case OPCODE::OP_GREATER_SINT64: {
        greater<std::int64_t>();
      }
        break;
      case OPCODE::OP_LESS_SINT64: {
        less<std::int64_t>();
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_SINT64: {
        greater_equal<std::int64_t>();
      }
        break;
      case OPCODE::OP_LESS_EQUAL_SINT64: {
        less_equal<std::int64_t>();
      }
        break;
      case OPCODE::OP_EQUAL_SINT64: {
        equal<std::int64_t>();
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_SINT64: {
        push_local<std::int64_t>(ip);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_SINT64: {
        pop_to_local<std::int64_t>(ip);
      }
        break;
      case OPCODE::OP_SET_LOCAL_SINT64: {
        set_local<std::int64_t>(ip);
      }
        break;
      case OPCODE::OP_PUSH_SINT64_CONST: {
        push_const<std::int64_t>(ip);
      }
        break;
      case OPCODE::OP_ADD_SINT32: {
        add<std::int32_t>();
      }
        break;
      case OPCODE::OP_MULTIPLY_SINT32: {
        multiply<std::int32_t>();
      }
        break;
      case OPCODE::OP_DIVIDE_SINT32: {
        divide<std::int32_t>();
      }
        break;
      case OPCODE::OP_SUBTRACT_SINT32: {
        subtract<std::int32_t>();
      }
        break;
      case OPCODE::OP_NEGATE_SINT32: {
        negate<std::int32_t>();
      }
        break;
      case OPCODE::OP_GREATER_SINT32: {
        greater<std::int32_t>();
      }
        break;
      case OPCODE::OP_LESS_SINT32: {
        less<std::int32_t>();
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_SINT32: {
        greater_equal<std::int32_t>();
      }
        break;
      case OPCODE::OP_LESS_EQUAL_SINT32: {
        less_equal<std::int32_t>();
      }
        break;
      case OPCODE::OP_EQUAL_SINT32: {
        equal<std::int32_t>();
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_SINT32: {
        push_local<std::int32_t>(ip);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_SINT32: {
        pop_to_local<std::int32_t>(ip);
      }
        break;
      case OPCODE::OP_SET_LOCAL_SINT32: {
        set_local<std::int32_t>(ip);
      }
        break;
      case OPCODE::OP_PUSH_SINT32_CONST: {
        push_const<std::int32_t>(ip);
      }
        break;
      case OPCODE::OP_ADD_SINT16: {
        add<std::int16_t>();
      }
        break;
      case OPCODE::OP_MULTIPLY_SINT16: {
        multiply<std::int16_t>();
      }
        break;
      case OPCODE::OP_DIVIDE_SINT16: {
        divide<std::int16_t>();
      }
        break;
      case OPCODE::OP_SUBTRACT_SINT16: {
        subtract<std::int16_t>();
      }
        break;
      case OPCODE::OP_NEGATE_SINT16: {
        negate<std::int16_t>();
      }
        break;
      case OPCODE::OP_GREATER_SINT16: {
        greater<std::int16_t>();
      }
        break;
      case OPCODE::OP_LESS_SINT16: {
        less<std::int16_t>();
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_SINT16: {
        greater_equal<std::int16_t>();
      }
        break;
      case OPCODE::OP_LESS_EQUAL_SINT16: {
        less_equal<std::int16_t>();
      }
        break;
      case OPCODE::OP_EQUAL_SINT16: {
        equal<std::int16_t>();
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_SINT16: {
        push_local<std::int16_t>(ip);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_SINT16: {
        pop_to_local<std::int16_t>(ip);
      }
        break;
      case OPCODE::OP_SET_LOCAL_SINT16: {
        set_local<std::int16_t>(ip);
      }
        break;
      case OPCODE::OP_PUSH_SINT16_CONST: {
        push_const<std::int16_t>(ip);
      }
        break;
      case OPCODE::OP_ADD_SINT8: {
        add<std::int8_t>();
      }
        break;
      case OPCODE::OP_MULTIPLY_SINT8: {
        multiply<std::int8_t>();
      }
        break;
      case OPCODE::OP_DIVIDE_SINT8: {
        divide<std::int8_t>();
      }
        break;
      case OPCODE::OP_SUBTRACT_SINT8: {
        subtract<std::int8_t>();
      }
        break;
      case OPCODE::OP_NEGATE_SINT8: {
        negate<std::int8_t>();
      }
        break;
      case OPCODE::OP_GREATER_SINT8: {
        greater<std::int8_t>();
      }
        break;
      case OPCODE::OP_LESS_SINT8: {
        less<std::int8_t>();
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_SINT8: {
        greater_equal<std::int8_t>();
      }
        break;
      case OPCODE::OP_LESS_EQUAL_SINT8: {
        less_equal<std::int8_t>();
      }
        break;
      case OPCODE::OP_EQUAL_SINT8: {
        equal<std::int8_t>();
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_SINT8: {
        push_local<std::int8_t>(ip);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_SINT8: {
        pop_to_local<std::int8_t>(ip);
      }
        break;
      case OPCODE::OP_SET_LOCAL_SINT8: {
        set_local<std::int8_t>(ip);
      }
        break;
      case OPCODE::OP_PUSH_SINT8_CONST: {
        push_const<std::int8_t>(ip);
      }
        break;
      case OPCODE::OP_ADD_FLOAT: {
        add<float>();
      }
        break;
      case OPCODE::OP_MULTIPLY_FLOAT: {
        multiply<float>();
      }
        break;
      case OPCODE::OP_DIVIDE_FLOAT: {
        divide<float>();
      }
        break;
      case OPCODE::OP_SUBTRACT_FLOAT: {
        subtract<float>();
      }
        break;
      case OPCODE::OP_NEGATE_FLOAT: {
        negate<float>();
      }
        break;
      case OPCODE::OP_GREATER_FLOAT: {
        greater<float>();
      }
        break;
      case OPCODE::OP_LESS_FLOAT: {
        less<float>();
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_FLOAT: {
        greater_equal<float>();
      }
        break;
      case OPCODE::OP_LESS_EQUAL_FLOAT: {
        less_equal<float>();
      }
        break;
      case OPCODE::OP_EQUAL_FLOAT: {
        equal<float>();
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_FLOAT: {
        push_local<float>(ip);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_FLOAT: {
        pop_to_local<float>(ip);
      }
        break;
      case OPCODE::OP_SET_LOCAL_FLOAT: {
        set_local<float>(ip);
      }
        break;
      case OPCODE::OP_PUSH_FLOAT_CONST: {
        push_const<float>(ip);
      }
        break;
      case OPCODE::OP_ADD_DOUBLE: {
        add<double>();
      }
        break;
      case OPCODE::OP_MULTIPLY_DOUBLE: {
        multiply<double>();
      }
        break;
      case OPCODE::OP_DIVIDE_DOUBLE: {
        divide<double>();
      }
        break;
      case OPCODE::OP_SUBTRACT_DOUBLE: {
        subtract<double>();
      }
        break;
      case OPCODE::OP_NEGATE_DOUBLE: {
        negate<double>();
      }
        break;
      case OPCODE::OP_GREATER_DOUBLE: {
        greater<double>();
      }
        break;
      case OPCODE::OP_LESS_DOUBLE: {
        less<double>();
      }
        break;
      case OPCODE::OP_GREATER_EQUAL_DOUBLE: {
        greater_equal<double>();
      }
        break;
      case OPCODE::OP_LESS_EQUAL_DOUBLE: {
        less_equal<double>();
      }
        break;
      case OPCODE::OP_EQUAL_DOUBLE: {
        equal<double>();
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_DOUBLE: {
        push_local<double>(ip);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_DOUBLE: {
        pop_to_local<double>(ip);
      }
        break;
      case OPCODE::OP_SET_LOCAL_DOUBLE: {
        set_local<double>(ip);
      }
        break;
      case OPCODE::OP_PUSH_DOUBLE_CONST: {
        push_const<double>(ip);
      }
        break;
      case OPCODE::OP_PUSH_LOCAL_STRING: {
        push_local<sanema::StringReference>(ip);
      }
        break;
      case OPCODE::OP_SET_EXTERNAL_STRING: {
        pop_to_local<sanema::StringReference>(ip);
      }
        break;
      case OPCODE::OP_SET_LOCAL_STRING: {
        set_local<sanema::StringReference>(ip);
      }
        break;
      case OPCODE::OP_PUSH_STRING_CONST: {
        auto string_literal_index = read_from_bytecode<StringReference>(ip);
        push(string_literal_index);
      }
        break;
      case OPCODE::OP_JUMP: {
        auto offset = read_from_bytecode<std::uint16_t>(ip);
        ip += offset;
      }
        break;
      case OPCODE::OP_CALL_EXTERNAL_FUNCTION: {
        auto function_id = read_from_bytecode<std::uint64_t>(ip);
        auto &function = binding_collection.get_function_by_id(function_id);
        function.call(*this);
      }
        break;
      case OPCODE::OP_CALL: {
        auto function_address = read_from_bytecode<std::uint64_t>(ip);
        IPType new_ip = byte_code.code_data.data() + function_address;
        auto &last_call_stack = call_stack.back();
        last_call_stack.ip = ip;
        call_stack.emplace_back(last_call_stack.get_end_address());
        ip = new_ip;
//        std::cout<<"calling function "<<function_address<<"\n";
      }
        break;
      case OPCODE::OP_PREPARE_PARAMETER: {
        auto address = read_from_bytecode<std::uint64_t>(ip);
        std::uint8_t *global_address = call_stack.back().get_begin_address() + address;
        auto value=pop<OperandType>();
        push<address_t>(address_t{global_address});
        push<OperandType>(value);

      }
        break;
      case OPCODE::OP_JUMP_IF_FALSE: {
        auto offset = read_from_bytecode<std::uint64_t>(ip);
//        std::cout<<"offset: "<<offset<<"\n";
        auto value = pop<bool>();
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
        }else{
          return;
        }
        break;
      }
      case OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL: {
        auto local_address = read_from_bytecode<std::uint64_t>(ip);
        auto global_address = call_stack.back().get_begin_address() + local_address;
        push(address_t{global_address});
      }
        break;
      case OPCODE::OP_PUSH_EXTERNAL_FIELD_ADDRESS: {
        auto field_id = pop<std::uint64_t>();
        auto type_id = pop<std::uint64_t>();
        auto object_address = pop<address_t>();
        auto &type = binding_collection.get_type_by_id(type_id);
        auto field_pointer = type.get_field_address((void *) object_address.address,
                                                    field_id);
        push(address_t{(std::uint8_t *) field_pointer});
      }
        break;
      case OPCODE::OP_POP_GLOBAL_ADDRESS_AS_LOCAL: {
        sanema::ContextFrame &context_frame = call_stack.back();
        auto global_address = pop<address_t>();
        auto variable_address = pop<address_t>();
        address_t new_local_address{
          global_address.address - (int64_t) (context_frame.get_begin_address() - stack_memory.data())};
      }
        break;
      case OPCODE::OP_NIL:
        break;
      case OPCODE::OP_PUSH_LOCAL_BOOL:
        break;
      case OPCODE::OP_SET_EXTERNAL_BOOL:
        break;
      case OPCODE::OP_SET_LOCAL_BOOL:
        break;
      case OPCODE::OP_NOT:
        break;
      case OPCODE::OP_PRINT:
        break;
      case OPCODE::OP_INVOKE:
        break;
      case OPCODE::OP_SUPER_INVOKE:
        break;
      case OPCODE::OP_CLOSURE:
        break;
      case OPCODE::OP_CLOSE_UPVALUE:
        break;
      case OPCODE::OP_CLASS:
        break;
      case OPCODE::OP_INHERIT:
        break;
      case OPCODE::OP_METHOD:
        break;
    }
  }
}


sanema::VM::VM(int memory_size_mb) : running_byte_code(nullptr) {
  auto megabytes_to_bytes = [](std::uint64_t size) { return (size * 1024) * 1024; };
  stack_memory.reserve(megabytes_to_bytes(memory_size_mb));
  operand_stack=new unsigned char[megabytes_to_bytes(memory_size_mb)];

}

sanema::VM::~VM() {
  delete operand_stack;
}

void sanema::VM::prepare_function_parameters(std::uint32_t n) {
  external_function_parameters.clear();
  for (int i = 0; i < n; i++) {
    external_function_parameters.emplace_back(pop<OperandType>());
  }
}

sanema::OperandType
sanema::VM::get_external_function_parameter(size_t index) const {
  return external_function_parameters[external_function_parameters.size() - 1 - index];
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
  push(reference);
}


template<>
std::string sanema::get_function_parameter_from_vm<std::string>(VM &vm, size_t index,
                                                                sanema::FunctionParameter::Modifier modifier) {
  auto value = vm.get_external_function_parameter(index);
  StringReference reference;
  switch (modifier) {
    case FunctionParameter::Modifier::VALUE:
      reference = static_cast<StringReference>(value);
      break;
    case FunctionParameter::Modifier::MUTABLE:
    case FunctionParameter::Modifier::CONST:
      auto address = static_cast<address_t>(value);
      reference = vm.call_stack.back().read<StringReference>(address);
      break;
  }
  return vm.get_string(reference);
}

template<>
std::string const &sanema::get_function_parameter_from_vm<std::string const &>(VM &vm, size_t index,
                                                                               sanema::FunctionParameter::Modifier modifier) {
  auto value = vm.get_external_function_parameter(index);
  StringReference reference;
  switch (modifier) {
    case FunctionParameter::Modifier::VALUE:
      reference = static_cast<StringReference>(value);
      break;
    case FunctionParameter::Modifier::CONST:
    case FunctionParameter::Modifier::MUTABLE:
      auto address = static_cast<address_t>(value);
      reference = *((StringReference *) address.address);
      break;
  }
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
