#include "Generator.h"

#include "Arguments.h"
#include "TextWriter.h"

#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>

using std::filesystem::path;

static const std::string_view coutName = "cout";

Generator::Generator(const Arguments& args)
    : m_Args(args)
{
  ReadInputFile();
}

void Generator::Generate()
{
  if (m_Args.Has("c"))
    GenerateC();

  if (m_Args.Has("cpp"))
    GenerateCpp();

  if (m_Args.Has("csharp"))
    GenerateCSharp();

  if (m_Args.Has("java"))
    GenerateJava();

  if (m_Args.Has("python"))
    GeneratePython();
}

void Generator::ReadInputFile()
{
  const auto& entry         = m_Args[m_Args.GetCount() - 1u];
  const auto  inputFilename = path(entry.first);

  if (!std::filesystem::exists(inputFilename))
  {
    std::string msg = "Failed to open file \"";
    msg += inputFilename.u8string();
    msg += "\" for reading: the file does not exist.";
    throw std::runtime_error(msg);
  }

  std::ifstream ifs{inputFilename, std::ios::binary | std::ios::ate};

  if (!ifs)
  {
    std::string msg = "Failed to open file \"";
    msg += inputFilename.u8string();
    msg += "\" for reading.";
    throw std::runtime_error(msg);
  }

  const size_t fileSize = size_t(ifs.tellg());
  ifs.seekg(0, std::ios::beg);

  m_Bytes.resize(fileSize);

  ifs.read(reinterpret_cast<char*>(m_Bytes.data()), std::streamsize(fileSize));
}

static inline void WriteDataAsBytes(TextWriter& w, const std::vector<uint8_t>& bytes,
                                    bool isJava)
{
  char buff[8];

  for (size_t i = 0u; i < bytes.size(); ++i)
  {
    const int  byte = int(bytes[i]);
    const auto res  = std::to_chars(std::begin(buff), std::end(buff), byte, 16);

    const size_t length = size_t(std::distance(buff, res.ptr));

    // Java bytes support the range [-128 .. +127], but our bytes are [0 .. +255].
    // Appropriately cast so that the Java compiler interprets it as an unsigned byte.
    if (isJava && byte > 127)
      w << "(byte)";

    w << "0x" << std::string_view(buff, length) << ", ";

    if (i > 0u && (i % 20) == 0)
      w << '\n';
  }

  w << '\n';
}

class LangInput
{
public:
  std::string_view Namespace;
  std::string_view VarName;
  path             OutputFilename;
  bool             IsCout;
};

static inline LangInput GetLangInput(std::string_view value)
{
  LangInput ret{};

  const auto semicolonIdx = value.find(';');

  if (semicolonIdx == std::string_view::npos)
  {
    // No var name, just a path
    ret.OutputFilename = value;
  }
  else
  {
    // Split var name and path
    ret.VarName        = value.substr(0u, semicolonIdx);
    ret.OutputFilename = value.substr(semicolonIdx + 1u);

    const auto colonIdx = ret.VarName.find(':');
    if (colonIdx != std::string_view::npos)
    {
      if (colonIdx > semicolonIdx)
      {
        std::string msg = "Ill-formed language string \"";
        msg += value;
        msg += '\"';
        throw std::runtime_error(msg);
      }

      ret.Namespace = ret.VarName.substr(0u, colonIdx);
      ret.VarName.remove_prefix(ret.Namespace.length() + 1u);
    }
  }

  ret.IsCout = ret.OutputFilename == coutName;

  return ret;
}

static inline void FinishFile(TextWriter& w, const path& outputFilename,
                              std::string_view languageDisplayName)
{
  if (outputFilename == coutName)
    std::cout << w.GetContents() << std::endl;
  else
  {
    std::cout << languageDisplayName << " -> " << outputFilename.u8string() << std::endl;
    w.SaveContentsToDisk(outputFilename);
  }
}

