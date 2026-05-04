#include "JsonParser.h"
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace JsonParser {
JsonParser::JsonParser(std::string_view source_code)
    : m_source_code(source_code) {}

JsonValue JsonParser::parse() {
    whitespace();
    char c{current()};
    switch (c) {
    case '{':
        return JsonValue{handle_object()};
    case '[':
        return JsonValue{handle_array()};
    case '"':
        return JsonValue{handle_string()};
    default:
        if (std::isdigit(c) or c == '-') {
            return JsonValue{handle_number()};
        } else if (auto kw{try_handle_keyword()}; kw.has_value()) {
            return *kw;
        } else {
            throw std::runtime_error("Unexpected Character");
        }
    }
    return JsonValue{std::monostate()};
}

///////////////
// Handlers  //
///////////////

double JsonParser::handle_number() {
    size_t start{m_current};
    if (current() == '-') {
        advance();
    }
    while (std::isdigit(current()) && !is_at_end()) {
        advance();
    }
    if (try_consume('.')) {
        while (std::isdigit(current()) && !is_at_end()) {
            advance();
        }
    }
    size_t end{m_current};

    auto substr{m_source_code.substr(start, end - start)};
    return std::stod(substr);
}

std::string JsonParser::handle_string() {
    consume('"', "Expected an opening quotation mark");
    size_t start{m_current};
    while (current() != '"') {
        advance();
        if (is_at_end() || current() == '\n') {
            break;
        }
    }
    size_t end{m_current};
    consume('"', "Expected a closing quotation mark");
    auto substr{m_source_code.substr(start, end - start)};
    return substr;
}
std::optional<JsonValue> JsonParser::try_handle_keyword() {
    std::unordered_map<std::string, JsonValue> keyword_map{
        {"true", JsonValue{true}},
        {"false", JsonValue{false}},
        {"null", JsonValue{std::monostate{}}}};
    for (const auto &[kw, value] : keyword_map) {

        auto end{m_current + kw.length()};
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

JsonValue::Array JsonParser::handle_array() {
    JsonValue::Array output{};
    consume('[', "Expected an opening bracket");
    while (!try_consume(']')) {
        whitespace();
        auto value{parse()};
        whitespace();
        output.push_back(value);
        if (is_at_end() || !try_consume(',')) {
            consume(']', "Expected a closing bracket");
            break;
        }
    }
    return output;
}
JsonValue::Object JsonParser::handle_object() {
    JsonValue::Object output{};
    consume('{', "Expected an opening brace");

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
            consume('}', "Expected a closing brace");
            break;
        }
    }
    return output;
}

//////////////
// Helpers //
/////////////

void JsonParser::whitespace() {
    while (std::isspace(current())) {
        advance();
    }
}
void JsonParser::consume(char c, std::string_view msg) {
    if (current() != c) {
        throw std::runtime_error(msg.data());
    }
    advance();
}

char JsonParser::current() {
    if (is_at_end()) {
        return '\0';
    }
    return m_source_code[m_current];
}
char JsonParser::advance() {
    if (is_at_end()) {
        return '\0';
    }
    return m_source_code[m_current++];
}
bool JsonParser::try_consume(char c) {
    if (current() != c) {
        return false;
    }
    advance();
    return true;
}

bool JsonParser::is_at_end() { return m_current > m_source_code.size(); }
} // namespace JsonParser
