


#ifndef NATURE_TYPES_H
#define NATURE_TYPES_H

#include <variant>
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <memory>
#include <ostream>
#include <cstdint>

namespace sanema {
  struct Integer {
    char size = 32;
    bool is_signed = true;

    bool operator==(const Integer &rhs) const;

    bool operator!=(const Integer &rhs) const;
  };

  struct Float {

    bool operator==(const Float &rhs) const;

    bool operator!=(const Float &rhs) const;
  };

  struct Void {

    bool operator==(const Void &rhs) const;

    bool operator!=(const Void &rhs) const;
  };

  struct Double {

    bool operator==(const Double &rhs) const;

    bool operator!=(const Double &rhs) const;
  };

  struct String {

    bool operator==(const String &rhs) const;

    bool operator!=(const String &rhs) const;
  };

  struct Boolean {

    bool operator==(const Boolean &rhs) const;

    bool operator!=(const Boolean &rhs) const;
  };

  enum class StringLocation : std::uint8_t {
    LiteralPool,
    LocalStack
  };
  struct StringReference {


    StringLocation location;
    std::uint32_t ref;

  };

  std::ostream &operator<<(std::ostream &stream, StringReference const &string_reference);

  using PrimitiveType = std::variant<Integer, Float, Double, String, Boolean>();
  struct Struct;

  struct TypeId {
    TypeId(const std::string &identifier);

    std::string identifier{};

    bool operator==(const TypeId &rhs) const;

    bool operator!=(const TypeId &rhs) const;
  };

  struct Field;

  struct Struct {
    Struct(const TypeId &typeId);

    TypeId type_id;
    std::vector<Field> fields;

    bool operator==(const Struct &rhs) const;

    bool operator!=(const Struct &rhs) const;
  };

  using CompleteType = std::variant<Integer, Float, Double, String, Boolean, Struct, Void>;

  bool is_second_type_compatible(CompleteType &type_1, CompleteType type_2);

  std::optional<sanema::CompleteType> parse_type(std::string const &string);

  template<typename CPPTYPE, typename SANTYPE>
  SANTYPE type_from_cpptype();

  template<>
  String type_from_cpptype<std::string, String>() {
    return String{};
  }

  template<>
  Integer type_from_cpptype<std::uint8_t, Integer>() {
    return Integer{8};
  }

  template<>
  Integer type_from_cpptype<std::uint16_t, Integer>() {
    return Integer{16};
  }

  template<>
  Integer type_from_cpptype<std::uint32_t, Integer>() {
    return Integer{32};
  }

  template<>
  Double type_from_cpptype<double, Double>() {
    return Double{};
  }

  template<>
  Float type_from_cpptype<float, Float>() {
    return Float{};
  }

  template<>
  Boolean type_from_cpptype<bool, Boolean>() {
    return Boolean{};
  }

  template<>
  Integer type_from_cpptype<std::uint64_t, Integer>() {
    return Integer{64};
  }


  std::string type_to_string(CompleteType const &type);

  uint64_t get_type_size(CompleteType &type);

  struct Field {
    std::string identifier;
    std::optional<CompleteType> type{};
  };
}
#endif //NATURE_TYPES_H
