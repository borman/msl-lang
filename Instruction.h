#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Atom.h"

struct Instruction
{
  enum Opcode
  {
    // Push to stack
    PushVar, PushInt, PushReal, PushBool, PushLiteral, PushArrayItem,
    // Pop from stack
    PopVar, PopArrayItem, PopDelete,
    // Tuple boundaries
    TupOpen, TupClose, TupUnOpen, TupUnClose,
    // Operations
    Add, Sub, Mul, Div, Mod, And, Or,
    // Tests
    TestLess, TestGreater, TestEqual,
    // Jumps
    Jump, JumpIfNot, Call, Return
  };
  union Arg
  {
    int intval;
    double realval;
    bool boolval;
    size_t addr;
    unsigned int atom;
  };

  Instruction(Opcode op, int intval=0)
   : opcode(op) { arg.intval = intval; } 
  Instruction(Opcode op, size_t addr)
   : opcode(op) { arg.addr = addr; } 
  Instruction(Opcode op, double realval)
   : opcode(op) { arg.realval = realval; } 
  Instruction(Opcode op, bool boolval)
   : opcode(op) { arg.boolval = boolval; } 
  Instruction(Opcode op, const Atom &atom)
   : opcode(op) { arg.atom = atom.id(); } 

  Opcode opcode;
  Arg arg;
};

#endif // INSTRUCTION_H
