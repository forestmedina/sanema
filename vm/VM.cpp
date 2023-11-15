//
// Created by fores on 11/1/2023.
//

#include "VM.h"
#include "opcodes.h"
#include <types.h>
#include <binding/BindingCollection.h>
void sanema::VM::run(ByteCode const& byte_code,BindingCollection & binding_collection) {
  running_byte_code=&byte_code;
  IPType ip = byte_code.code_data.data();
  auto end_address = byte_code.code_data.data() + byte_code.code_data.size();
  while (ip < end_address) {
    std::cout << "Ip offset: " << (ip - byte_code.code_data.data()) << "\n";
    execute_instruction(ip,binding_collection);
  }

}
//Generate function calls for each operation of the specified type
#define GENERATE_OPERATION(OP_TYPE, type, OPERATIONENUM, OPERATION_FUNCTION)  case OPCODE::OP_##OPERATIONENUM##OP_TYPE: {OPERATION_FUNCTION<type>();}break;
#define GENERATE_LOCAL_OPERATION(OP_TYPE, type, OPERATIONENUM, OPERATION_FUNCTION)  case OPCODE::OP_##OPERATIONENUM##OP_TYPE: {OPERATION_FUNCTION<type>(ip);}break;
#define GENERATE_PUSH(OPTYPE, type)  case OPCODE::OP_PUSH_##OPTYPE##_CONST: {push_const<type>(ip);}break;
#define GENERATE_TYPE_OPERATIONS(OP_TYPE, type) \
      GENERATE_OPERATION(OP_TYPE,type,ADD_,add)\
      GENERATE_OPERATION(OP_TYPE,type,MULTIPLY_,multiply)\
      GENERATE_OPERATION(OP_TYPE,type,DIVIDE_,divide)\
      GENERATE_OPERATION(OP_TYPE,type,SUBTRACT_,subtract)\
      GENERATE_OPERATION(OP_TYPE,type,NEGATE_,negate)\
      GENERATE_OPERATION(OP_TYPE,type,GREATER_,greater)\
      GENERATE_OPERATION(OP_TYPE,type,LESS_,less)\
      GENERATE_OPERATION(OP_TYPE,type,GREATER_EQUAL_,greater_equal)\
      GENERATE_OPERATION(OP_TYPE,type,LESS_EQUAL_,less_equal)\
      GENERATE_OPERATION(OP_TYPE,type,EQUAL_,equal)      \
      GENERATE_LOCAL_OPERATION(OP_TYPE,type,PUSH_LOCAL_,push_local)\
      GENERATE_LOCAL_OPERATION(OP_TYPE,type,POP_TO_LOCAL_,pop_to_local)\
      GENERATE_LOCAL_OPERATION(OP_TYPE,type,SET_LOCAL_,set_local)\
      GENERATE_PUSH(OP_TYPE,type)

sanema::ExecuteResult sanema::VM::execute_instruction(sanema::IPType &ip,BindingCollection& binding_collection) {
  auto opcode = static_cast<OPCODE>(*ip);
  std::cout << "Executing opcode: " << opcode_to_string(opcode) << "\n";
  ip++;
  switch (opcode) {
    case OPCODE::OP_POP: {
      pop<uint64_t>();
    }
      break;
    case OPCODE::OP_RESERVE_STACK_SPACE : {
      auto size = read_from_bytecode<std::uint64_t>(ip);
      sanema::ContextFrame &context_frame = call_stack.back();
      context_frame.reserve(size);
    }
      break;
    case OPCODE::OP_TRUE: {
      operand_stack.emplace_back(true);
    }
      break;
    case OPCODE::OP_FALSE: {
      operand_stack.emplace_back(false);
    }
      break;

    GENERATE_TYPE_OPERATIONS(SINT64,
                             std::int64_t)
    GENERATE_TYPE_OPERATIONS(SINT32,
                             std::int32_t)
    GENERATE_TYPE_OPERATIONS(SINT16,
                             std::int16_t)
    GENERATE_TYPE_OPERATIONS(SINT8,
                             std::int8_t)
    GENERATE_TYPE_OPERATIONS(FLOAT,
                             float)
    GENERATE_TYPE_OPERATIONS(DOUBLE,
                             double)
    GENERATE_LOCAL_OPERATION(STRING,
                             sanema::StringReference,
                             PUSH_LOCAL_,
                             push_local)
    GENERATE_LOCAL_OPERATION(STRING,
                             sanema::StringReference,
                             POP_TO_LOCAL_,
                             pop_to_local)
    GENERATE_LOCAL_OPERATION(STRING,
                             sanema::StringReference,
                             SET_LOCAL_,
                             set_local)
    case OPCODE::OP_PUSH_STRING_CONST: {
      auto string_literal_index = read_from_bytecode<std::uint64_t>(ip);
      push(string_literal_index);
    }
      break;
    case OPCODE::OP_JUMP: {
      auto offset = read_from_bytecode<std::uint16_t>(ip);
      ip += offset;
    }
      break;
    case OPCODE::OP_CALL_EXTERNAL_FUNCTION: {
        auto function_id=read_from_bytecode<std::uint64_t>(ip);
        auto& function=binding_collection.get_function_by_id(function_id);
        function.call(*this);

    }
    break;
    case OPCODE::OP_JUMP_IF_FALSE: {
      auto offset = read_from_bytecode<std::uint16_t>(ip);
      auto value = pop<bool>();
      if (!value) {
        ip += offset;
      }
    }
      break;


  }
  return ExecuteResult::OK;
}

