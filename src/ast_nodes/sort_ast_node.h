#pragma once
#include "ast_node.h"
#include "sort_params.h"
#include <string>
#include <sstream>

// Forward declarations
struct LogicalNode;
template<typename ParamType>
std::unique_ptr<LogicalNode> createLogicalNode(const ParamType& params);

struct SortAstNode : public AstNode {
    SortParams params;
    
    SortAstNode(const SortParams& params)
        : params(params) {}
    
    std::string debugName() const override {
        std::ostringstream oss;
        oss << "SortAstNode: (keys=" << params.sortKeys.size()
            << ", direction=" << (params.ascending ? "ASC" : "DESC") << ")";
        return oss.str();
    }
    
    // Sort can use the same params for logical phase
    SortParams logicalParams() const {
        return params;
    }
    
    // Implement createLogicalNode - declared in .cpp to avoid circular deps
    std::unique_ptr<LogicalNode> createLogicalNode() const override;
};

// Specialize the create function for SortParams
template<>
inline std::unique_ptr<AstNode> createAstNode<SortParams>(const SortParams& params) {
    return std::make_unique<SortAstNode>(params);
}
