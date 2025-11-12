#pragma once
#include <string>

// Parameters for creating FooLogicalNode from FooAstNode
// Foo is a simple node, so it uses basic optimization parameters
struct FooLogicalParams {
    std::string nodeType = "foo";
    std::string optimizationHint;
    
    // Foo-specific logical parameters
    int costMultiplier = 1;
};

