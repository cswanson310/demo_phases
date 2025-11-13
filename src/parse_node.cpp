#include "parse_node.h"
#include <map>
#include <iostream>

// Function-local static to guarantee initialization order
std::map<std::string, ParseNodeFactory>& getParserMap() {
    static std::map<std::string, ParseNodeFactory> parserMap;
    return parserMap;
}

void registerParseNode(std::string name, ParseNodeFactory factory) {
    std::cout << "[Startup] Registering node type: " << name << std::endl;
    getParserMap().emplace(name, factory);
}

std::unique_ptr<ParseNode> createParseNodeFromInput(std::string name, std::string argString) {
    auto& parserMap = getParserMap();
    auto it = parserMap.find(name);
    if (it == parserMap.end()) {
        throw std::runtime_error("Unknown parse node type: " + name);
    }
    return it->second(argString);
}
