#include <map>
#include <iostream>
#include "include/parse_node.h"

namespace parse_node {

namespace {
std::map<std::string, Parser>& getParserMap() {
    // Use a function-local static to ensure initialization before use
    static std::map<std::string, Parser> parserMap;
    return parserMap;
}
}

void registerNode(std::string name, Parser parser) {
    std::cout << "[Startup] Registering node type: " << name << std::endl;
    getParserMap().emplace(name, parser);
}

std::unique_ptr<Node> createFromInput(std::string name, std::string argString) {
    auto& map = getParserMap();
    auto it = map.find(name);
    if (it == map.end()) {
        throw std::runtime_error("Unknown node type: " + name);
    }
    return it->second(argString);
}

}