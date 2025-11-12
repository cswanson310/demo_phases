#pragma once
#include <string>

// Parameters for creating AST nodes from parse nodes
// This is a pure data structure with no dependencies
struct AstParams {
    std::string nodeType;
    std::string debugInfo;
};

