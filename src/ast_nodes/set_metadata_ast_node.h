#pragma once
#include "ast_node.h"
#include "set_metadata_params.h"
#include <string>

// Forward declarations
struct LogicalNode;
template<typename ParamType>
std::unique_ptr<LogicalNode> createLogicalNode(const ParamType& params);

struct SetMetadataAstNode : public AstNode {
    SetMetadataParams params;
    
    SetMetadataAstNode(const SetMetadataParams& params)
        : params(params) {}
    
    std::string debugName() const override {
        return "SetMetadataAstNode: (name=" + params.metaName + ", expr=" + params.expression + ")";
    }
    
    // SetMetadata can use the same params for logical phase
    SetMetadataParams logicalParams() const {
        return params;
    }
    
    // Implement createLogicalNode - declared in .cpp to avoid circular deps
    std::unique_ptr<LogicalNode> createLogicalNode() const override;
};
