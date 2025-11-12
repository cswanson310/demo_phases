#pragma once
#include <string>

// Parameters for creating FooAstNode from FooParseNode
struct FooAstParams {
    std::string nodeType = "foo";
    std::string debugInfo;
    
    // Can add foo-specific parameters here
    int fooSpecificData = 0;
};
