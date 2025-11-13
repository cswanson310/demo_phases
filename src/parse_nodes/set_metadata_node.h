#pragma once
#include "parse_node.h"
#include "set_metadata_params.h"
#include <string>

// Forward declarations
struct AstNode;
template<typename ParamType>
std::unique_ptr<AstNode> createAstNode(const ParamType& params);

struct SetMetadataNode : public TypedParseNode<SetMetadataNode, SetMetadataParams> {
    std::string metaName;
    std::string expression;
    
    SetMetadataNode(const std::string& arg) {
        // Parse input like "name:expression" or just use defaults
        size_t colonPos = arg.find(':');
        if (colonPos != std::string::npos) {
            metaName = arg.substr(0, colonPos);
            expression = arg.substr(colonPos + 1);
        } else {
            metaName = "default_meta";
            expression = arg;
        }
    }
    
    std::string get_shape() const override {
        return "set_metadata_shape";
    }
    
    // Returns type-specific AST parameters
    SetMetadataParams astParams() const override {
        return SetMetadataParams{
            .metaName = metaName,
            .expression = expression
        };
    }
    
    // Implementation in .cpp to avoid template instantiation order issues
    std::unique_ptr<AstNode> createAstNode() const override;
};

