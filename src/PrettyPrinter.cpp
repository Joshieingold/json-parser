#include "JsonParser.h" // Make sure this includes your JsonValue definition
#include <iostream>
#include <string>
#include <variant>

void prettyPrint(const JsonParser::JsonValue &jsonValue, int indent = 0) {
    const std::string indentStr(indent, ' ');

    // Helper lambda to handle different types
    auto printValue = [&](const auto &val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T,
                                     typename JsonParser::JsonValue::Array>) {
            std::cout << "[\n";
            for (size_t i = 0; i < val.size(); ++i) {
                std::cout << indentStr << "  ";
                prettyPrint(val[i], indent + 2);
                if (i != val.size() - 1)
                    std::cout << ",";
                std::cout << "\n";
            }
            std::cout << indentStr << "]";
        } else if constexpr (std::is_same_v<
                                 T, typename JsonParser::JsonValue::Object>) {
            std::cout << "{\n";
            size_t count = 0;
            for (const auto &[key, value] : val) {
                std::cout << indentStr << "  \"" << key << "\": ";
                prettyPrint(value, indent + 2);
                if (count != val.size() - 1)
                    std::cout << ",";
                std::cout << "\n";
                ++count;
            }
            std::cout << indentStr << "}";
        } else if constexpr (std::is_same_v<T, double>) {
            std::cout << val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            std::cout << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::monostate>) {
            std::cout << "null";
        }
    };

    std::visit(printValue, jsonValue.value);
}
