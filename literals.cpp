

#include "literals.h"
#include "util/lambda_visitor.hpp"
#include <stdexcept>


sanema::Literal sanema::conver_literal_to_type(const sanema::Literal &literal, sanema::CompleteType &type) {
  return match_double(type,
                      literal,
                      [](sanema::Integer type, LiteralSInt64 literal) -> Literal {
                        return get_integer_literal_from_type(type,
                                                      literal.value);
                      },
                      [](sanema::Integer type, LiteralSInt32 literal) -> Literal {
                        return get_integer_literal_from_type(type,
                                                      literal.value);
                      },
                      [](sanema::Integer type, LiteralSInt16 literal) -> Literal {
                        return get_integer_literal_from_type(type,
                                                      literal.value);
                      },
                      [](sanema::Integer type, LiteralSInt8 literal) -> Literal {
                        return get_integer_literal_from_type(type,
                                                      literal.value);
                      },
                      [](sanema::Float type, LiteralDouble literal) -> Literal {
                        return LiteralFloat(literal.value);
                      },
                      [](sanema::Double type, LiteralFloat literal) -> Literal {
                        return LiteralDouble(literal.value);
                      },
                      [](auto, auto literal) -> Literal {
                        return literal;
                      }
                     );

}


sanema::Literal sanema::get_integer_literal_from_type(sanema::Integer type, std::uint64_t value) {
  switch (type.size) {
    case 8:
      return LiteralSInt8{(std::int8_t) value};
      break;
    case 16:
      return LiteralSInt16{(std::int16_t) value};
      break;
    case 32:
      return LiteralSInt32{(std::int32_t) value};
      break;
    case 64:
      return LiteralSInt64{(std::int64_t) value};
      break;
  }
}

sanema::Literal sanema::get_default_literal_for_type(sanema::CompleteType const&type) {
  return match(type,
        [](sanema::Integer const &integer) ->Literal {
          switch (integer.size) {
            case 8: return LiteralSInt8(0);break;
            case 16:return LiteralSInt16(0);break;
            case 32: return  LiteralSInt32(0);break;
            case 64: return LiteralSInt64(0);break;
          };
          return LiteralSInt8(0);

        },
        [](sanema::Float const &a_float) ->Literal {
          return LiteralFloat(0);
        },
        [](sanema::String const &integer) ->Literal {
          return LiteralString("");
        },
        [](sanema::Void const &a_void) ->Literal {
          throw std::runtime_error("Void can't be set we should never reach this");
        },
        [](sanema::Double const &a_double)->Literal  {
          return LiteralDouble(0);
        },
        [](sanema::Boolean const &integer) ->Literal {
              return LiteralBoolean(false);
        },
        [](sanema::Struct const &integer) ->Literal {
          throw std::runtime_error("Struct  can't be set we should never reach this");
        }
    );
}
