#include <cstdio>
#include "ASTPrint.h"
#include "AST.h"
#include "Symbols.h"

// #define PRINT_REGIONS_IN_AST

using namespace AST;

static const char *infix(Infix::Subtype t)
{
  switch (t)
  {
    case Infix::Equals: return "=";
    case Infix::Less: return "<";
    case Infix::Greater: return ">";
    case Infix::Plus: return "+";
    case Infix::Minus: return "-";
    case Infix::Mul: return "*";
    case Infix::Div: return "/";
    case Infix::Mod: return "mod";
    case Infix::And: return "and";
    case Infix::Or: return "or";
    default: return NULL;
  }
}

static void indent(FILE *dest, unsigned int n)
{
  for (unsigned int i=0; i<n; i++)
    fprintf(dest, "  ");
} 

void AST::printTokens(FILE *dest, Base *tokens)
{
  for(Base *t = tokens; t != NULL; t = t->next<Base>())
  {
    const TextRegion &r = t->region();
    fprintf(dest, "<%2u:%2u - %2u:%2u>\t", 
           r.startRow, r.startCol,
           r.endRow, r.endCol);
    switch (t->type())
    {
      case Base::Symbol:
        fprintf(dest, "SYM  %s", Symbols::name(t->as<Symbol>()->subtype()));
        break;

      case Base::Bool:
        fprintf(dest, "BOOL %s", t->as<Bool>()->value()? "TRUE": "FALSE");
        break;

      case Base::Int:
        fprintf(dest, "INT  %d", t->as<Int>()->value());
        break;

      case Base::Real:
        fprintf(dest, "REAL %lf", t->as<Real>()->value());
        break;

      case Base::Literal:
        fprintf(dest, "LIT  %s", t->as<Literal>()->text().c_str());
        break;

      case Base::FuncCall:
        fprintf(dest, "FUN  %s", t->as<FuncCall>()->name().c_str());
        break;
      
      case Base::Variable:
        fprintf(dest, "VAR  %s", t->as<Variable>()->name().c_str());
        break;

      case Base::ArrayItem:
        fprintf(dest, "ARR  %s", t->as<ArrayItem>()->name().c_str());
        break;

      case Base::Infix:
        fprintf(dest, "INFX %s", infix(t->as<Infix>()->subtype()));
        break;

      default:
        fprintf(dest, "UNK");
    }
    fprintf(dest, "\n");
  }
}

void AST::printBlock(FILE *dest, const char *name, Base *ast, 
    unsigned int n_indent, bool one_line)
{
  int d = 0;
  if (name != NULL)
  {
    indent(dest, n_indent);
    fprintf(dest, "(%s", name);
    d = 1;
  }
  for (Base *i = ast; i!=NULL; i=i->next<Base>())
  {
    if (!(name == NULL && i==ast))
      fprintf(dest, one_line? " ": "\n");
    printTree(dest, i, n_indent+d, !one_line);
  }
  if (name != NULL)
    fprintf(dest, ")");
}

