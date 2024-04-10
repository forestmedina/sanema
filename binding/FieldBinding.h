//
// Created by fores on 11/2/2023.
//

#ifndef NATURE_FIELDBINDING_H
#define NATURE_FIELDBINDING_H

#include <string>
#include "Value.h"
#include <util/lambda_visitor.hpp>
#include <memory>
#include <types.h>

namespace sanema {

  struct FieldType {
    virtual CompleteType get_type() = 0;
  };
  template <typename T>
  struct FieldTypeGeneric : FieldType {
    CompleteType get_type() override {
      return type_from_cpptype<T>();
    }
  };
  struct FieldPointer {
    virtual void set(void *, Value &&) {

    };

    virtual Value get_value(void *) { return 0; };

    virtual void *get_address(void *) { return 0; };

    virtual CompleteType get_type() = 0;

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

    Value get_value(void *object) override {
      return ((ClassType *) object)->*m_member_pointer;
    };

    void *get_address(void *object) override {
      return (void*)&(((ClassType *) object)->*m_member_pointer);
    };

    virtual ~FieldPointerGeneric() = default;

    CompleteType get_type() override;

    FieldType ClassType::* m_member_pointer;
  };

  template<typename ClassType, typename FieldType>
  std::unique_ptr<FieldPointer> build_generic_field_pointer(FieldType ClassType::* field_pointer) {
    return std::make_unique<FieldPointerGeneric<ClassType, FieldType>>
      (field_pointer);
  }
  class FieldBinding {
  public:
    FieldBinding(std::string const &name, size_t offset,std::unique_ptr<FieldType> field_type);

    FieldBinding(FieldBinding &&other) {
      std::swap(name,
                other.name);
      std::swap(offset,
                other.offset);
      std::swap(field_type,
                other.field_type);
    };

    FieldBinding &operator=(FieldBinding &&other) {
      std::swap(name,
                other.name);
      std::swap(offset,
                other.offset);
      std::swap(field_type,
                other.field_type);
      return *this;
    };
    std::string name;
    size_t offset{0};
    std::unique_ptr<FieldType> field_type{nullptr};
  };

  template<typename ClassType, typename FieldType>
  CompleteType FieldPointerGeneric<ClassType, FieldType>::get_type() {
    return type_from_cpptype<FieldType>();
  }

  template <typename T>
   FieldBinding  bind_field(std::string name, size_t offset) {
    return FieldBinding{name, offset,std::make_unique<FieldTypeGeneric<T>>()};
  }
}

#endif //NATURE_FIELDBINDING_H
