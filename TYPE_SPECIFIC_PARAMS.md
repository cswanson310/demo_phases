# Type-Specific Parameters at Every Phase

## Overview

This document describes how each node type has its own parameter struct at every phase of the pipeline, ensuring complete flexibility and type safety throughout the transformation process.

## Architecture

### Parameter Hierarchy

```
Parse Node → AST Node → Logical Node
    |           |            |
    v           v            v
No params   XxxAstParams  XxxLogicalParams
```

Each transformation step uses type-specific parameter structs:

1. **Parse → AST**: Each parse node type defines its own `XxxAstParams` struct
2. **AST → Logical**: Each AST node type defines its own `XxxLogicalParams` struct

### Example: FooNode

```
FooNode (parse)
    ↓ produces FooAstParams { nodeType, debugInfo, fooSpecificData: int }
FooAstNode (ast)
    ↓ produces FooLogicalParams { nodeType, optimizationHint, costMultiplier: int }
FooLogicalNode (logical)
```

### Example: BarNode

```
BarNode (parse)
    ↓ produces BarAstParams { nodeType, debugInfo, barItems: vector<string>, barFlag: bool, barCostEstimate: double }
BarAstNode (ast)
    ↓ produces BarLogicalParams { nodeType, optimizationHint, canUseIndex: bool, estimatedRows: int, selectivity: double }
BarLogicalNode (logical)
```

## File Structure

### Parameter Libraries

Each parameter type is its own `cc_library`:

```
# AST Parameters
src/ast_params/
  ├── foo_ast_params.h      # FooAstParams struct
  └── bar_ast_params.h      # BarAstParams struct

# Logical Parameters
src/logical_params/
  ├── foo_logical_params.h  # FooLogicalParams struct
  └── bar_logical_params.h  # BarLogicalParams struct
```

### BUILD.bazel Dependencies

```python
cc_library(
    name = "foo_ast_params",
    hdrs = ["src/ast_params/foo_ast_params.h"],
    strip_include_prefix = "src/ast_params",
)

cc_library(
    name = "foo_logical_params",
    hdrs = ["src/logical_params/foo_logical_params.h"],
    strip_include_prefix = "src/logical_params",
)

cc_library(
    name = "foo_ast_nodes",
    deps = [
        ":ast_node",
        ":foo_ast_params",      # Input params
        ":foo_logical_params",  # Output params
        ":logical_node",
        ":foo_logical_nodes",   # For template specialization
    ],
)

cc_library(
    name = "foo_logical_nodes",
    deps = [
        ":logical_node",
        ":foo_logical_params",  # Input params
    ],
)
```

## Implementation Pattern

### AST Node Implementation

```cpp
// src/ast_nodes/foo_ast_node.h
#include "foo_ast_params.h"
#include "foo_logical_params.h"

struct FooAstNode : public ast_node::Node {
    // Constructor takes type-specific AST params
    FooAstNode(const FooAstParams& params);
    
    // Returns type-specific logical params
    FooLogicalParams logicalParams() const {
        return FooLogicalParams{
            .nodeType = "foo",
            .optimizationHint = "can_be_pushed_down",
            .costMultiplier = fooData > 0 ? 2 : 1
        };
    }
    
    // Polymorphic interface for transformation
    std::unique_ptr<logical_node::Node> createLogicalNode() const override;
};

// Template specialization for creating AST nodes from FooAstParams
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<FooAstParams>(const FooAstParams& params) {
        return std::make_unique<FooAstNode>(params);
    }
}
```

```cpp
// src/ast_nodes/foo_ast_node.cpp
#include "foo_ast_node.h"
#include "src/logical_nodes/foo_logical_node.h"

std::unique_ptr<logical_node::Node> FooAstNode::createLogicalNode() const {
    return logical_node::create<FooLogicalParams>(logicalParams());
}
```

### Logical Node Implementation

