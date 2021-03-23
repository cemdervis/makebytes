#include "Arguments.h"

#include <algorithm>

static inline std::string_view Trim(std::string_view str)
{
  const auto trimWithDelim = [&str](char delim) {
    while (!str.empty() && str.back() == delim)
      str.remove_suffix(1u);

    while (!str.empty() && str.front() == delim)
      str.remove_prefix(1u);
  };

  trimWithDelim(' ');
  trimWithDelim('\"');
  trimWithDelim(' ');

  return str;
}

Arguments::Arguments(int argc, char* argv[])
{
  if (argc <= 0)
    return;

  m_WorkingDir = argv[0];

  if (argc < 2)
    return;

  for (int i = 1; i < argc; ++i)
  {
    const std::string_view str = argv[i];
    const auto             idx = str.find('=');

    if (idx != std::string::npos)
    {
      const auto key   = Trim(str.substr(0, idx));
      const auto value = Trim(str.substr(idx + 1));

      m_Entries.emplace_back(std::string(key), std::string(value));
    }
    else
    {
      m_Entries.emplace_back(std::string(Trim(str)), std::string());
    }
  }
}

const std::filesystem::path& Arguments::GetWorkingDir() const
{
  return m_WorkingDir;
}

bool Arguments::Has(std::string_view name) const
{
  const auto it =
      std::find_if(m_Entries.cbegin(), m_Entries.cend(),
                   [&name](const std::pair<std::string, std::string>& pair) {
                     return pair.first == name;
                   });

  return it != m_Entries.cend();
}

std::string_view Arguments::Get(std::string_view name) const
{
  const auto it =
      std::find_if(m_Entries.cbegin(), m_Entries.cend(),
                   [&name](const std::pair<std::string, std::string>& pair) {
                     return pair.first == name;
                   });

  return it != m_Entries.cend() ? it->second : std::string_view();
}

size_t Arguments::GetCount() const
{
  return m_Entries.size();
}

const Arguments::Entry& Arguments::operator[](size_t index) const
{
  return m_Entries[index];
}
