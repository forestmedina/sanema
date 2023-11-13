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
namespace sanema {
  using IPType = std::uint8_t const *;
  enum class ExecuteResult {
    OK,
    END,
    ERROR
  };
  enum class OperandTypeEnum:std::uint64_t{
    Boolean=1,
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


  union OperandType {
    bool bool_v;
    std::uint8_t uint8_v;
    std::uint16_t uint16_v;
    std::uint32_t uint32_v;
    std::uint64_t uint64_v;
    std::int8_t sint8_v;
    std::int16_t sint16_v;
    std::int32_t sint32_v;
    std::int64_t sint64_v;
    float float_v;
    double double_v;
    sanema::StringReference string_reference;

    explicit OperandType(std::uint8_t value) : uint64_v(value) {}

    explicit OperandType(std::uint16_t value) : uint64_v(value) {}

    explicit OperandType(std::uint32_t value) : uint64_v(value) {}

    explicit OperandType(std::uint64_t value) : uint64_v(value) {}

    explicit OperandType(std::int8_t value) : sint64_v(value) {}

    explicit OperandType(std::int16_t value) : sint64_v(value) {}

    explicit OperandType(std::int32_t value) : sint64_v(value) {}

    explicit OperandType(std::int64_t value) : sint64_v(value) {}

    explicit OperandType(float value) : float_v(value) {}

    explicit OperandType(double value) : double_v(value) {}
    explicit OperandType(StringReference value) : string_reference(value) {}

    // Conversion operator for uint8_t
    explicit operator std::uint8_t() const { return uint64_v; }

    // Conversion operator for uint16_t
    explicit operator std::uint16_t() const { return uint64_v; }

    // Conversion operator for uint32_t
    explicit  operator std::uint32_t() const { return uint64_v; }

    // Conversion operator for uint64_t
    explicit  operator std::uint64_t() const { return uint64_v; }

    // Conversion operator for int8_t
    explicit  operator std::int8_t() const { return boost::numeric_cast<std::int8_t>(sint64_v); }

    // Conversion operator for int16_t
    explicit operator std::int16_t() const { return boost::numeric_cast<std::int16_t>(sint64_v); }

    // Conversion operator for int32_t
    explicit  operator std::int32_t() const { return boost::numeric_cast<std::int32_t>(sint64_v); }

    // Conversion operator for int64_t
    explicit operator std::int64_t() const { return sint64_v; }
    // Conversion operator for int64_t
    explicit operator sanema::StringReference() const { return string_reference; }

    // Conversion operator for float
    explicit operator float() const { return float_v; }

    // Conversion operator for double
    explicit  operator double() const { return double_v; }

    explicit  operator bool() const { return bool_v; }
  };

  class VM {
  public:
    VM(int megabyte_size = 300);

    void run(ByteCode const &byte_code);

    template <class T>
    std::optional<T> get_value_stack(){
      if(operand_stack.empty()) return {};
      return pop<T>();
    }
  private:
    std::vector<OperandType> operand_stack;
    std::vector<sanema::ContextFrame> call_stack;
    std::vector<std::string> string_stack;
    std::vector<std::uint8_t> stack_memory;

    ExecuteResult execute_instruction(IPType &ip);

    template<class type>
    inline void push_local(IPType &ip) {
      auto address = read_from_bytecode<std::uint64_t>(ip);
      sanema::ContextFrame &context_frame = call_stack.back();
      auto value = context_frame.read<type>(address);
      std::cout << "Setting local address=" << address << " value=" << value << "\n";
      push(value);
    }

    template<class type>
    inline void pop_to_local(IPType &ip) {
      auto address = read_from_bytecode<std::uint64_t>(ip);
      sanema::ContextFrame &context_frame = call_stack.back();
      auto value = pop<type>();
      context_frame.write<type>(address,
                                value);
    }

    template<class type>
    inline void set_local(IPType &ip) {
      auto address = read_from_bytecode<std::uint64_t>(ip);
      sanema::ContextFrame &context_frame = call_stack.back();
      auto value = pop<type>();
      std::cout << "Setting local value=" << value << " address=" << address << "\n";
      context_frame.write<type>(address,
                                value);
    }

    template<class type>
    type pop() {
      type t = (type) operand_stack.back();
      operand_stack.pop_back();
      return t;
    }

    template<class type>
    void push_const(IPType &ip) {
      auto value = read_from_bytecode<type>(ip);
      std::cout << "constant value " << value << "\n";
      push(value);
    }

    template<class type>
    void push(type t) {
      operand_stack.emplace_back(t);
    }

    template<typename type>
    inline void multiply() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      auto result = value1 * value2;
      push(result);
    }

    template<typename type>
    inline void add() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      auto result = value1 + value2;
      std::cout << "adding " << value1 << " + " << value2 << " = " << result << "\n";
      push(result);
    }

    template<typename type>
    inline void subtract() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      auto result = value1 - value2;
      push(result);
    }

    template<typename type>
    inline void divide() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      auto result = value1 / value2;
      push(result);
    }

    template<typename type>
    inline void negate() {
      //TODO implement type conversion
      auto value = pop<type>();
      auto result = -value;
      push(result);
    }

    template<typename type>
    inline void greater() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      bool result = value1 > value2;
      push(result);
    }

    template<typename type>
    inline void less() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      bool result = value1 < value2;
      push(result);
    }

    template<typename type>
    inline void equal() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      bool result = value1 == value2;
      push(result);
    }

    template<typename type>
    inline void greater_equal() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      bool result = value1 >= value2;
      push(result);
    }

    template<typename type>
    inline void less_equal() {
      //TODO implement type conversion
      auto value2 = pop<type>();
      auto value1 = pop<type>();
      bool result = value1 <= value2;
      push(result);
    }
  };

}
#endif //UPDATE_SKELETON_PY_VM_H
