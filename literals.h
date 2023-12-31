

#ifndef NATURE_LITERALS_H
#define NATURE_LITERALS_H
#include "types.h"
namespace sanema{
  struct LiteralSInt64{std::int64_t value;};
  struct LiteralSInt32{std::int32_t value;};
  struct LiteralSInt16{std::int16_t value;};
  struct LiteralSInt8{std::int8_t value;};
  struct LiteralFloat{float value;};
  struct LiteralDouble{double value;};
  struct LiteralBoolean{bool value;};
  struct LiteralString{std::string value;};
  using Literal=std::variant<LiteralSInt64,LiteralSInt32,LiteralSInt16,LiteralSInt8,LiteralFloat,LiteralDouble,LiteralBoolean,LiteralString>;
  sanema::CompleteType get_literal_type(Literal& literal);
  Literal get_default_literal_for_type(sanema::CompleteType const& type);
}
#endif //NATURE_LITERALS_H
