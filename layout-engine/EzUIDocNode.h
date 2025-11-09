#pragma once


struct EzUIDocNode {
  std::wstring name;
  std::unordered_map<std::wstring, std::wstring> attributes;
  std::vector<std::unique_ptr<EzUIDocNode>> children;
  std::wstring text;
  std::wstring content;

  std::optional<std::wstring> GetId() const {
    auto it = attributes.find(L"id");
    if (it != attributes.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  std::vector<std::wstring> GetClassNames() const {
    std::vector<std::wstring> classNames;
    auto it = attributes.find(L"class");
    if (it != attributes.end()) {
      std::wstring classes = it->second;
      size_t start = 0;
      size_t end = classes.find(L' ');
      while (end != std::wstring::npos) {
        classNames.push_back(classes.substr(start, end - start));
        start = end + 1;
        end = classes.find(L' ', start);
      }
      classNames.push_back(classes.substr(start));
    }
    return classNames;
  }

  bool HasClass(const std::wstring& className) const {
    auto classNames = GetClassNames();
    return std::find(classNames.begin(), classNames.end(), className) != classNames.end();
  }
};

void PrintXmlTree(const EzUIDocNode* node, int indent = 0);
std::wstring DumpDocTree(const EzUIDocNode* node);