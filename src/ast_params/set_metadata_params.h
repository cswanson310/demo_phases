#pragma once
#include <string>

// Parameters for SetMetadata operations throughout the pipeline
struct SetMetadataParams {
    std::string metaName;     // Name of the metadata field
    std::string expression;   // Expression to compute the metadata value
};

