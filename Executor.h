#ifndef EXECUTOR_H
#define EXECUTOR_h

#include <stack>
#include <map>
#include "Program.h"

class Executor
{
  public:
    struct Value
    {
      enum Type
      {
        TupOpen, TupClose,
        Int, Real, Bool, String
      };
      union Data
      {
        int intval;
        double realval;
        bool boolval;
        unsigned int strval;
      };

      Value(int i = 0):          type(Int)    { d.intval = i; }
      Value(double r):       type(Real)   { d.realval = r; }
      Value(bool b):         type(Bool)   { d.boolval = b; }
      Value(unsigned int s): type(String) { d.strval = s; }
      Value(Type t):         type(t)      {}

      Type type;
      Data d;
    };
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

    Executor(Program &program)
      : m_prog(program), m_pc(0) {}

    void run(unsigned int entryFun);

  private:
    void push(const Value &v);
    Value pop();
    Value pop(Value::Type t);
    void popdelete();

    Value variable(unsigned int name);
    void setVariable(unsigned int name, const Value &val);

    void call(unsigned int name, bool saveRet=true);
    void jump(size_t addr);
    void ret();

    void trap();
    void badType();

    void exec(const Instruction &instr);
    void step();

    Program &m_prog;
    size_t m_pc;
    bool m_stopped;
    std::stack<size_t> m_callStack;
    std::stack<Value> m_valstack;
    std::map<unsigned int, Value> m_vars;
};

#endif // EXECUTOR_H
