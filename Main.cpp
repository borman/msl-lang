#include <cstdio>
#include "Tokenizer.h"
#include "Lexer.h"
#include "Parser.h"
#include "Symbols.h"
#include "ASTExpr.h"

using namespace AST;


static void printTokens(Base *tokens);
static void printAST(AST::Base *ast, unsigned int n_indent = 0);
static void printASTBlock(const char *name, Base *ast, unsigned int n_indent = 0);

int main()
{
  Tokenizer tokenizer;
  Lexer lexer;
  Parser parser;
  
  try
  {
    int c;
    while ((c = getchar()) != EOF)
      tokenizer.feed(c);
    tokenizer.feed('\n');
    lexer.feed(tokenizer.peek());
    Base *tokens = lexer.peek();
    printTokens(tokens);
    parser.feed(tokens);
    Fun *funs = parser.peek();
    printASTBlock("program", funs);
    printf("\n");
  }
  catch (const Tokenizer::Exception &e)
  {
    printf("stdin:%u:%u: Tokenizer error: %s\n", e.row()+1, e.col()+1, e.text().c_str());
    return 1;
  }
  catch (const Lexer::Exception &e)
  {
    const TextRegion &r = e.region();
    printf("stdin:%u:%u-%u-%u: Lexer error: %s in \"%s\"\n",
           r.startRow, r.startCol, r.endRow, r.endCol,
           e.text().c_str(), e.token().c_str());
    return 1;
  }
  catch (const Parser::Exception &e)
  {
    printf("Parser error: %s\n", e.text().c_str());
    printTokens(e.tokens());
    return 1;
  }

  return 0;
}


static const char *infix(Infix::Subtype t)
{
  switch (t)
  {
    case Infix::Mul: return "*";
    case Infix::Div: return "/";
    case Infix::Mod: return "mod";
    case Infix::Plus: return "+";
    case Infix::Minus: return "-";
    case Infix::Equals: return "=";
    case Infix::Less: return "<";
    case Infix::Greater: return ">";
    case Infix::And: return "and";
    case Infix::Or: return "or";
  }
}

static void printTokens(Base *tokens)
{
  for(Base *t = tokens; t != NULL; t = t->next<Base>())
  {
    const TextRegion &r = t->region();
    printf("<%2u:%2u - %2u:%2u>\t", 
           r.startRow, r.startCol,
           r.endRow, r.endCol);
    switch (t->type())
    {
      case Base::Token:
        if (t->as<Token>()->isLiteral())
        printf("TOK%c %s", 
            t->as<Token>()->isLiteral()? 'L':' ',
            t->as<Token>()->text().c_str());
        break;

      case Base::Symbol:
        printf("SYM  %s", Symbols::name(t->as<Symbol>()->subtype()));
        break;

      case Base::Bool:
        printf("BOOL %s", t->as<Bool>()->value()? "TRUE": "FALSE");
        break;

      case Base::Int:
        printf("INT  %d", t->as<Int>()->value());
        break;

      case Base::Real:
        printf("REAL %lf", t->as<Real>()->value());
        break;

      case Base::Literal:
        printf("LIT  %s", t->as<Literal>()->text().c_str());
        break;

      case Base::FuncCall:
        printf("FUN  %s", t->as<FuncCall>()->name().c_str());
        break;
      
      case Base::Variable:
        printf("VAR  %s", t->as<Variable>()->name().c_str());
        break;

      case Base::ArrayItem:
        printf("ARR  %s", t->as<ArrayItem>()->name().c_str());
        break;

      case Base::Infix:
        printf("INFX %s", infix(t->as<Infix>()->subtype()));
        break;

      default:
        printf("UNK");
    }
    printf("\n");
  }
}

static void indent(unsigned int n)
{
  for (unsigned int i=0; i<n; i++)
    printf("  ");
} 

static void printASTBlock(const char *name, Base *ast, unsigned int n_indent)
{
  indent(n_indent);
  printf("(%s", name);
  for (Base *i = ast; i!=NULL; i=i->next<Base>())
  {
    printf("\n");
    printAST(i, n_indent+1);
  }
  printf(")");
}

