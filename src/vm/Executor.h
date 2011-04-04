#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "Stack.h"
#include "Map.h"
#include "Program.h"
#include "StringTable.h"
#include "Value.h"
#include "Context.h"
#include "Builtin.h"

/**
 * A linear code executor.
 *
 * Implements a stack machine.
 */
class Executor
{
  public:
    class Exception
    {
      public:
        enum Type { BadType, Trap, UndefVar, UndefFun };

        Exception(Type type, size_t addr)
          : m_type(type), m_addr(addr) {}

        Type type() const { return m_type; }
        size_t addr() const { return m_addr; }

        private:
          Type m_type;
          size_t m_addr;
    };

    Executor(Program &program, StringTable *strings);

    void addBuiltin(AbstractBuiltin *b);
    void run(unsigned int entryFun);
    void run(const char *entryName);

  private:
    // Branching
    void call(unsigned int name, bool saveRet=true);
    void jump(size_t addr);
    void ret();

    // Exceptions
    void trap();
    void badType();

    // One-cycle execution
    void exec(const Instruction &instr);
    Value execPush(const Instruction &instr);
    Value execBinOp(const Instruction &instr, 
        const Value &left, const Value &right);
    void step();

    // Data
    Program &m_prog;
    size_t m_pc;
    bool m_stopped;
    Stack<size_t> m_callStack;
    Vector<AbstractBuiltin *> m_builtins;
    Context m_context;
};

#endif // EXECUTOR_H
