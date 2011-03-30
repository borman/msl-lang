#ifndef PROGRAM_H
#define PROGRAM_H

#include "Vector.h"
#include "Instruction.h"

class Program
{
  public:
    struct EntryPoint
    {
      EntryPoint(const Atom &n=Atom(), size_t a=0)
        : name(n), addr(a) {}
      Atom name;
      size_t addr;
    };

    size_t write(const Instruction &instr)
    {
      m_instrs.push_back(instr);
      return m_instrs.size()-1;
    }

    size_t size() const { return m_instrs.size(); }
    size_t nextAddr() const { return m_instrs.size(); }

    Instruction &operator[](size_t addr) { return m_instrs[addr]; } 
    const Instruction &operator[](size_t addr) const { return m_instrs[addr]; }

    size_t entryCount() const { return m_entries.size(); }
    const EntryPoint entry(size_t i) const { return m_entries[i]; }
    void addEntry(const EntryPoint &e) { m_entries.push_back(e); }
  private:
    Vector<Instruction> m_instrs;
    Vector<EntryPoint> m_entries;
};

#endif // PROGRAM_H
