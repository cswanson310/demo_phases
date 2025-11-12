#pragma once
#include <string>

// Parameters for creating logical nodes from AST nodes
// This is a pure data structure with no dependencies
struct LogicalParams {
    std::string nodeType;
    std::string optimizationHint;
};

