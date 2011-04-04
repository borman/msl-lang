#ifndef FILE_H
#define FILE_H

#include <cstdio>

// FIXME: Exceptions?
class File
{
  public:
    enum Mode
    {
      Read, Write
    };
    class Exception
    {
      public:
        Exception(int code)
          : m_code(code) {}
        int code() const { return m_code; }
      private:
        int m_code;
    };

    File(const char *name, Mode mode);
    File(FILE *exfile);
    ~File();

    int printf(const char *format, ...);
    int getc();

    void close();

  private:
    FILE *m_file;
    bool m_own;
};

extern File cout;
extern File cerr;
extern File cin;

#endif // FILE_H