static void printAST(Base *ast, unsigned int n_indent)
{
  indent(n_indent);
  if (ast == NULL)
  {
    printf("<null>");
    return;
  }

  switch (ast->type())
  {
    case Base::Int:
      printf("%d", ast->as<Int>()->value());
      break;
    case Base::Real:
      printf("%lf", ast->as<Real>()->value());
      break;
    case Base::Bool:
      printf("%s", ast->as<Bool>()->value()? "true" : "false");
      break;
    case Base::Literal:
      printf("\"%s\"", ast->as<Literal>()->text().c_str());
      break;
    case Base::Variable:
      printf("%s", ast->as<Variable>()->name().c_str());
      break;

    case Base::FuncCall:
      {
        FuncCall *func = ast->as<FuncCall>();
        printf("(call\n");
        indent(n_indent+1);
        printf("%s\n", func->name().c_str());
        printAST(func->arg(), n_indent+1);
        printf(")");
      } break;

    case Base::ArrayItem:
      {
        ArrayItem *array = ast->as<ArrayItem>();
        printf("(array\n");
        indent(n_indent+1);
        printf("%s\n", array->name().c_str());
        printAST(array->arg(), n_indent+1);
        printf(")");
      } break;

    case Base::Infix:
      {
        Infix *op = ast->as<Infix>();
        printf("(%s\n", infix(op->subtype()));
        printAST(op->left(), n_indent+1);
        printf("\n");
        printAST(op->right(), n_indent+1);
        printf(")");
      } break;

    case Base::Tuple:
      {
        Tuple *tuple = ast->as<Tuple>();
        printf("(tuple");
        if (tuple->contents() != NULL)
        {
          printf("\n");
          printASTBlock("tuple", tuple->contents(), n_indent+1);
        }
        printf(")");
      } break;

    case Base::Selector:
      {
        Selector *sel = ast->as<Selector>();
        printf("(if\n");
        printAST(sel->condition(), n_indent+1);
        printf("\n");
        printAST(sel->positive(), n_indent+1);
        printf("\n");
        printAST(sel->negative(), n_indent+1);
        printf(")");
      } break;
    
    case Base::Do:
      printf("(do\n");
      printAST(ast->as<Do>()->expr(), n_indent+1);
      printf(")");
      break;

    case Base::Let:
      {
        Let *let = ast->as<Let>();
        printf("(let\n");
        printAST(let->lvalue(), n_indent+1);
        printf("\n");
        printAST(let->rvalue(), n_indent+1);
        printf(")");
      } break;

    case Base::If:
      {
        If *sel = ast->as<If>();
        printf("(if\n");
        printAST(sel->condition(), n_indent+1);
        printf("\n");
        printASTBlock("block", sel->positive(), n_indent+1);
        if (sel->negative() != NULL)
        {
          printf("\n");
          printASTBlock("block", sel->negative(), n_indent+1);
        }
        printf(")");
      } break;

    case Base::While:
      {
        While *sel = ast->as<While>();
        printf("(if\n");
        printAST(sel->condition(), n_indent+1);
        printf("\n");
        printASTBlock("block", sel->body(), n_indent+1);
        printf(")");
      } break;

    case Base::For:
      {
        For *sel = ast->as<For>();
        printf("(for\n");
        printAST(sel->var(), n_indent+1);
        printf("\n");
        printAST(sel->from(), n_indent+1);
        printf("\n");
        printAST(sel->to(), n_indent+1);
        printf("\n");
        printASTBlock("block", sel->body(), n_indent+1);
        printf(")");
      } break;

    case Base::Fun:
      {
        Fun *fun = ast->as<Fun>();
        printf("(fun %s\n", fun->name().c_str());
        printAST(fun->arg(), n_indent+1);
        printf("\n");
        printASTBlock("block", fun->body(), n_indent+1);
        printf(")");
      } break;

    default:
      break;
  }
}
