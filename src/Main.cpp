#include <cstring>
#include "LoadedProgram.h"
#include "Executor.h"
#include "BasicBuiltin.h"
#include "File.h"

using namespace AST;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    cout.printf("Usage: %s <file.msl>\n", argv[0]);
    return 1;
  }

  const char *filename = argv[1];
  try
  {
    LoadedProgram program(filename);

    BasicBuiltin builtins(program.strings());

    Executor executor(program, program.strings());
    executor.addBuiltin(&builtins);
    executor.run("main");
  }
  catch (const File::Exception &e)
  {
    cout.printf("%s: %s\n", filename, strerror(e.code()));
  }
  catch (const LoadedProgram::Exception &e)
  {
    cout.printf("%s:%s\n", filename, e.text());
    return 1;
  }
  catch (const Executor::Exception &e)
  {
    cout.printf("Executor error: %s at %04zu\n", e.text(), e.addr());
    return 1;
  }

  return 0;
}


