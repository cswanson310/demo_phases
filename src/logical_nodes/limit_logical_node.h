#pragma once
#include "logical_node.h"
#include "limit_params.h"
#include <string>
#include <sstream>

struct LimitLogicalNode : public logical_node::Node {
    LimitParams params;
    
    LimitLogicalNode(const LimitParams& params)
        : params(params) {}
    
    std::string debugName() const override {
        return "LimitLogicalNode";
    }
    
    std::string explain() const override {
        std::ostringstream oss;
        oss << "LOGICAL_PLAN:\n"
            << "  Operation: Limit\n"
            << "  Row Limit: " << params.limitValue << "\n"
            << "  Estimated Memory: " << (params.limitValue * 100) << " bytes";
        return oss.str();
    }
};

// Specialize the create function for LimitLogicalParams
namespace logical_node {
    template<>
    inline std::unique_ptr<Node> create<LimitParams>(const LimitParams& params) {
        return std::make_unique<LimitLogicalNode>(params);
    }
}

