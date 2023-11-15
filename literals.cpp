

#include "literals.h"
#include "util/lambda_visitor.hpp"
#include <stdexcept>




sanema::Literal sanema::get_default_literal_for_type(sanema::CompleteType const &type) {
  return match(type,
               [](sanema::Integer const &integer) -> Literal {
                 switch (integer.size) {
                   case 8:
                     return LiteralSInt8(0);
                     break;
                   case 16:
                     return LiteralSInt16(0);
                     break;
                   case 32:
                     return LiteralSInt32(0);
                     break;
                   case 64:
                     return LiteralSInt64(0);
                     break;
                 };
                 return LiteralSInt8(0);

               },
               [](sanema::Float const &a_float) -> Literal {
                 return LiteralFloat(0);
               },
               [](sanema::String const &integer) -> Literal {
                 return LiteralString("");
               },
               [](sanema::Void const &a_void) -> Literal {
                 throw std::runtime_error("Void can't be set we should never reach this");
               },
               [](sanema::Double const &a_double) -> Literal {
                 return LiteralDouble(0);
               },
               [](sanema::Boolean const &integer) -> Literal {
                 return LiteralBoolean(false);
               },
               [](sanema::Struct const &integer) -> Literal {
                 throw std::runtime_error("Struct  can't be set we should never reach this");
               }
              );
}

sanema::CompleteType sanema::get_literal_type(Literal &literal) {
  return match(literal,
               [&](sanema::LiteralSInt64 int_64) -> sanema::CompleteType {
                 return sanema::Integer{64};
               },
               [](sanema::LiteralSInt32 int_32) -> sanema::CompleteType {
                 return sanema::Integer{32};
               },
               [](sanema::LiteralSInt16 int_16) -> sanema::CompleteType {
                 return sanema::Integer{16};
               },
               [](sanema::LiteralSInt8 int_8) -> sanema::CompleteType {
                 return sanema::Integer{8};
               },
               [](sanema::LiteralBoolean boolean) -> sanema::CompleteType {
                 return sanema::Boolean{};

               },
               [](sanema::LiteralFloat a_float) -> sanema::CompleteType {
                 return sanema::Float{};
               },
               [](sanema::LiteralDouble a_double) -> sanema::CompleteType {
                 return sanema::Double{};
               },
               [](sanema::LiteralString &string) -> sanema::CompleteType {
                 return sanema::String{};
               }
              );
}
