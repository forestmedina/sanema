//
// Created by fores on 11/13/2023.
//

#ifndef SANEMA_SCRIPTENTRY_H
#define SANEMA_SCRIPTENTRY_H

#include <ScriptID.h>
#include <compiler/ByteCodeCompiler.h>

namespace sanema {
  struct ScriptEntry {
    ScriptEntry(ScriptID id, ByteCode bytecode);

    ScriptEntry();

    ScriptID id{0};
    ByteCode bytecode{};
  };
}


#endif //SANEMA_SCRIPTENTRY_H