```cpp
// src/logical_nodes/foo_logical_node.h
#include "foo_logical_params.h"

struct FooLogicalNode : public logical_node::Node {
    int costMultiplier;
    
    // Constructor takes type-specific logical params
    FooLogicalNode(const FooLogicalParams& params)
        : costMultiplier(params.costMultiplier) {}
    
    std::string explain() const override {
        int totalCost = 100 * costMultiplier;
        return "Estimated Cost: " + std::to_string(totalCost) + " units";
    }
};

// Template specialization for creating logical nodes from FooLogicalParams
namespace logical_node {
    template<>
    inline std::unique_ptr<Node> create<FooLogicalParams>(const FooLogicalParams& params) {
        return std::make_unique<FooLogicalNode>(params);
    }
}
```

## Key Benefits

### 1. **Complete Flexibility**
Each node type can define exactly the parameters it needs at each phase:
- Foo uses simple `int` parameters
- Bar uses complex structures with vectors, bools, and doubles

### 2. **Type Safety**
Template specialization ensures compile-time type checking:
```cpp
// This is type-safe - compiler verifies FooLogicalParams → FooLogicalNode
auto node = logical_node::create<FooLogicalParams>(params);
```

### 3. **Clean Boundaries**
Each parameter library is a separate compilation unit with no dependencies:
```
foo_ast_params → (no deps)
foo_logical_params → (no deps)
```

### 4. **Polymorphic Transformation**
Despite type-specific parameters, transformations are still polymorphic:
```cpp
// Works for any AST node type!
auto logicalNode = ast_to_logical_transformer::astToLogical(astNode);
```

### 5. **No Runtime Overhead**
- No dynamic casts
- No type checking at runtime
- Virtual dispatch only for polymorphic operations
- Template specialization resolved at compile time

## Adding a New Node Type

To add a new node type (e.g., "Baz"):

1. **Create parameter structs:**
   - `src/ast_params/baz_ast_params.h`
   - `src/logical_params/baz_logical_params.h`

2. **Create node implementations:**
   - `src/parse_nodes/baz_node.{h,cpp}`
   - `src/ast_nodes/baz_ast_node.{h,cpp}`
   - `src/logical_nodes/baz_logical_node.{h,cpp}`

3. **Add BUILD.bazel targets:**
   ```python
   cc_library(name = "baz_ast_params", ...)
   cc_library(name = "baz_logical_params", ...)
   cc_library(name = "baz_parse_node", deps = [..., ":baz_ast_params", ...])
   cc_library(name = "baz_ast_nodes", deps = [..., ":baz_logical_params", ...])
   cc_library(name = "baz_logical_nodes", deps = [":baz_logical_params", ...])
   ```

4. **Implement template specializations:**
   - In `baz_ast_node.h`: `ast_node::create<BazAstParams>`
   - In `baz_logical_node.h`: `logical_node::create<BazLogicalParams>`

No changes needed to transformer libraries or base interfaces!

## Comparison with Previous Architecture

### Before (Pass-Through Pattern)
```
FooAstNode → LogicalParams (generic)
BarAstNode → LogicalParams (generic)
```
- All nodes used the same generic `LogicalParams`
- Lost type-specific information at logical phase

### After (Type-Specific Pattern)
```
FooAstNode → FooLogicalParams (specific)
BarAstNode → BarLogicalParams (specific)
```
- Each node type has its own logical parameters
- Type-specific information preserved throughout pipeline
- Mirrors the Parse→AST pattern

## Related Documentation

- [NODE_PIPELINE_GUIDE.md](NODE_PIPELINE_GUIDE.md) - How to add new node types
- [ARCHITECTURE.md](ARCHITECTURE.md) - Overall system architecture
- [CONCEPTS_GUIDE.md](CONCEPTS_GUIDE.md) - C++20 concepts usage
- [TRANSFORMER_PATTERN.md](TRANSFORMER_PATTERN.md) - Polymorphic transformations

