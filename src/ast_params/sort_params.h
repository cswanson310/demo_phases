#pragma once
#include <string>
#include <vector>

// Parameters for Sort operations throughout the pipeline
struct SortParams {
    std::vector<std::string> sortKeys;  // Which fields to sort by
    bool ascending = true;               // Sort direction
};

