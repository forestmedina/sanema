//
// Created by fores on 11/23/2023.
//

#ifndef SANEMA_OPERANDTYPE_H
#define SANEMA_OPERANDTYPE_H

#include <cstdint>
#include <types.h>
#include <boost/numeric/conversion/cast.hpp>

namespace sanema {

  struct local_register_t {
    local_register_t()=default;

    explicit local_register_t(std::uint16_t  address);

    std::uint16_t address{0};
    bool is_reference{0};
  };

  union OperandType {
    OperandType();

    bool bool_v;
    std::uint8_t uint8_v;
    std::uint16_t uint16_v;
    std::uint32_t uint32_v;
    std::uint64_t uint64_v;
    std::int8_t sint8_v;
    std::int16_t sint16_v;
    std::int32_t sint32_v;
    std::int64_t sint64_v;
    local_register_t address;
    float float_v;
    double double_v;
    sanema::StringReference string_reference;

    explicit inline OperandType(std::uint8_t value) : uint64_v(value) {}

    explicit inline OperandType(std::uint16_t value) : uint64_v(value) {}

    explicit inline OperandType(std::uint32_t value) : uint64_v(value) {}

    explicit inline OperandType(std::uint64_t value) : uint64_v(value) {}

    explicit inline OperandType(std::int8_t value) : sint64_v(value) {}

    explicit inline OperandType(std::int16_t value) : sint64_v(value) {}

    explicit inline OperandType(std::int32_t value) : sint64_v(value) {}

    explicit inline OperandType(std::int64_t value) : sint64_v(value) {}

    explicit inline OperandType(local_register_t value) : address(value) {}

    explicit inline OperandType(float value) : float_v(value) {}

    explicit inline OperandType(double value) : double_v(value) {}

    explicit inline OperandType(bool value) : bool_v(value) {}

    explicit inline OperandType(StringReference value) : string_reference(value) {}

    // Conversion operator for uint8_t
    explicit inline operator std::uint8_t() const { return uint64_v; }

    // Conversion operator for uint16_t
    explicit inline operator std::uint16_t() const { return uint64_v; }

    // Conversion operator for uint32_t
    explicit inline  operator std::uint32_t() const { return uint64_v; }

    // Conversion operator for uint64_t
    explicit  inline operator std::uint64_t() const { return uint64_v; }

    explicit inline  operator local_register_t() const { return address; }

    // Conversion operator for int8_t
    explicit inline  operator std::int8_t() const { return boost::numeric_cast<std::int8_t>(sint64_v); }

    // Conversion operator for int16_t
    explicit inline  operator std::int16_t() const { return boost::numeric_cast<std::int16_t>(sint64_v); }

    // Conversion operator for int32_t
    explicit inline  operator std::int32_t() const { return boost::numeric_cast<std::int32_t>(sint64_v); }

    // Conversion operator for int64_t
    explicit inline  operator std::int64_t() const { return sint64_v; }

    // Conversion operator for int64_t
    explicit  inline  operator sanema::StringReference() const { return string_reference; }

    // Conversion operator for float
    explicit inline  operator float() const { return float_v; }

    // Conversion operator for double
    explicit  inline  operator double() const { return double_v; }

    explicit inline  operator bool() const { return bool_v; }
  };

} // sanema

#endif //SANEMA_OPERANDTYPE_H
