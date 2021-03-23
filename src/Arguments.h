#pragma once

#include <filesystem>
#include <string>
#include <vector>

class Arguments
{
public:
  using Entry = std::pair<std::string, std::string>;

  Arguments(int argc, char* argv[]);

  const std::filesystem::path& GetWorkingDir() const;

  bool Has(std::string_view name) const;

  std::string_view Get(std::string_view name) const;

  size_t GetCount() const;

  const Entry& operator[](size_t index) const;

private:
  std::filesystem ::path m_WorkingDir;
  std::vector<Entry>     m_Entries;
};
