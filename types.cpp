


#include "types.h"
#include <util/lambda_visitor.hpp>
#include <algorithm>
sanema::TypeIdentifier::TypeIdentifier(const std::string &identifier)
  : identifier(identifier) {}

bool sanema::TypeIdentifier::operator==(const sanema::TypeIdentifier &rhs) const {
  return identifier == rhs.identifier;
}

bool sanema::TypeIdentifier::operator!=(const sanema::TypeIdentifier &rhs) const {
  return !(rhs == *this);
}


sanema::UserDefined::UserDefined(sanema::TypeIdentifier typeId)
  : type_id(std::move(typeId)) {}

bool sanema::UserDefined::operator==(const sanema::UserDefined &rhs) const {
  return type_id == rhs.type_id;
}

bool sanema::UserDefined::operator!=(const sanema::UserDefined &rhs) const {
  return !(rhs == *this);
}

sanema::Field *sanema::UserDefined::get_field(std::string const& identifier) {
  auto iter=std::find_if(fields.begin(),fields.end(),[&](Field& field)->bool{
    return identifier==field.identifier;
  });
  if(iter!=fields.end()){
    return &(*iter);
  }
  return nullptr;
}

std::optional<sanema::IncompleteType> sanema::parse_type(const std::string &string) {
  std::map<std::string, IncompleteType> type_map{
    {"int8",   Integer{8}},
    {"int16",  Integer{16}},
    {"int32",  Integer{32}},
    {"int64",  Integer{64}},
    {"uint8",  Integer{8, false}},
    {"uint16", Integer{16, false}},
    {"uint32", Integer{32, false}},
    {"uint64", Integer{64, false}},
    {"float",  Float{}},
    {"double", Double{}},
    {"bool",   Boolean{}},
    {"void",   Void{}},
    {"string", String{}}
  };
  if (type_map.count(string) > 0) {
    return type_map.at(string);
  }
  return UnidentifiedType{string};
}

std::uint64_t sanema::get_type_size(sanema::CompleteType const &type) {
  return match(type,
               [](Integer const &integer) -> std::uint64_t {
                 return integer.size/8;
               },
               [](Float const &a_float) -> std::uint64_t {
                 return 4;
               },
               [](Double const &a_double) -> std::uint64_t {
                 return 8;
               },
               [](String const &a_double) -> std::uint64_t {
                 return sizeof(sanema::StringReference) ;
               },
               [](Boolean const &a_double) -> std::uint64_t {
                 return 1;
               },
               [](Void const &a_double) -> std::uint64_t {
                 return 0;
               },
               [](UserDefined const &user_defined) -> std::uint64_t {
                 return user_defined.size;
               }
              );
}

bool sanema::is_second_type_compatible(sanema::CompleteType &type_1, sanema::CompleteType type_2) {
  if (std::holds_alternative<Integer>(type_1) && std::holds_alternative<Integer>(type_2)) {
    Integer integer1 = std::get<Integer>(type_1);
    Integer integer2 = std::get<Integer>(type_2);
    return integer2.size >= integer1.size;
  }
  return type_1 == type_2;
}

std::string sanema::type_to_string(sanema::CompleteType const &type) {
  return match(type,
               [](sanema::Integer const &integer) -> std::string {
                 switch (integer.size) {
                   case 8:
                     return "int8";
                     break;
                   case 16:
                     return "int16";
                     break;
                   case 32:
                     return "int32";
                     break;
                   case 64:
                     return "int64";
                     break;
                 };
                 return "unknown_size_int";
               },
               [](sanema::Float const &a_float) -> std::string {
                 return "float";
               },
               [](sanema::String const &integer) -> std::string {
                 return "string";
               },
               [](sanema::Void const &a_void) -> std::string {
                 return "void";
               },
               [](sanema::Double const &a_double) -> std::string {
                 return "double";
               },
               [](sanema::Boolean const &a_boolean) -> std::string {
                 return "boolean";
               },
               [](sanema::UserDefined const &a_struct) -> std::string {
                 return a_struct.type_id.identifier;
               }
              );
  return std::string("NO_TYPE");
}


bool sanema::Integer::operator==(const sanema::Integer &rhs) const {
  return size == rhs.size &&
         is_signed == rhs.is_signed;
}

bool sanema::Integer::operator!=(const sanema::Integer &rhs) const {
  return !(rhs == *this);
}

bool sanema::Float::operator==(const sanema::Float &rhs) const {
  return true;
}

bool sanema::Float::operator!=(const sanema::Float &rhs) const {
  return false;
}

bool sanema::Void::operator==(const sanema::Void &rhs) const {
  return true;
}

bool sanema::Void::operator!=(const sanema::Void &rhs) const {
  return false;
}

bool sanema::Double::operator==(const sanema::Double &rhs) const {
  return true;
}

bool sanema::Double::operator!=(const sanema::Double &rhs) const {
  return false;
}

bool sanema::String::operator==(const sanema::String &rhs) const {
  return true;
}

bool sanema::String::operator!=(const sanema::String &rhs) const {
  return false;
}

bool sanema::Boolean::operator==(const sanema::Boolean &rhs) const {
  return true;
}

bool sanema::Boolean::operator!=(const sanema::Boolean &rhs) const {
  return false;
}

std::ostream &sanema::operator<<(std::ostream &stream, const sanema::StringReference &string_reference) {
  stream << "string(";
  switch (string_reference.location) {
    case StringLocation::LiteralPool:
      stream << "literal:";
      break;
    case StringLocation::LocalStack:
      stream << "local:";
      break;
  }
  stream << string_reference.ref << ")";
  return stream;
}

bool sanema::is_user_defined(const sanema::CompleteType &type) {
  return get_type_category(type) == TypeCategory::USER_DEFINED;
}

sanema::TypeCategory sanema::get_type_category(CompleteType const &type) {
  return match(type,
        [](UserDefined const &type) -> sanema::TypeCategory {
          return TypeCategory::USER_DEFINED;
        },
        [](auto const &type) -> sanema::TypeCategory{
          return TypeCategory::PRIMITIVE ;
        }
       );
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::string>() {
  return String{};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::int8_t>() {
  return Integer{8};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::int16_t>() {
  return Integer{16};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::int32_t>() {
  return Integer{32};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::int64_t>() {
  return Integer{64};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::uint8_t>() {
  return Integer{8};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::uint16_t>() {
  return Integer{16};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::uint32_t>() {
  return Integer{32};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<std::uint64_t>() {
  return Integer{64};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<double>() {
  return Double{};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<float>() {
  return Float{};
}

template<>
sanema::CompleteType sanema::type_from_cpptype<bool>() {
  return Boolean{};
}

