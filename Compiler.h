#ifndef COMPILER_H
#define COMPILER_H

#include "AST.h"

class Compiler
{
  public:
    Compiler(FILE *out): m_counter(0), m_out(out) {}

    void feed(AST::Fun *funs);
  private:
    void compileFun(AST::Fun *fun);

    void compileBlock(AST::Operator *block);
    void compileOperator(AST::Operator *op);
    
    // Operators
    void compileDo(AST::Do *ast);
    void compileLet(AST::Let *ast);
    void compileIf(AST::If *ast);
    void compileWhile(AST::While *ast);
    void compileFor(AST::For *ast);

    // Push-Expressions 
    void compilePushExpr(AST::Expression *expr);
    void compilePushInt(AST::Int *expr);
    void compilePushReal(AST::Real *expr);
    void compilePushBool(AST::Bool *expr);
    void compilePushLiteral(AST::Literal *expr);
    void compilePushVariable(AST::Variable *expr);
    void compilePushFuncCall(AST::FuncCall *expr);
    void compilePushArrayItem(AST::ArrayItem *expr);
    void compilePushTuple(AST::Tuple *expr);
    void compilePushSelector(AST::Selector *expr);
    void compilePushInfix(AST::Infix *expr);
    
    // Pop-Expressions
    void compilePopExpr(AST::Expression *expr);
    void compilePopVariable(AST::Variable *expr);
    void compilePopArrayItem(AST::ArrayItem *expr);
    void compilePopTuple(AST::Tuple *expr);

    unsigned int m_counter;
    FILE *m_out;
};

#endif // COMPILER_H

