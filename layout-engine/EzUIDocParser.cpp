#include "pch.h"
#include "EzUIDocParser.h"
#include <iostream>
#include <fstream>
#include "EzUITextLoader.h"
#include <common/wchar_utils.h>


std::unique_ptr<EzUIDocNode> EzUIDocParser::Parse(const std::wstring& src) {
  EzUIDocParser parser(src);

  auto nodes = parser.ParseNodes();
  if (nodes.size() == 1)
    return std::move(nodes[0]);

  auto root = std::make_unique<EzUIDocNode>();
  root->name = L"Window";
  root->children = std::move(nodes);

  return root;
}

std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseFile(const std::filesystem::path& filepath) {
  EzUITextLoader loader;
  auto text = loader.Load(filepath.wstring());
  return Parse(text);
}

/// Parse a sequence of sibling nodes.
std::vector<std::unique_ptr<EzUIDocNode>> EzUIDocParser::ParseNodes() {
  std::vector<std::unique_ptr<EzUIDocNode>> nodes;

  while (true) {
    ConsumeWhiteSpace();
    if (Eof() || StartsWith(L"</"))
      break;

    auto node = ParseNode();
    if (node->name == L"comment") {
      continue;
    }

    nodes.push_back(std::move(node));
  }

  return nodes;
}

/// Parse a single node.
std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseNode() {
  if (StartsWith(L"<!--")) {
    return ParseComment();
  } else if (StartsWith(L"<")) {
    return ParseElement();
  } else {
    return ParseText();
  }
}

/// Parse a single element, including its open tag, contents, and closing tag.
std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseComment() {
  // Opening tag.
  Expect(L"<!--");

  std::wstring result;
  while (!StartsWith(L"-->")) {
    result += ConsumeChar();
  }

  Expect(L"-->");

  auto aNode = std::make_unique<EzUIDocNode>();
  aNode->name = L"comment";
  aNode->text = result;
  return aNode;
}

/// Parse a single element, including its open tag, contents, and closing tag.
std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseElement() {
  auto startPos = mPos;
  // Opening tag.
  Expect(L"<");

  auto tagName = ParseName();
  auto attrs = ParseAttributes();

  if (StartsWith(L"/>")) {
    Expect(L"/>");
    auto aNode = std::make_unique<EzUIDocNode>();
    aNode->name = tagName;
    aNode->attributes = attrs;
    return aNode;
  }

  Expect(L">");

  std::vector<std::unique_ptr<EzUIDocNode>> children;
  std::wstring content;

  // Special handling for <style> and <svg> tags
  if (tagName == L"style") {
    auto endTag = L"</" + tagName + L">";
    content = ConsumeWhile([&](auto c) {
      return !StartsWith(endTag);
    });
  } else {
    // Contents.
    children = ParseNodes();
  }

  // Closing tag.
  Expect(L"</");
  Expect(tagName);
  Expect(L">");

  auto aNode = std::make_unique<EzUIDocNode>();
  aNode->name = tagName;
  aNode->attributes = attrs;
  aNode->children = std::move(children);
  aNode->content = content;
  return aNode;
}


/// Parse a tag or attribute name.
std::wstring EzUIDocParser::ParseName() {
  return ConsumeWhile([](auto c) {
    return IsAlnum(c) || c == '_' || c == '-' || c == ':';
  });
}

/// Parse a list of name="value" pairs, separated by whitespace.
std::unordered_map<std::wstring, std::wstring> EzUIDocParser::ParseAttributes() {
  std::unordered_map<std::wstring, std::wstring> attributes;

  while (true) {
    ConsumeWhiteSpace();
    if (NextChar() == '>' || StartsWith(L"/>"))
      break;

    auto [name, value] = ParseAttr();
    attributes[name] = value;
  }
  return attributes;
}

/// Parse a single name="value" pair.
std::pair<std::wstring, std::wstring> EzUIDocParser::ParseAttr() {
  auto name = ParseName();
  ConsumeWhiteSpace();
  Expect(L"=");
  ConsumeWhiteSpace();
  auto value = ParseAttrValue();
  return { name, value };
}

/// Parse a quoted value.
std::wstring EzUIDocParser::ParseAttrValue() {
  auto openQuote = ConsumeChar(); // ' or "
  if (!(openQuote == '"' || openQuote == '\''))
    throw std::runtime_error("Attribute value must start with a quote");

  auto value = ConsumeWhile([=](auto c) {
    return c != openQuote;
  });

  auto closeQuote = ConsumeChar();
  if (openQuote != closeQuote)
    throw std::runtime_error("Attribute value must end with a matching quote");

  return value;
}

/// Parse a text node.
std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseText() {
  // dom::text(self.consume_while(| c | c != '<'))
  auto s = ConsumeWhile([](auto c) { return c != '<'; });

  auto node = std::make_unique<EzUIDocNode>();
  node->name = L"text";
  node->text = s;

  return node;
}
