#include "JsonParser.h"
#include <cctype>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

namespace JsonParser {
JsonParser::JsonParser(std::string_view source_code) {
  m_source_code = source_code;
  m_current = 0;
}

// Main function
JsonValue JsonParser::parse() {
  whitespace();
  char c = current();
  switch (c) {
  case '{':
    return JsonValue{handle_object()};
  case '[':
    return JsonValue{handle_array()};
  case '"':
    return JsonValue{handle_string()};
  default:
    if (std::isdigit(c)) {
      return JsonValue{handle_number()};
    } else if (auto kw{try_handle_keyword()}; kw.has_value()) {
      return *kw;
    } else {
      throw std::runtime_error("Could not process character");
    }
  }
  return JsonValue{std::monostate()};
}

// Handlers
std::string JsonParser::handle_string() {
  consume('"', "Expected an Opening quatation mark");
  int start = m_current;
  while (current() != '"') {
    advance();
    if (is_at_end() || current() == '\n') {
      break;
    }
  }
  int end = m_current;
  consume('"', "Expected a closing quatation mark");
  auto substr{m_source_code.substr(start, end - start)};
  return substr;
}
JsonValue::Array JsonParser::handle_array() {
  JsonValue::Array output{};
  consume('[', "Expected opening bracket.");
  while (!try_consume(']')) {
    whitespace();
    auto value{parse()};
    whitespace();
    output.push_back(value);
    if (is_at_end() || !try_consume(',')) {
      consume(']', "Expected closing bracket.");
      break;
    }
  }
  return output;
}

JsonValue::Object JsonParser::handle_object() {
  JsonValue::Object output{};
  consume('{', "Expected opening bracket.");
  while (!try_consume('}')) {
    whitespace();
    auto key{handle_string()};
    whitespace();
    consume(':', "Expected colon seperator between key value pair");
    whitespace();
    auto value{parse()};
    whitespace();
    output.insert({key, value});
    if (is_at_end() || !try_consume(',')) {
      consume('}', "Expected closing bracket.");
      break;
    }
  }
  return output;
}

double JsonParser::handle_number() {
  int start = m_current;
  while (std::isdigit(current()) && !is_at_end()) {
    advance();
  }
  // If there is a dot dont throw, just add it together and do it again.
  if (try_consume('.')) {
    while (std::isdigit(current()) && !is_at_end()) {
      advance();
    }
  }
  int end = m_current;
  auto substr{m_source_code.substr(start, end - start)};
  return std::stod(substr);
}

std::optional<JsonValue> JsonParser::try_handle_keyword() {
  std::unordered_map<std::string, JsonValue> keyword_map{
      {"true", JsonValue{true}},
      {"false", JsonValue{false}},
      {"null", JsonValue{std::monostate()}},
  };
  for (const auto &[kw, value] : keyword_map) {
    auto end = m_current + kw.length();
    if (end > m_source_code.size()) {
      continue;
    }
    if (m_source_code.substr(m_current, kw.length()) == kw) {
      m_current += kw.length();
      return value;
    }
  }
  return std::nullopt;
}

/////////////
// Helpers //
/////////////

void JsonParser::consume(char c, std::string_view error_msg) {
  if (current() != c) {
    throw std::runtime_error(error_msg.data());
  }
  advance();
}
bool JsonParser::try_consume(char c) {
  if (current() != c) {
    return false;
  }
  advance();
  return true;
}

// finds the next character that is not whitespace.
void JsonParser::whitespace() {
  while (std::isspace(current())) {
    advance();
  };
}

// Gets the character at the next index.
char JsonParser::advance() {
  if (is_at_end()) {
    return '\0';
  }
  return m_source_code[m_current++];
}

// Gets the character at the current index.
char JsonParser::current() {
  if (is_at_end()) {
    return '\0';
  }
  return m_source_code[m_current];
}

// Returns true when file is over.
bool JsonParser::is_at_end() { return m_current >= m_source_code.size(); }

} // namespace JsonParser
