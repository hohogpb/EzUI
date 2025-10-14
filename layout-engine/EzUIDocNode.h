#pragma once


struct EzUIDocNode {
  std::wstring name;
  std::unordered_map<std::wstring, std::wstring> attributes;
  std::vector<std::unique_ptr<EzUIDocNode>> children;
  std::wstring text;
};