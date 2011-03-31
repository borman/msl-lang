#ifndef EXECUTOR_H
#define EXECUTOR_h

#include "Stack.h"
#include "Map.h"
#include "Program.h"
#include "StringTable.h"
#include "Value.h"
#include "Scope.h"

class Executor
{
  public:
    class Exception
    {
      public:
        enum Type
        {
          BadType,
          Trap,
          UndefVar,
          UndefFun
        };

        Exception(Type type, size_t addr)
          : m_type(type), m_addr(addr) {}

        Type type() const { return m_type; }
        size_t addr() const { return m_addr; }

        private:
          Type m_type;
          size_t m_addr;
    };

    Executor(Program &program, StringTable *strings)
      : m_prog(program), m_strings(strings),
        m_pc(0), m_stopped(true) {}

    void run(unsigned int entryFun);

  private:
    void push(const Value &v);
    Value pop();
    Value pop(Value::Type t);
    void popdelete();

    Value getVariable(unsigned int name);
    void setVariable(unsigned int name, const Value &val);

    void call(unsigned int name, bool saveRet=true);
    void jump(size_t addr);
    void ret();

    void trap();
    void badType();

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
    Stack<Value> m_valStack;
    Stack<Scope> m_scope;
};

#endif // EXECUTOR_H
