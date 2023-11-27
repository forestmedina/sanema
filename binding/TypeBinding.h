//
// Created by fores on 11/2/2023.
//


#ifndef NATURE_TYPEBINDING_H
#define NATURE_TYPEBINDING_H

#include <string>
#include <utility>
#include <vector>
#include "FieldBinding.h"
#include "MethodBinding.h"
#include <common/TypeCollection.h>
namespace sanema {


  class TypeBindingPointer {
  public:
    virtual void construct(void *address) = 0;

    virtual std::uint64_t size() = 0;
    virtual void register_type_definition(TypeCollection& type_collection,std::uint64_t id)=0;
    template<typename Type, typename Member>
    TypeBindingPointer* with_field(std::string identifier,Member Type::*field_member){
      add_field(identifier,bind_field(identifier,field_member));
      return this;
    }
    virtual ~TypeBindingPointer()=default;
    virtual void* get_field_address(void* object_ptr,std::uint64_t id)=0;
  private:
     virtual void add_field(std::string identifier,FieldBinding binding )=0;
  };

  template<typename T>
  class TypeBinding : public TypeBindingPointer {
  public:
    explicit TypeBinding(std::string name) : name(std::move(name)) {}

    TypeBinding(TypeBinding&& other)=default;
    TypeBinding & operator=(TypeBinding&& other)=default;
    void construct(void *address) override {
      T *t_pointer = new(address) T();
    }

    void register_type_definition(TypeCollection &type_collection, std::uint64_t id) override {
      UserDefined user_defined{TypeIdentifier{name}};
      user_defined.external_id=id;
      unsigned int field_id=0;
      for(auto& field_binding:fields){
        Field field_definition{field_binding.name,field_binding.field_pointer->get_type(),field_id};
        user_defined.fields.emplace_back(field_definition);
        ++field_id;
      }
      type_collection.add_type(user_defined);

    }

    void *get_field_address(void* object_ptr,std::uint64_t id) override {
      return fields[id].field_pointer->get_address(object_ptr);
    }

    std::uint64_t size() override {
      return sizeof(T);
    }
     ~TypeBinding() override=default;

  private:
    void add_field(std::string identifier,FieldBinding binding )override{
      fields.emplace_back(std::move(binding));
    };
    std::string name;
    std::vector<FieldBinding> fields{};

  };
}

#endif //NATURE_TYPEBINDING_H
