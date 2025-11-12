#pragma once
#include "logical_node.h"
#include <string>

struct FooLogicalNode : public logical_node::Node {
    std::string nodeType;
    std::string optimizationHint;
    
    FooLogicalNode(const std::string& type, const std::string& hint)
        : nodeType(type), optimizationHint(hint) {}
    
    std::string debugName() const override {
        return "FooLogicalNode[" + nodeType + "]";
    }
    
    std::string explain() const override {
        return "LOGICAL_PLAN:\n"
               "  Operation: FooOperation\n"
               "  Type: " + nodeType + "\n"
               "  Optimization: " + optimizationHint + "\n"
               "  Estimated Cost: 100 units";
    }
};

