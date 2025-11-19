#pragma once
#include "ast_node.h"
#include "limit_params.h"
#include <string>

// Forward declarations
struct LogicalNode;
template<typename ParamType>
std::unique_ptr<LogicalNode> createLogicalNode(const ParamType& params);

struct LimitAstNode : public AstNode {
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
    std::unique_ptr<LogicalNode> createLogicalNode() const override;
};

template <>
std::unique_ptr<AstNode> createAstNode(const LimitParams& params);
