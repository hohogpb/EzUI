#pragma once

#include <variant>

struct Color {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 255;
};

// Unit of length
enum class Unit {
  Px,
};

using Value = std::variant<
  std::wstring,
  std::pair<float, Unit>,
  Color
>;

// Simple selector: tag#id.class1.class2...
struct SimpleSelector {
  std::optional<std::wstring> tagName;
  std::optional<std::wstring> id;
  std::vector<std::wstring> classes;

    std::wstring to_string() const {
        std::wostringstream oss;

        // tagName
        if (tagName) {
            oss << *tagName;
        } else {
            oss << L"*"; // universal selector
        }

        // id
        if (id) {
            oss << L"#" << *id;
        }

        // classes
        for (const auto& cls : classes) {
            oss << L"." << cls;
        }

        return oss.str();
    }
};

using Selector = SimpleSelector;


struct Declaration {
  std::wstring name;
  Value value;
};

struct Rule {
  std::vector<Selector> selectors;
  std::vector<Declaration> declarations;
};

struct Stylesheet {
  std::vector<Rule> rules;
};