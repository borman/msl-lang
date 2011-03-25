#include <cstdio>
#include "ASTPrint.h"
#include "AST.h"
#include "Symbols.h"

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
    fprintf(dest, " ");
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

void AST::printBlock(FILE *dest, const char *name, Base *ast, unsigned int n_indent)
{
  indent(dest, n_indent);
  fprintf(dest, "(%s", name);
  for (Base *i = ast; i!=NULL; i=i->next<Base>())
  {
    fprintf(dest, "\n");
    printTree(dest, i, n_indent+1);
  }
  fprintf(dest, ")");
}

void AST::printTree(FILE *dest, Base *ast, unsigned int n_indent)
{
  indent(dest, n_indent);
  if (ast == NULL)
  {
    fprintf(dest, "<null>");
    return;
  }

  switch (ast->type())
  {
    case Base::Int:
      fprintf(dest, "%d", ast->as<Int>()->value());
      break;
    case Base::Real:
      fprintf(dest, "%lf", ast->as<Real>()->value());
      break;
    case Base::Bool:
      fprintf(dest, "%s", ast->as<Bool>()->value()? "true" : "false");
      break;
    case Base::Literal:
      fprintf(dest, "\"%s\"", ast->as<Literal>()->text().c_str());
      break;
    case Base::Variable:
      fprintf(dest, "'%s", ast->as<Variable>()->name().c_str());
      break;

    case Base::FuncCall:
      {
        FuncCall *func = ast->as<FuncCall>();
        fprintf(dest, "(call\n");
        indent(dest, n_indent+1);
        fprintf(dest, "'%s\n", func->name().c_str());
        printTree(dest, func->arg(), n_indent+1);
        fprintf(dest, ")");
      } break;

    case Base::ArrayItem:
      {
        ArrayItem *array = ast->as<ArrayItem>();
        fprintf(dest, "(array\n");
        indent(dest, n_indent+1);
        fprintf(dest, "'%s\n", array->name().c_str());
        printTree(dest, array->arg(), n_indent+1);
        fprintf(dest, ")");
      } break;

    case Base::Infix:
      {
        Infix *op = ast->as<Infix>();
        fprintf(dest, "(%s\n", infix(op->subtype()));
        printTree(dest, op->left(), n_indent+1);
        fprintf(dest, "\n");
        printTree(dest, op->right(), n_indent+1);
        fprintf(dest, ")");
      } break;

    case Base::Tuple:
      {
        Tuple *tuple = ast->as<Tuple>();
        fprintf(dest, "(tuple");
        if (tuple->contents() != NULL)
        {
          fprintf(dest, "\n");
          printBlock(dest, "tuple", tuple->contents(), n_indent+1);
        }
        fprintf(dest, ")");
      } break;

    case Base::Selector:
      {
        Selector *sel = ast->as<Selector>();
        fprintf(dest, "(if\n");
        printTree(dest, sel->condition(), n_indent+1);
        fprintf(dest, "\n");
        printTree(dest, sel->positive(), n_indent+1);
        fprintf(dest, "\n");
        printTree(dest, sel->negative(), n_indent+1);
        fprintf(dest, ")");
      } break;
    
    case Base::Do:
      fprintf(dest, "(do\n");
      printTree(dest, ast->as<Do>()->expr(), n_indent+1);
      fprintf(dest, ")");
      break;

    case Base::Return:
      fprintf(dest, "(return\n");
      printTree(dest, ast->as<Return>()->expr(), n_indent+1);
      fprintf(dest, ")");
      break;

    case Base::Let:
      {
        Let *let = ast->as<Let>();
        fprintf(dest, "(let\n");
        printTree(dest, let->lvalue(), n_indent+1);
        fprintf(dest, "\n");
        printTree(dest, let->rvalue(), n_indent+1);
        fprintf(dest, ")");
      } break;

    case Base::If:
      {
        If *sel = ast->as<If>();
        fprintf(dest, "(if\n");
        printTree(dest, sel->condition(), n_indent+1);
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
        fprintf(dest, "(if\n");
        printTree(dest, sel->condition(), n_indent+1);
        fprintf(dest, "\n");
        printBlock(dest, "block", sel->body(), n_indent+1);
        fprintf(dest, ")");
      } break;

    case Base::For:
      {
        For *sel = ast->as<For>();
        fprintf(dest, "(for\n");
        printTree(dest, sel->var(), n_indent+1);
        fprintf(dest, "\n");
        printTree(dest, sel->from(), n_indent+1);
        fprintf(dest, "\n");
        printTree(dest, sel->to(), n_indent+1);
        fprintf(dest, "\n");
        printBlock(dest, "block", sel->body(), n_indent+1);
        fprintf(dest, ")");
      } break;

    case Base::Fun:
      {
        Fun *fun = ast->as<Fun>();
        fprintf(dest, "(fun %s\n", fun->name().c_str());
        printTree(dest, fun->arg(), n_indent+1);
        fprintf(dest, "\n");
        printBlock(dest, "block", fun->body(), n_indent+1);
        fprintf(dest, ")");
      } break;

    default:
      break;
  }
}
