//
// Created by fores on 3/24/2024.
//

#ifndef SANEMA_ARGUMENT_H
#define SANEMA_ARGUMENT_H
#include <cstdint>
#include <variant>
#include <string>
namespace sanema{
  using ArgumentValue= std::variant<std::int8_t,
                                    std::int16_t ,
                                    std::int32_t,
                                    std::int64_t ,
                                    float,
                                    std::string,
                                    bool,
                                    void*>;
  struct Argument {
    Argument(const std::string &name, const ArgumentValue &value);

    std::string name;
    ArgumentValue value;

  };
}


#endif //SANEMA_ARGUMENT_H
