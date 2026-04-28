#include "./PrettyPrinter.cpp"
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
  // Get the file
  std::fstream file{"../src/CanadianaQuiz.json"};
  std::string content{};
  std::string line{};

  // Put the contents into a string,
  while (std::getline(file, line)) {
    content += line + "\n";
  }

  // Parse
  JsonParser::JsonParser json_parser{content};

  prettyPrint(json_parser.parse());
  return 0;
}
