#include "JsonValue.h"
#include <optional>
#include <string>
#include <string_view>

namespace JsonParser {
class JsonParser {
  public:
    JsonParser(std::string_view source_code);

    JsonValue parse();

  private:
    std::string handle_string();
    JsonValue::Array handle_array();
    JsonValue::Object handle_object();
    double handle_number();
    std::optional<JsonValue> try_handle_keyword();

  private:
    void whitespace();
    char advance();
    char current();
    void consume(char c, std::string_view msg);
    bool try_consume(char c);
    bool is_at_end();

  private:
    std::string m_source_code{};
    size_t m_current{};
};
} // namespace JsonParser
