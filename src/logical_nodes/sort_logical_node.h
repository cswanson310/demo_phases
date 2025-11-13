#pragma once
#include "logical_node.h"
#include "sort_params.h"
#include <string>
#include <sstream>

struct SortLogicalNode : public LogicalNode {
    SortParams params;
    
    SortLogicalNode(const SortParams& params)
        : params(params) {}
    
    std::string debugName() const override {
        return "SortLogicalNode";
    }
    
    std::string explain() const override {
        std::ostringstream oss;
        oss << "LOGICAL_PLAN:\n"
            << "  Operation: Sort\n"
            << "  Sort Keys: [";
        for (size_t i = 0; i < params.sortKeys.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << params.sortKeys[i];
        }
        oss << "]\n"
            << "  Direction: " << (params.ascending ? "ASCENDING" : "DESCENDING") << "\n"
            << "  Algorithm: " << (params.sortKeys.size() > 3 ? "External Sort" : "QuickSort") << "\n"
            << "  Estimated Cost: " << (params.sortKeys.size() * 200) << " units";
        return oss.str();
    }
};

// Specialize the create function for SortParams
template<>
inline std::unique_ptr<LogicalNode> createLogicalNode<SortParams>(const SortParams& params) {
    return std::make_unique<SortLogicalNode>(params);
}
