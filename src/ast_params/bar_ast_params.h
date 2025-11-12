#pragma once
#include <string>
#include <vector>

// Parameters for creating BarAstNode from BarParseNode
// Note: Bar has completely different fields than Foo!
struct BarAstParams {
    std::string nodeType = "bar";
    std::string debugInfo;
    
    // Bar-specific parameters (different from Foo's single int)
    std::vector<std::string> barItems;
    bool barFlag = false;
    double barCostEstimate = 0.0;
};

