#include "pch.h"
#include "EzUICssParser.h"
#include "common/wchar_utils.h"
#include "EzUITextLoader.h"

static std::tuple<int, int, int> specificity(const SimpleSelector& sel) {
  return {
      sel.id ? 1 : 0,
      static_cast<int>(sel.classNames.size()),
      sel.tagName ? 1 : 0
  };
}

static bool ValidIdentifierChar(wchar_t c) {
  return IsAlnum(c) || c == '-' || c == '_';
}

std::unique_ptr<Stylesheet> EzUICssParser::Parse(const std::wstring& src) {
  EzUICssParser parser(src);

  auto sheet = std::make_unique<Stylesheet>();
  sheet->rules = parser.ParseRules();

  return sheet;
}

std::unique_ptr<Stylesheet> EzUICssParser::ParseFile(const std::filesystem::path& filepath) {
  EzUITextLoader loader;
  auto text = loader.Load(filepath.wstring());
  return Parse(text);
}

std::vector<Rule> EzUICssParser::ParseRules() {
  std::vector<Rule> rules;
  while (!Eof()) {
    ConsumeWhiteSpace();
    if (Eof()) break;
    rules.push_back(ParseRule());
  }
  return rules;
}

Rule EzUICssParser::ParseRule() {
  Rule rule;
  rule.selectors = ParseSelectors();
  rule.declarations = ParseDeclarations();
  return rule;
}

std::vector<Selector> EzUICssParser::ParseSelectors() {
  std::vector<Selector> selectors;
  while (true) {
    auto aSelector = ParseSimpleSelector();
    selectors.push_back(aSelector);
    ConsumeWhiteSpace();

    auto c = NextChar();
    if (c == ',') {
      ConsumeChar();
      ConsumeWhiteSpace();
    } else if (c == '{') {
      break;
    } else {
      throw std::runtime_error("Unexpected character in selector list");
    }
  }
  // Return selectors with highest specificity first, for use in matching.
  std::sort(selectors.begin(), selectors.end(), [](const auto& a, const auto& b) {
    return specificity(a) > specificity(b);
  });
  return selectors;
}

SimpleSelector EzUICssParser::ParseSimpleSelector() {
  SimpleSelector selector;
  while (!Eof()) {
    char c = NextChar();
    if (c == '#') {
      ConsumeChar();
      selector.id = ParseIdentifier();
    } else if (c == '.') {
      ConsumeChar();
      selector.classNames.push_back(ParseIdentifier());
    } else if (c == '*') {
      ConsumeChar(); // universal selector
    } else if (IsAlpha(c)) {
      selector.tagName = ParseIdentifier();
    } else if (c == ':') {
      // Pseudo-class or pseudo-element; skip for now.
      ConsumeChar();
      auto aPseudo = ParseIdentifier();
      selector.pseudoClasses.push_back(aPseudo);
    } else {
      break;
    }
  }
  return selector;
}

std::vector<Declaration> EzUICssParser::ParseDeclarations() {
  ExpectChar('{');
  std::vector<Declaration> declarations;
  while (true) {
    ConsumeWhiteSpace();
    if (NextChar() == '}') {
      ConsumeChar();
      break;
    }
    declarations.push_back(ParseDeclaration());
  }
  return declarations;
}

Declaration EzUICssParser::ParseDeclaration() {
  auto name = ParseIdentifier();
  ConsumeWhiteSpace();
  ExpectChar(':');
  auto value = ParsePropertys();
  return { name, value };
}

std::wstring EzUICssParser::ParseIdentifier() {
  auto result = ConsumeWhile(ValidIdentifierChar);
  return result;
}

std::vector<std::wstring> EzUICssParser::ParsePropertys() {
  std::vector<std::wstring> properties;
  while (true) {
    ConsumeWhiteSpace();
    auto c = NextChar();
    if (c == ';' || c == '}') {
      ConsumeChar();
      break;
    }
    properties.push_back(ParseProperty());
  }
  return properties;
}

std::wstring EzUICssParser::ParseProperty() {
  auto result = ConsumeWhile([](auto c) {
    return IsAlnum(c) || c == '-' || c == '_' || c == '(' || c == ')' || c == '#' || c == ',' || c == '.';
  });
  return result;
}

