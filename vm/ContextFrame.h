//
// Created by fores on 11/1/2023.
//

#ifndef NATURE_CONTEXTFRAME_H
#define NATURE_CONTEXTFRAME_H
#include <vector>
#include <assert.h>
#include <cstdint>
namespace sanema {
  class ContextFrame {
  public:

    explicit ContextFrame(std::uint8_t *begin_address);

    template<class T>
     T read(std::uint64_t address){

      auto final_address=begin_address+address;
      //TODO we could make this a runtime check instead of only a assert
      assert(final_address+sizeof(T)<end_address);
       return *((T*)final_address);
     }
     template<class T>
     void write(std::uint64_t address,T value){

      auto final_address=begin_address+address;
      //TODO we could make this a runtime check instead of only a assert
      assert(final_address+sizeof(T)<end_address);
       *((T*)final_address)=value;
     }
     inline void reserve(std::uint64_t size){
         end_address+=size;
     }

  private:
    //begin and end are  addresses to a  point the stack. Begin address should specified in the constructor
    uint8_t * begin_address;
    uint8_t * end_address;

  };
}


#endif //NATURE_CONTEXTFRAME_H
