#pragma once
#include "logical_node.h"
#include "set_metadata_params.h"
#include <string>
#include <sstream>

struct SetMetadataLogicalNode : public LogicalNode {
    SetMetadataParams params;
    
    SetMetadataLogicalNode(const SetMetadataParams& params)
        : params(params) {}
    
    std::string debugName() const override {
        return "SetMetadataLogicalNode";
    }
    
    std::string explain() const override {
        std::ostringstream oss;
        oss << "LOGICAL_PLAN:\n"
            << "  Operation: SetMetadata\n"
            << "  Metadata Name: " << params.metaName << "\n"
            << "  Expression: " << params.expression << "\n"
            << "  Side Effects: Yes (metadata write)\n"
            << "  Estimated Cost: 10 units";
        return oss.str();
    }
};

// Specialize the create function for SetMetadataParams
template<>
inline std::unique_ptr<LogicalNode> createLogicalNode<SetMetadataParams>(const SetMetadataParams& params) {
    return std::make_unique<SetMetadataLogicalNode>(params);
}
