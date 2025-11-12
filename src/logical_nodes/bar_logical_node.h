#pragma once
#include "logical_node.h"
#include "bar_logical_params.h"
#include <string>
#include <sstream>

struct BarLogicalNode : public logical_node::Node {
    std::string nodeType;
    std::string optimizationHint;
    bool canUseIndex;
    int estimatedRows;
    double selectivity;
    
    BarLogicalNode(const BarLogicalParams& params)
        : nodeType(params.nodeType)
        , optimizationHint(params.optimizationHint)
        , canUseIndex(params.canUseIndex)
        , estimatedRows(params.estimatedRows)
        , selectivity(params.selectivity) {}
    
    std::string debugName() const override {
        return "BarLogicalNode[" + nodeType + "]";
    }
    
    std::string explain() const override {
        std::ostringstream oss;
        int estimatedCost = canUseIndex ? 50 : 200;
        oss << "LOGICAL_PLAN:\n"
            << "  Operation: BarOperation\n"
            << "  Type: " << nodeType << "\n"
            << "  Optimization: " << optimizationHint << "\n"
            << "  Index Available: " << (canUseIndex ? "Yes" : "No") << "\n"
            << "  Estimated Rows: " << estimatedRows << "\n"
            << "  Selectivity: " << selectivity << "\n"
            << "  Estimated Cost: " << estimatedCost << " units";
        return oss.str();
    }
};

// Specialize the create function for BarLogicalParams
namespace logical_node {
    template<>
    inline std::unique_ptr<Node> create<BarLogicalParams>(const BarLogicalParams& params) {
        return std::make_unique<BarLogicalNode>(params);
    }
}

