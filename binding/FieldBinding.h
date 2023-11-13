//
// Created by fores on 11/2/2023.
//

#ifndef NATURE_FIELDBINDING_H
#define NATURE_FIELDBINDING_H

#include <string>
#include "Value.ixx"
#include <util/lambda_visitor.hpp>

namespace sanema {


  struct FieldPointer {
    virtual void set(void *, Value &&) {

    };

    virtual Value get_value(void *) { return 0; };

    virtual ~FieldPointer() = default;
  };

  template<typename ClassType, typename FieldType>
  struct FieldPointerGeneric : FieldPointer {
    FieldPointerGeneric(FieldType ClassType::* member_pointer) : m_member_pointer{member_pointer} {

    }

    virtual void set(void *object, Value &&value) override {
      match(value,
            [object, this](FieldType &correct_value) {
              ((ClassType *) object)->*m_member_pointer = correct_value;
            },
            [](auto &ignore) {
            }
           );
    };

    virtual Value get_value(void *object) override {
      return ((ClassType *) object)->*m_member_pointer;
    };

    virtual ~FieldPointerGeneric() = default;

    FieldType ClassType::* m_member_pointer;
  };

  template<typename ClassType, typename FieldType>
  std::unique_ptr<FieldPointer> build_generic_field_pointer(FieldType ClassType::* field_pointer) {
    return std::make_unique<FieldPointerGeneric < ClassType, FieldType>>
    (field_pointer);
  }

  class FieldBinding {
    std::string name;
    std::unique_ptr<FieldPointer> field_pointer;
  };
  template<typename Type, typename Member>
  FieldBinding bind_field(std::string name,Member Type::* field_pointer){
    return FieldBinding{name, build_generic_field_pointer(field_pointer)};
  }
}

#endif //NATURE_FIELDBINDING_H
