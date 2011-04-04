#ifndef LOADEDPROGRAM_H
#define LOADEDPROGRAM_H

#include "Program.h"
#include "DataSource.h"
#include "String.h"
#include "StringTable.h"

class LoadedProgram: public Program
{
  public:
    class Exception
    {
      public:
        Exception(const char *text)
          : m_text(text) {}
        const char *text() const { return m_text.c_str(); }
      private:
        String m_text;
    };

    LoadedProgram(DataSource<int> &src);
    // Convenience: load from file
    LoadedProgram(const char *file);

    const StringTable *strings() const { return &m_strings; }
    StringTable *strings() { return &m_strings; }
  private:
    void load(DataSource<int> &src);
    void error(const char *format, ...);
    
    StringTable m_strings;
};

#endif // LOADEDPROGRAM_H


