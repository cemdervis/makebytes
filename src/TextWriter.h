#pragma once

#include <filesystem>
#include <string>

using std::filesystem::path;

class TextWriter
{
public:
  TextWriter();

  ~TextWriter() noexcept;

  void Clear();

  void OpenBrace();

  void CloseBrace(bool semicolon = false);

  void Indent();

  void Unindent();

  void Write(char ch);

  void Write(const char* str, size_t length);

  void Write(std::string_view str)
  {
    Write(str.data(), str.length());
  }

  void SetIndentationEnabled(bool enabled);

  void SaveContentsToDisk(const path& filename);

  TextWriter& operator<<(char ch)
  {
    Write(ch);
    return *this;
  }

  TextWriter& operator<<(const char* str)
  {
    Write(str, std::strlen(str));
    return *this;
  }

  TextWriter& operator<<(std::string_view str)
  {
    Write(str);
    return *this;
  }

  TextWriter& operator<<(const std::string& str)
  {
    Write(str.data(), str.length());
    return *this;
  }

  std::string_view GetContents() const
  {
    return m_Contents;
  }

private:
  void DetermineIndentString();

  std::string m_Contents;
  size_t      m_Depth{};
  std::string m_IndentStr;
  bool        m_IndentationEnabled;
};
