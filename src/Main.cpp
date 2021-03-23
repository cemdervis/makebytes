#include "Arguments.h"
#include "Generator.h"

#include <iostream>
#include <stdexcept>

#if defined(WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

static const std::string_view g_AppName = "makebytes";

static inline void PrintUsage()
{
  std::cout << "Usage: " << g_AppName << " <options> <input_file>" << std::endl;
  std::cout << "Options: " << std::endl;

  for (const auto opt : {
           std::make_pair("c=<var;file>", "Generate a C source file."),
           std::make_pair("cpp=<var;file>", "Generate a C++ source file."),
           std::make_pair("csharp=<namespace:var;file>",
                          "Generate a C# source file."),
           std::make_pair("java=<namespace:var;file>",
                          "Generate a Java source file."),
           std::make_pair("python=<var;file>",
                          "Generate a Python source file."),
       })
  {
    std::cout << "  " << opt.first << " -- " << opt.second << std::endl;
  }
}

int main(int argc, char* argv[])
{
  if (argc <= 1)
  {
    PrintUsage();
    return 0;
  }

  try
  {
    const Arguments args{argc, argv};
    Generator       gen{args};
    gen.Generate();
    return 0;
  }
  catch (std::exception& ex)
  {
    std::string msg = "error: ";
    msg += ex.what();

    std::cerr << msg << std::endl;

#if defined(WIN32) || defined(_WIN64)
    msg += '\n';
    OutputDebugStringA(msg.c_str());
#endif

    return 1;
  }
}
