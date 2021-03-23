#pragma once

#include <string>
#include <vector>

class Arguments;

class Generator
{
public:
  explicit Generator(const Arguments& args);

  void Generate();

private:
  void ReadInputFile();

  void GenerateC();

  void GenerateCpp();

  void GenerateCSharp();

  void GenerateJava();

  void GeneratePython();

  const Arguments&     m_Args;
  std::vector<uint8_t> m_Bytes;
};