void Generator::GenerateC()
{
  const auto args = GetLangInput(m_Args.Get("c"));

  TextWriter w;

  std::string includeGuardName;

  if (!args.IsCout)
  {
    includeGuardName = args.OutputFilename.filename().u8string();
    std::transform(includeGuardName.begin(), includeGuardName.end(),
                   includeGuardName.begin(),
                   [](char ch) { return char(std::toupper(int(ch))); });
    std::replace(includeGuardName.begin(), includeGuardName.end(), '.', '_');
    includeGuardName += "_INCLUDED";

    w << "#ifndef " << includeGuardName << '\n';
    w << "#define " << includeGuardName << '\n';
    w << '\n';
    w << "#include <stddef.h>\n";
    w << "#include <stdint.h>\n";
    w << '\n';
    w << "static const uint8_t " << args.VarName << "[] = ";
  }

  w.OpenBrace();
  WriteDataAsBytes(w, m_Bytes, false);
  w.CloseBrace(!args.IsCout);

  if (!args.IsCout)
  {
    w << '\n';
    w << "#endif // " << includeGuardName << '\n';
  }

  FinishFile(w, args.OutputFilename, "C");
}

void Generator::GenerateCpp()
{
  const auto args = GetLangInput(m_Args.Get("cpp"));

  TextWriter w;

  if (!args.IsCout)
  {
    w << "#pragma once\n";
    w << '\n';
    w << "#include <cstddef>\n";
    w << "#include <cstdint>\n";
    w << '\n';
    w << "static const uint8_t " << args.VarName << "[] = ";
  }

  w.OpenBrace();
  WriteDataAsBytes(w, m_Bytes, false);
  w.CloseBrace(!args.IsCout);

  FinishFile(w, args.OutputFilename, "C++");
}

void Generator::GenerateCSharp()
{
  const auto args     = GetLangInput(m_Args.Get("csharp"));
  const bool isPublic = m_Args.Has("public");

  if (args.Namespace.empty())
  {
    throw std::runtime_error("No namespace specified for C#.");
  }

  TextWriter w;

  if (!args.IsCout)
  {
    w << "namespace " << args.Namespace << '\n';
    w.OpenBrace();
    w << (isPublic ? "public" : "internal") << " static class " << args.VarName << '\n';
    w.OpenBrace();
    w << "public static readonly byte[] Data = new[]\n";
  }

  w.OpenBrace();
  WriteDataAsBytes(w, m_Bytes, false);
  w.CloseBrace(!args.IsCout);

  if (!args.IsCout)
  {
    w.CloseBrace();
    w.CloseBrace();
  }

  FinishFile(w, args.OutputFilename, "C#");
}

void Generator::GenerateJava()
{
  const auto args = GetLangInput(m_Args.Get("java"));

  TextWriter w;

  if (!args.IsCout)
  {
    if (!args.Namespace.empty())
    {
      w << "package " << args.Namespace << ";\n";
      w << '\n';
    }

    w << "public class " << args.VarName << ' ';
    w.OpenBrace();

    w << "private " << args.VarName << "() ";
    w.OpenBrace();
    w << "// Nothing to do here.\n";
    w.CloseBrace();

    w << '\n';

    w << "public static byte[] getData() ";
    w.OpenBrace();
    w << "return m_Data;\n";
    w.CloseBrace();
    w << '\n';

    w << "private static final byte[] m_Data = ";
  }

  w.OpenBrace();
  WriteDataAsBytes(w, m_Bytes, true);
  w.CloseBrace(!args.IsCout);

  if (!args.IsCout)
  {
    w.CloseBrace();
  }

  FinishFile(w, args.OutputFilename, "Java");
}

void Generator::GeneratePython()
{
  const auto args = GetLangInput(m_Args.Get("python"));

  TextWriter w;

  if (!args.IsCout)
  {
    w << args.VarName << " = ";
  }

  w << "bytes([\n";
  w.Indent();
  WriteDataAsBytes(w, m_Bytes, false);
  w.Unindent();
  w << "])\n";

  FinishFile(w, args.OutputFilename, "Python");
}
