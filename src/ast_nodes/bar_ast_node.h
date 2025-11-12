#pragma once
#include "ast_node.h"
#include "bar_ast_params.h"
#include "bar_logical_params.h"
#include <string>
#include <vector>
#include <sstream>

// Forward declare logical_node namespace
namespace logical_node {
    struct Node;
    template<typename ParamType>
    std::unique_ptr<Node> create(const ParamType& params);
}

struct BarAstNode : public ast_node::Node {
    std::string nodeType;
    std::string debugInfo;
    std::vector<std::string> items;
    bool flag;
    double costEstimate;
    
    BarAstNode(const BarAstParams& params)
        : nodeType(params.nodeType)
        , debugInfo(params.debugInfo)
        , items(params.barItems)
        , flag(params.barFlag)
        , costEstimate(params.barCostEstimate) {}
    
    std::string debugName() const override {
        std::ostringstream oss;
        oss << "BarAstNode[" << nodeType << "]: " << debugInfo 
            << " (items=" << items.size()
            << ", flag=" << (flag ? "true" : "false")
            << ", cost=" << costEstimate << ")";
        return oss.str();
    }
    
    // Bar has its own logical params type with richer information
    BarLogicalParams logicalParams() const {
        return BarLogicalParams{
            .nodeType = "bar",
            .optimizationHint = flag ? "can_use_index" : "full_scan",
            .canUseIndex = flag,
            .estimatedRows = static_cast<int>(items.size() * 100),
            .selectivity = flag ? 0.1 : 1.0
        };
    }
    
    // Implement createLogicalNode - declared in .cpp to avoid circular deps
    std::unique_ptr<logical_node::Node> createLogicalNode() const override;
};

// Specialize the create function for BarAstParams
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<BarAstParams>(const BarAstParams& params) {
        return std::make_unique<BarAstNode>(params);
    }
}

