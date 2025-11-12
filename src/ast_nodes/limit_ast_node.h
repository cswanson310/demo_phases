#pragma once
#include "ast_node.h"
#include "limit_params.h"
#include <string>

// Forward declare logical_node namespace
namespace logical_node {
    struct Node;
    template<typename ParamType>
    std::unique_ptr<Node> create(const ParamType& params);
}

struct LimitAstNode : public ast_node::Node {
    LimitParams params;
    
    LimitAstNode(const LimitParams& params)
        : params(params) {}
    
    std::string debugName() const override {
        return "LimitAstNode: (limit=" + std::to_string(params.limitValue) + ")";
    }
    
    // Limit has its own logical params type
    LimitParams logicalParams() const {
        return params;
    }
    
    // Implement createLogicalNode - declared in .cpp to avoid circular deps
    std::unique_ptr<logical_node::Node> createLogicalNode() const override;
};

// Specialize the create function for LimitAstParams
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<LimitParams>(const LimitParams& params) {
        return std::make_unique<LimitAstNode>(params);
    }
}

