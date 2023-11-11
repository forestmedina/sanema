//
// Created by fores on 11/1/2023.
//

#include "VM.h"
#include "opcodes.h"

void VM::run(const ByteCode &byte_code) {
  IPType ip = byte_code.code_data.data();
  auto end_address = byte_code.code_data.data() + byte_code.code_data.size();
  while (ip < end_address) {
    std::cout<<"Ip offset: "<<(ip-byte_code.code_data.data())<<"\n";
    execute_instruction(ip);
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

ExecuteResult VM::execute_instruction(IPType &ip) {
  auto opcode = static_cast<OPCODE>(*ip);
  std::cout<<"Executing opcode: "<<opcode_to_string(opcode)<<"\n";
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

    GENERATE_TYPE_OPERATIONS(SINT64,std::int64_t)
    GENERATE_TYPE_OPERATIONS(SINT32,std::int32_t)
    GENERATE_TYPE_OPERATIONS(SINT16,std::int16_t)
    GENERATE_TYPE_OPERATIONS(SINT8,std::int8_t)
    GENERATE_TYPE_OPERATIONS(FLOAT,float)
    GENERATE_TYPE_OPERATIONS(DOUBLE,double )
    case OPCODE::OP_JUMP: {
      auto offset = read_from_bytecode<std::uint16_t>(ip);
      ip += offset;
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

VM::VM(int megabyte_size) {
  auto megabytes_to_bytes = [](std::uint64_t size) { return (size * 1024) * 1024; };
  stack_memory.reserve(megabytes_to_bytes(megabyte_size));
  call_stack.emplace_back(stack_memory.data());
}
