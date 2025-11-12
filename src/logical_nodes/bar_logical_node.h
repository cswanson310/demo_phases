#pragma once
#include "logical_node.h"
#include <string>
#include <sstream>

struct BarLogicalNode : public logical_node::Node {
    std::string nodeType;
    std::string optimizationHint;
    
    BarLogicalNode(const std::string& type, const std::string& hint)
        : nodeType(type), optimizationHint(hint) {}
    
    std::string debugName() const override {
        return "BarLogicalNode[" + nodeType + "]";
    }
    
    std::string explain() const override {
        std::ostringstream oss;
        oss << "LOGICAL_PLAN:\n"
            << "  Operation: BarOperation\n"
            << "  Type: " << nodeType << "\n"
            << "  Optimization: " << optimizationHint << "\n"
            << "  Index Available: " << (optimizationHint == "can_use_index" ? "Yes" : "No") << "\n"
            << "  Estimated Cost: " << (optimizationHint == "can_use_index" ? "50" : "200") << " units";
        return oss.str();
    }
};

