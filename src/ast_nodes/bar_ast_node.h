#pragma once
#include "ast_node.h"
#include "bar_ast_params.h"
#include <string>
#include <vector>
#include <sstream>

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
    
    LogicalParams logicalParams() const override {
        return LogicalParams{
            .nodeType = "bar",
            .optimizationHint = flag ? "can_use_index" : "full_scan"
        };
    }
};

// Specialize the create function for BarAstParams
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<BarAstParams>(const BarAstParams& params) {
        return std::make_unique<BarAstNode>(params);
    }
}

