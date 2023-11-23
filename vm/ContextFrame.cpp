//
// Created by fores on 11/1/2023.
//

#include "ContextFrame.h"

sanema::ContextFrame::ContextFrame(uint8_t *begin_address) : begin_address(begin_address),end_address{begin_address} {

}

uint8_t *sanema::ContextFrame::get_begin_address() const {
  return begin_address;
}

uint8_t *sanema::ContextFrame::get_end_address() const {
  return end_address;
}
