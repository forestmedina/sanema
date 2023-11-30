//
// Created by fores on 11/1/2023.
//

#ifndef NATURE_CONTEXT_FRAME_H
#define NATURE_CONTEXT_FRAME_H
#include <cassert>
#include <cstdint>
#include "OperandType.h"
namespace sanema {
  class ContextFrame {
  public:

    explicit ContextFrame(std::uint8_t *begin_address);

    template<class T>
     T read(address_t address){

      auto final_address=begin_address+std::uint64_t(address.address);
       auto const size_of_value=sizeof(T);
      auto end_read_address=final_address+size_of_value;
      //TODO we could make this a runtime check instead of only a assert
      assert(end_read_address<=end_address);
       return *reinterpret_cast<T *>(final_address);
     }
     template<class T>
     void write(address_t address,T value){

      auto final_address=begin_address+std::uint64_t (address.address);
      //TODO we could make this a runtime check instead of only a assert
      assert(final_address+sizeof(T)<end_address);
       *reinterpret_cast<T *>(final_address)=value;
     }
     inline void reserve(std::uint64_t size){
         end_address+=size;
     }
    std::uint8_t const * ip;
    [[nodiscard]] uint8_t *get_begin_address() const;

    [[nodiscard]] uint8_t *get_end_address() const;

  private:
    //begin and end are  addresses to a  point the stack. Begin address should be specified in the constructor
    uint8_t * begin_address;
    uint8_t * end_address;

  };
}


#endif //NATURE_CONTEXT_FRAME_H
