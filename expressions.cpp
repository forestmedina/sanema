


#include "expressions.h"

void sanema::InstructionList::add_expresion(const sanema::Instruction &expression) {
  expressions.push_back(expression);
}


sanema::DefineStruct::DefineStruct() {}

sanema::DeclareVariable::DeclareVariable() {}

sanema::BlockInstruction::BlockInstruction(const sanema::Instruction &instruction_sum)
  : instruction_sum(instruction_sum) {}

bool sanema::FunctionDefinitionCompleted::operator==(const sanema::FunctionDefinitionCompleted &rhs) const {

  if (parameters.size() != rhs.parameters.size()) return false;
  for (int i = 0; i < parameters.size(); i++) {
    auto &parameter1 = parameters[i];
    auto &parameter2 = rhs.parameters[i];
    if (parameter1 != parameter2) {
      return false;
    }
  }
  return identifier == rhs.identifier;
}

bool sanema::FunctionDefinitionCompleted::operator!=(const sanema::FunctionDefinitionCompleted &rhs) const {
  return !(rhs == *this);
}

bool sanema::FunctionDefinitionCompleted::is_compatible(sanema::FunctionDefinitionCompleted &other) {
 if (parameters.size() != other.parameters.size()) return false;
  for (int i = 0; i < parameters.size(); i++) {
    auto &parameter1 = parameters[i];
    auto &parameter2 = other.parameters[i];
    if (!parameter1.is_compatible( parameter2)) {
      return false;
    }
  }
  return identifier == other.identifier;
}


