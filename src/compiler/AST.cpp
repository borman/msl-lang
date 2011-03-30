#include "AST.h"

using namespace AST;
 
static void deleteSubtree(Base *root);

void AST::deleteChain(Base *root)
{
  while (root != NULL)
  {
    Base *head = root;
    root = root->next<Base>();
    deleteSubtree(head);
  }
}

static void deleteSubtree(Base *root)
{
  if (root == NULL)
    return;
  switch (root->type())
  {
    case Base::Int:
    case Base::Real:
    case Base::Bool:
    case Base::Literal:
    case Base::Variable:
    case Base::Token:
    case Base::Symbol:
    case Base::None:
      break;

    case Base::FuncCall:
      deleteChain(root->as<FuncCall>()->arg());
      break;

    case Base::ArrayItem:
      deleteChain(root->as<ArrayItem>()->arg());
      break;

    case Base::Infix:
      deleteChain(root->as<Infix>()->left());
      deleteChain(root->as<Infix>()->right());
      break;

    case Base::Tuple:
      deleteChain(root->as<Tuple>()->contents());
      break;

    case Base::Selector:
      deleteChain(root->as<Selector>()->condition());
      deleteChain(root->as<Selector>()->positive());
      deleteChain(root->as<Selector>()->negative());
      break;
    
    case Base::Do:
      deleteChain(root->as<Do>()->expr());
      break;

    case Base::Return:
      deleteChain(root->as<Return>()->expr());
      break;

    case Base::Let:
      deleteChain(root->as<Let>()->lvalue());
      deleteChain(root->as<Let>()->rvalue());
      break;

    case Base::If:
      deleteChain(root->as<Selector>()->condition());
      deleteChain(root->as<If>()->positive());
      deleteChain(root->as<If>()->negative());
      break;

    case Base::While:
      deleteChain(root->as<While>()->condition());
      deleteChain(root->as<While>()->body());
      break;

    case Base::For:
      deleteChain(root->as<For>()->var());
      deleteChain(root->as<For>()->from());
      deleteChain(root->as<For>()->to());
      deleteChain(root->as<For>()->body());
      break;

    case Base::Fun:
      deleteChain(root->as<Fun>()->arg());
      deleteChain(root->as<Fun>()->body());
      break;
  }
  delete root;
}

