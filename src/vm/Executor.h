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
        Exception(const char *text, size_t addr)
          : m_text(text), m_addr(addr) {}

        const char *text() const { return m_text; }
        size_t addr() const { return m_addr; }

        private:
          const char *m_text;
          size_t m_addr;
    };
    class Undefined: public Exception
    {
      public:
        enum Type { Function, Variable };

        Undefined(Type t, const Atom &name, size_t addr)
          : Exception("Undefined", addr), 
            m_type(t), m_name(name) {}
        Type type() const { return m_type; }
        Atom name() const { return m_name; }
      private:
        Type m_type;
        Atom m_name;
    };
    class BadType: public Exception
    {
      public:
        BadType(Value::Type expected, Value::Type found, size_t addr)
          : Exception("Types incompatible", addr),
            m_expected(expected), m_found(found) {}
        Value::Type expected() const { return m_expected; }
        Value::Type found() const { return m_found; }
      private:
        Value::Type m_expected;
        Value::Type m_found;
    };

    Executor(Program &program, StringTable *strings);

    void addBuiltin(AbstractBuiltin *b);
    void run(StringTable::Ref entryFun);
    void run(const char *entryName);

  private:
    // Branching
    void call(StringTable::Ref name, bool saveRet=true);
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
