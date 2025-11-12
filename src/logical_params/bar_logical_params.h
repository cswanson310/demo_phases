#pragma once
#include <string>

// Parameters for creating BarLogicalNode from BarAstNode
// Bar has more complex logical planning needs
struct BarLogicalParams {
    std::string nodeType = "bar";
    std::string optimizationHint;
    
    // Bar-specific logical parameters
    bool canUseIndex;
    int estimatedRows;
    double selectivity;
};