void AST::printTree(FILE *dest, Base *ast, unsigned int n_indent, bool do_indent)
{
  if (do_indent)
    indent(dest, n_indent);

  if (ast == NULL)
  {
    fprintf(dest, "<null>");
    return;
  }

#ifdef PRINT_REGIONS_IN_AST
  char pos[32];
  const TextRegion &r = ast->region();
  snprintf(pos, 32, "<%u:%u-%u:%u> ", 
      r.startRow, r.startCol, r.endRow, r.endCol);
#else
  static const char *pos = "";
#endif

  switch (ast->type())
  {
    case Base::Int:
      fprintf(dest, "%s%d", pos, ast->as<Int>()->value());
      break;
    case Base::Real:
      fprintf(dest, "%s%lf", pos, ast->as<Real>()->value());
      break;
    case Base::Bool:
      fprintf(dest, "%s%s", pos, ast->as<Bool>()->value()? "true" : "false");
      break;
    case Base::Literal:
      fprintf(dest, "%s\"%s\"", pos, ast->as<Literal>()->text().c_str());
      break;
    case Base::Variable:
      fprintf(dest, "%s'%s", pos, ast->as<Variable>()->name().c_str());
      break;

    case Base::FuncCall:
      {
        FuncCall *func = ast->as<FuncCall>();
        fprintf(dest, "(%scall '%s ", pos, func->name().c_str());
        printTree(dest, func->arg(), n_indent+1, false);
        fprintf(dest, ")");
      } break;

    case Base::ArrayItem:
      {
        ArrayItem *array = ast->as<ArrayItem>();
        fprintf(dest, "(%sarray '%s ", pos, array->name().c_str());
        printTree(dest, array->arg(), n_indent+1, false);
        fprintf(dest, ")");
      } break;

    case Base::Infix:
      {
        Infix *op = ast->as<Infix>();
        fprintf(dest, "(%s%s ", pos, infix(op->subtype()));
        printTree(dest, op->left(), n_indent+1, false);
        fprintf(dest, " ");
        printTree(dest, op->right(), n_indent+1, false);
        fprintf(dest, ")");
      } break;

    case Base::Tuple:
      {
        Tuple *tuple = ast->as<Tuple>();
        fprintf(dest, "(%s", pos);
        if (tuple->contents() != NULL)
          printBlock(dest, NULL, tuple->contents(), n_indent+1, true);
        fprintf(dest, ")");
      } break;

    case Base::Selector:
      {
        Selector *sel = ast->as<Selector>();
        fprintf(dest, "(%sif ", pos);
        printTree(dest, sel->condition(), n_indent+1, false);
        fprintf(dest, " ");
        printTree(dest, sel->positive(), n_indent+1, false);
        fprintf(dest, " ");
        printTree(dest, sel->negative(), n_indent+1, false);
        fprintf(dest, ")");
      } break;
    
    case Base::Do:
      fprintf(dest, "(%sdo ", pos);
      printTree(dest, ast->as<Do>()->expr(), n_indent+1, false);
      fprintf(dest, ")");
      break;

    case Base::Return:
      fprintf(dest, "(%sreturn ", pos);
      printTree(dest, ast->as<Return>()->expr(), n_indent+1, false);
      fprintf(dest, ")");
      break;

    case Base::Let:
      {
        Let *let = ast->as<Let>();
        fprintf(dest, "(%slet ", pos);
        printTree(dest, let->lvalue(), n_indent+1, false);
        fprintf(dest, " ");
        printTree(dest, let->rvalue(), n_indent+1, false);
        fprintf(dest, ")");
      } break;

    case Base::If:
      {
        If *sel = ast->as<If>();
        fprintf(dest, "(%sif ", pos);
        printTree(dest, sel->condition(), n_indent+1, false);
        fprintf(dest, "\n");
        printBlock(dest, "block", sel->positive(), n_indent+1);
        if (sel->negative() != NULL)
        {
          fprintf(dest, "\n");
          printBlock(dest, "block", sel->negative(), n_indent+1);
        }
        fprintf(dest, ")");
      } break;

    case Base::While:
      {
        While *sel = ast->as<While>();
        fprintf(dest, "(%swhile ", pos);
        printTree(dest, sel->condition(), n_indent+1, false);
        fprintf(dest, "\n");
        printBlock(dest, "block", sel->body(), n_indent+1);
        fprintf(dest, ")");
      } break;

    case Base::For:
      {
        For *sel = ast->as<For>();
        fprintf(dest, "(%sfor ", pos);
        printTree(dest, sel->var(), n_indent+1, false);
        fprintf(dest, " ");
        printTree(dest, sel->from(), n_indent+1, false);
        fprintf(dest, " ");
        printTree(dest, sel->to(), n_indent+1, false);
        fprintf(dest, "\n");
        printBlock(dest, "block", sel->body(), n_indent+1);
        fprintf(dest, ")");
      } break;

    case Base::Fun:
      {
        Fun *fun = ast->as<Fun>();
        fprintf(dest, "(%sfun '%s ", pos, fun->name().c_str());
        printTree(dest, fun->arg(), n_indent+1, false);
        fprintf(dest, "\n");
        printBlock(dest, "block", fun->body(), n_indent+1);
        fprintf(dest, ")");
      } break;

    default:
      break;
  }
}

// ========= Linear code =========

static void printInstr(FILE *dest, size_t addr, const Instruction &instr, StringTable *strings)
{
  switch (instr.opcode)
  {
#define INSTR(opcode) case Instruction::opcode: \
    fprintf(dest, "%04zu: %-20s\n", addr, #opcode); break

#define INSTR_G(opcode, fmt, val) case Instruction::opcode: \
    fprintf(dest, "%04zu: %-20s"fmt"\n", addr, #opcode, val); break

#define INSTR_A(opcode) case Instruction::opcode: \
    fprintf(dest, "%04zu: %-20s%s\n", addr, #opcode, strings->str(instr.arg.atom)); break

    INSTR_A(PushVar);
    INSTR_G(PushInt, "%d", instr.arg.intval);
    INSTR_G(PushReal, "%lf", instr.arg.realval);
    INSTR_G(PushBool, "%s", (instr.arg.boolval?"TRUE":"FALSE"));
    INSTR_A(PushLiteral);
    INSTR_A(PushArrayItem);
    INSTR_A(PopVar);
    INSTR_A(PopArrayItem);
    INSTR(PopDelete);
    INSTR(TupOpen);
    INSTR(TupClose);
    INSTR(TupUnOpen);
    INSTR(TupUnClose);
    INSTR(Add);
    INSTR(Sub);
    INSTR(Mul);
    INSTR(Div);
    INSTR(Mod);
    INSTR(And);
    INSTR(Or);
    INSTR(TestLess);
    INSTR(TestGreater);
    INSTR(TestEqual);
    INSTR_G(Jump, "@%04zu", instr.arg.addr);
    INSTR_G(JumpIfNot, "@%04zu", instr.arg.addr);
    INSTR_A(Call);
    INSTR(Return);
    INSTR(Trap);
#undef INSTR
#undef INSTR_G
#undef INSTR_A
  }
}

void AST::printCode(FILE *dest, const Program &prog, StringTable *strings)
{
  for (size_t i=0; i<prog.size(); i++)
    printInstr(dest, i, prog[i], strings);
}
