#ifndef PROGRAM_H
#define PROGRAM_H

#include <vector>
#include "Instruction.h"

class Program
{
  public:
    size_t write(const Instruction &instr)
    {
      m_instrs.push_back(instr);
      return m_instrs.size()-1;
    }

    size_t size() const { return m_instrs.size(); }
    size_t nextAddr() const { return m_instrs.size(); }

    Instruction &operator[](size_t addr) { return m_instrs[addr]; } 
    const Instruction &operator[](size_t addr) const { return m_instrs[addr]; }
  private:
    std::vector<Instruction> m_instrs;
};

#endif // PROGRAM_H
