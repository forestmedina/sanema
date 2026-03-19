//
// Created by fores on 11/13/2023.
//

#include "ScriptEntry.h"

sanema::ScriptEntry::ScriptEntry(ScriptID id, ByteCode bytecode)
  : id(id), bytecode(std::make_shared<ByteCode>(std::move(bytecode))) {}

sanema::ScriptEntry::ScriptEntry() : bytecode(std::make_shared<ByteCode>()) {}
