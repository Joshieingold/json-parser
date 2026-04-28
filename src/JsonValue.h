#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
namespace JsonParser {
struct JsonValue {
  using Array = std::vector<JsonValue>;
  using Object = std::unordered_map<std::string, JsonValue>;
  std::variant<Array, Object, double, std::string, bool, std::monostate> value;
};
} // namespace JsonParser
