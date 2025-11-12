#pragma once
#include "logical_node.h"
#include "foo_logical_params.h"
#include <string>

struct FooLogicalNode : public logical_node::Node {
    std::string nodeType;
    std::string optimizationHint;
    int costMultiplier;
    
    FooLogicalNode(const FooLogicalParams& params)
        : nodeType(params.nodeType)
        , optimizationHint(params.optimizationHint)
        , costMultiplier(params.costMultiplier) {}
    
    std::string debugName() const override {
        return "FooLogicalNode[" + nodeType + "]";
    }
    
    std::string explain() const override {
        int baseCost = 100;
        int totalCost = baseCost * costMultiplier;
        return "LOGICAL_PLAN:\n"
               "  Operation: FooOperation\n"
               "  Type: " + nodeType + "\n"
               "  Optimization: " + optimizationHint + "\n"
               "  Cost Multiplier: " + std::to_string(costMultiplier) + "\n"
               "  Estimated Cost: " + std::to_string(totalCost) + " units";
    }
};

// Specialize the create function for FooLogicalParams
namespace logical_node {
    template<>
    inline std::unique_ptr<Node> create<FooLogicalParams>(const FooLogicalParams& params) {
        return std::make_unique<FooLogicalNode>(params);
    }
}


