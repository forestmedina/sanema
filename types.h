


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
    LiteralPool=1,
    LocalStack=2
  };
  enum class FunctionParameterType : std::uint8_t {
    VariableReferece=1,
    Value=2
  };
  struct StringReference {

    StringLocation location;
    std::uint32_t ref;

  };

  std::ostream &operator<<(std::ostream &stream, StringReference const &string_reference);

  using PrimitiveType = std::variant<Integer, Float, Double, String, Boolean>();
  struct UserDefined;

  struct TypeIdentifier {
    TypeIdentifier(const std::string &identifier);

    std::string identifier{};

    bool operator==(TypeIdentifier const&rhs) const;

    bool operator!=( TypeIdentifier const &rhs) const;
  };

  struct Field;

  struct UserDefined {
    UserDefined(TypeIdentifier typeId);

    TypeIdentifier type_id;
    std::vector<Field> fields;
    Field* get_field(std::string_view  identifier);
    bool operator==(const UserDefined &rhs) const;

    bool operator!=(const UserDefined &rhs) const;
  };



  using CompleteType = std::variant<Integer, Float, Double, String, Boolean, UserDefined, Void>;

  enum class TypeCategory {
    PRIMITIVE,
    EXTERNAL,
    USER_DEFINED
  };
  TypeCategory get_type_category(CompleteType const& type);
  bool is_user_defined(CompleteType const& type);

  bool is_second_type_compatible(CompleteType &type_1, CompleteType type_2);

  std::optional<sanema::CompleteType> parse_type(std::string const &string);



  std::string type_to_string(CompleteType const &type);

  uint64_t get_type_size(CompleteType const&type);

  struct Field {
    std::string identifier;
    std::optional<CompleteType> type{};
    std::uint64_t offset;
  };

    template<typename CPPTYPE>
  CompleteType type_from_cpptype(){
    return Void{};
  };

  template<>
  CompleteType type_from_cpptype<std::string>();

  template<>
 CompleteType type_from_cpptype<std::int8_t>();

  template<>
  CompleteType type_from_cpptype<std::int16_t>();

  template<>
  CompleteType type_from_cpptype<std::int32_t>();
  template<>
  CompleteType type_from_cpptype<std::int64_t>();
  template<>
  CompleteType type_from_cpptype<std::uint8_t>();

  template<>
  CompleteType type_from_cpptype<std::uint16_t>();

  template<>
  CompleteType type_from_cpptype<std::uint32_t>();

  template<>
  CompleteType type_from_cpptype<double>();

  template<>
  CompleteType type_from_cpptype<float>();

  template<>
  CompleteType type_from_cpptype<bool>();

  template<>
  CompleteType type_from_cpptype<std::uint64_t>();
}
#endif //NATURE_TYPES_H
