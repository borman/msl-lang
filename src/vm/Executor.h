#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "Stack.h"
#include "Map.h"
#include "Program.h"
#include "StringTable.h"
#include "Value.h"
#include "Scope.h"
#include "ArrayStorage.h"

class AbstractBuiltin;

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

    struct Context
    {
      class BadType {};

      // Stack operations
      void push(const Value &v);
      Value pop();
      Value pop(Value::Type t);
      void popdelete();

      // Variable referencing
      Value getVar(unsigned int name);
      void setVar(unsigned int name, const Value &val);

      void openScope();
      void closeScope();

      Stack<Value> stack;
      Stack<Scope> scope;
      ArrayStorage arrays;
    };

    Executor(Program &program, StringTable *strings)
      : m_prog(program), m_strings(strings),
        m_pc(0), m_stopped(true) {}

    void addBuiltin(AbstractBuiltin *b);
    void run(unsigned int entryFun);

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

    Program &m_prog;
    StringTable *m_strings;
    size_t m_pc;
    bool m_stopped;
    Stack<size_t> m_callStack;
    Vector<AbstractBuiltin *> m_builtins;
    Context m_context;
};

#endif // EXECUTOR_H
