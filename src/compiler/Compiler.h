#ifndef COMPILER_H
#define COMPILER_H

#include "AST.h"
#include "Program.h"

class Compiler
{
  public:
    Compiler(Program &prog)
      : m_prog(prog) {}

    void compile(AST::Fun *funs);
    Program &program() { return m_prog; }
  private:
    void compileFun(AST::Fun *fun);

    void compileBlock(AST::Operator *block);
    void compileOperator(AST::Operator *op);
    
    // Operators
    void compileDo(AST::Do *ast);
    void compileReturn(AST::Return *ast);
    void compileLet(AST::Let *ast);
    void compileIf(AST::If *ast);
    void compileWhile(AST::While *ast);
    void compileFor(AST::For *ast);

    // Push-Expressions 
    void compilePush(AST::Expression *expr);
    void compilePush(AST::Int *expr);
    void compilePush(AST::Real *expr);
    void compilePush(AST::Bool *expr);
    void compilePush(AST::Literal *expr);
    void compilePush(AST::Variable *expr);
    void compilePush(AST::FuncCall *expr);
    void compilePush(AST::ArrayItem *expr);
    void compilePush(AST::Tuple *expr);
    void compilePush(AST::Selector *expr);
    void compilePush(AST::Infix *expr);
    
    // Pop-Expressions
    void compilePop(AST::Expression *expr);
    void compilePop(AST::Variable *expr);
    void compilePop(AST::ArrayItem *expr);
    void compilePop(AST::Tuple *expr);

    
    template<class T> 
    size_t emit(Instruction::Opcode opcode, T arg) { return m_prog.write(Instruction(opcode, arg)); }
    size_t emit(Instruction::Opcode opcode) { return m_prog.write(Instruction(opcode)); }

    Program &m_prog; 
};

#endif // COMPILER_H

