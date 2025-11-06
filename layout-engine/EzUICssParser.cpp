#include "pch.h"
#include "EzUICssParser.h"
#include "common/wchar_utils.h"

static std::tuple<int, int, int> specificity(const SimpleSelector& sel) {
  return {
      sel.id ? 1 : 0,
      static_cast<int>(sel.classes.size()),
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
    if (NextChar() == ',') {
      ConsumeChar();
      ConsumeWhiteSpace();
    } else if (NextChar() == '{') {
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
      selector.classes.push_back(ParseIdentifier());
    } else if (c == '*') {
      ConsumeChar(); // universal selector
    } else if (isalpha(c)) {
      selector.tagName = ParseIdentifier();
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
  ConsumeWhiteSpace();
  Value value = ParseValue();
  ConsumeWhiteSpace();
  ExpectChar(';');
  return { name, value };
}

std::wstring EzUICssParser::ParseIdentifier() {
  auto result = ConsumeWhile(ValidIdentifierChar);
  return result;
}

Value EzUICssParser::ParseValue() {
  auto c = NextChar();
  if (isdigit(c)) {
    return std::make_pair(ParseFloat(), ParseUnit());
  } else if (c == '#') {
    return ParseColor();
  } else {
    return ParseIdentifier();
  }
}

float EzUICssParser::ParseFloat() {
  auto s = ConsumeWhile([](wchar_t c) {
    return IsDigit(c) || c == '.';
  });
  return std::stof(s);
}

Unit EzUICssParser::ParseUnit() {
  auto s = ParseIdentifier();
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  if (s == L"px")
    return Unit::Px;
  throw std::runtime_error(std::format("unrecognized unit"));
}

Color EzUICssParser::ParseColor() {
  ExpectChar('#');
  return { ParseHexPair(), ParseHexPair(), ParseHexPair(), 255 };
}

uint8_t EzUICssParser::ParseHexPair() {
  std::wstring_view hex = mSrc.substr(mPos, 2);
  mPos += 2;
  return static_cast<uint8_t>(std::stoi(hex.data(), nullptr, 16));
}