sanema::VM::VM(int memory_size_mb)  {
  auto megabytes_to_bytes = [](std::uint64_t size) { return (size * 1024) * 1024; };
  stack_memory.reserve(megabytes_to_bytes(memory_size_mb));
  call_stack.emplace_back(stack_memory.data());
}

void sanema::VM::prepare_function_parameters(std::uint32_t n) {
  external_function_parameters.clear();
  for(int i=0;i<n;i++) {
    sanema::FunctionParameterType access_type{pop<uint8_t>()};

        external_function_parameters.emplace_back(access_type, pop<OperandType>());
    }

}

std::pair<sanema::FunctionParameterType,sanema::OperandType> sanema::VM::get_external_function_parameter(size_t index) {

  return external_function_parameters[external_function_parameters.size()-1-index];
}

std::string const& sanema::VM::get_string(const sanema::StringReference &reference) {
  switch (reference.location) {
    case StringLocation::LiteralPool:
      if(running_byte_code) {
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

void sanema::VM::push_string(std::string const& string_value) {
  string_stack.emplace_back(string_value);
  std::cout<<"Pushing string: "<<string_value<<"\n";
  StringReference reference{StringLocation::LocalStack,boost::numeric_cast<std::uint32_t>(string_stack.size()-1)};
  push(reference);
}


template<>
std::string sanema::get_function_parameter_from_vm<std::string>(VM &vm, size_t index) {
  auto value= vm.get_external_function_parameter(index);
  StringReference reference;
  switch (value.first) {
    case FunctionParameterType::Value:
      reference=static_cast<StringReference>(value.second);
      break;
    case FunctionParameterType::VariableReferece:
      auto address=static_cast<std::uint64_t>(value.second);
      reference=vm.call_stack.back().read<StringReference>(address);
    break;
  }
  return vm.get_string(reference);
}

template<>
std::string const& sanema::get_function_parameter_from_vm<std::string const&>(VM &vm, size_t index) {
  auto value= vm.get_external_function_parameter(index);
  StringReference reference;
  switch (value.first) {
    case FunctionParameterType::Value:
      reference=static_cast<StringReference>(value.second);
    break;
    case FunctionParameterType::VariableReferece:
      auto address=static_cast<std::uint64_t>(value.second);
    reference=vm.call_stack.back().read<StringReference>(address);
    break;
  }
  return vm.get_string(reference);
}

template<>
  void sanema::push_function_return_to_vm<std::string>(sanema::VM& vm, std::string vaule) {
  vm.push_string(vaule);
}

template<>
void sanema::push_function_return_to_vm<std::string const&>(sanema::VM& vm, std::string const& vaule) {
  vm.push_string(vaule);
}

template<>
void sanema::push_function_return_to_vm<std::string&>(sanema::VM& vm, std::string& vaule) {
  vm.push_string(vaule);
}