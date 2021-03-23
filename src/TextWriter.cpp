#include "TextWriter.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <stdexcept>

using namespace std;

TextWriter::TextWriter()
    : m_IndentationEnabled(true)
{
  m_Contents.reserve(2048);
}

TextWriter::~TextWriter() noexcept = default;

void TextWriter::Clear()
{
  m_Depth = 0;
  m_Contents.clear();
  DetermineIndentString();
}

void TextWriter::OpenBrace()
{
  Write("{\n");
  Indent();
}

void TextWriter::CloseBrace(bool semicolon)
{
  Unindent();
  if (semicolon)
    Write("};", 2);
  else
    Write('}');

  Write('\n');
}

void TextWriter::Indent()
{
  ++m_Depth;
  DetermineIndentString();
}

void TextWriter::Unindent()
{
  assert(m_Depth > 0u);
  --m_Depth;
  DetermineIndentString();
}

void TextWriter::Write(char ch)
{
  Write(&ch, 1);
}

void TextWriter::Write(const char* str, size_t length)
{
  if (m_IndentationEnabled && !m_Contents.empty() && m_Contents.back() == '\n')
    m_Contents.append(m_IndentStr);

  m_Contents.append(str, length);
}

void TextWriter::SetIndentationEnabled(bool enabled)
{
  m_IndentationEnabled = enabled;
}

void TextWriter::SaveContentsToDisk(const path& filename)
{
  assert(!filename.empty());

  const auto parentPath = filename.parent_path();

  if (!parentPath.empty())
  {
    std::filesystem::create_directories(parentPath);

    if (!std::filesystem::exists(parentPath))
    {
      std::string msg = "Failed to create directory \"";
      msg += parentPath.u8string();
      msg += '.';
      throw std::runtime_error(msg);
    }
  }

  ofstream ofs{filename, ios::binary};

  if (!ofs)
  {
    std::string msg = "Failed to open \"";
    msg += filename.u8string();
    msg += " for writing.";
    throw runtime_error(msg);
  }

  ofs << m_Contents;
}

void TextWriter::DetermineIndentString()
{
  m_IndentStr = string(m_Depth * 2, ' ');
}
