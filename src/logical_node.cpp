#include <map>
#include <iostream>
#include "include/logical_node.h"

namespace logical_node {

namespace {
std::map<std::string, Factory>& getFactoryMap() {
    static std::map<std::string, Factory> factoryMap;
    return factoryMap;
}
}

void registerNode(std::string name, Factory factory) {
    std::cout << "[Startup] Registering Logical node type: " << name << std::endl;
    getFactoryMap().emplace(name, factory);
}

std::unique_ptr<Node> create(const LogicalParams& params) {
    auto& map = getFactoryMap();
    auto it = map.find(params.nodeType);
    if (it == map.end()) {
        throw std::runtime_error("Unknown Logical node type: " + params.nodeType);
    }
    return it->second(params.nodeType, params.optimizationHint);
}

}

