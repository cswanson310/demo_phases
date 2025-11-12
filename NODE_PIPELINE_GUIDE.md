# Multi-Layer Node Pipeline System

This project implements a database-style query processing pipeline with three layers:
1. **Parse Layer** - Parse raw input into structured nodes
2. **AST Layer** - Transform parse nodes into abstract syntax trees
3. **Logical Layer** - Optimize and create execution plans

Each layer uses static registration so node types can be added without modifying central initialization code.

## Architecture Overview

```
Input String → Parse Node → AST Node → Logical Node → Execution
              (via factory) (astParams) (logicalParams)
```

Each transformation is driven by parameters returned from the previous layer:
- `parse_node::AstParams` - Info for creating AST nodes
- `ast_node::LogicalParams` - Info for creating logical nodes

## Adding a New Node Type

Let's add a "bar" node type to demonstrate the complete process.

### Step 1: Create Parse Node

**File: `src/parse_nodes/bar_node.h`**
```cpp
#pragma once
#include "parse_node.h"
#include <string>

struct BarNode : public parse_node::Node {
    std::string inputData;
    
    BarNode(const std::string& data) : inputData(data) {}
    
    std::string get_shape() const override {
        return "bar_shape";
    }
    
    parse_node::AstParams astParams() const override {
        return parse_node::AstParams{
            .nodeType = "bar",
            .debugInfo = "BarNode with data: " + inputData
        };
    }
};
```

**File: `src/parse_nodes/bar_node.cpp`**
```cpp
#include "bar_node.h"
#include "parse_node.h"
#include <memory>

// Register the bar parse node - runs before main()
REGISTER_NODE(bar, [](std::string argString) {
    return std::make_unique<BarNode>(argString);
});
```

### Step 2: Create AST Node

**File: `src/ast_nodes/bar_ast_node.h`**
```cpp
#pragma once
#include "ast_node.h"
#include <string>

struct BarAstNode : public ast_node::Node {
    std::string nodeType;
    std::string debugInfo;
    
    BarAstNode(const std::string& type, const std::string& debug)
        : nodeType(type), debugInfo(debug) {}
    
    std::string debugName() const override {
        return "BarAstNode[" + nodeType + "]: " + debugInfo;
    }
    
    ast_node::LogicalParams logicalParams() const override {
        return ast_node::LogicalParams{
            .nodeType = "bar",
            .optimizationHint = "cacheable"
        };
    }
};
```

**File: `src/ast_nodes/bar_ast_node.cpp`**
```cpp
#include "bar_ast_node.h"
#include "ast_node.h"
#include <memory>

// Register the bar AST node - runs before main()
REGISTER_AST_NODE(bar, [](const std::string& nodeType, const std::string& debugInfo) {
    return std::make_unique<BarAstNode>(nodeType, debugInfo);
});
```

### Step 3: Create Logical Node

**File: `src/logical_nodes/bar_logical_node.h`**
```cpp
#pragma once
#include "logical_node.h"
#include <string>

struct BarLogicalNode : public logical_node::Node {
    std::string nodeType;
    std::string optimizationHint;
    
    BarLogicalNode(const std::string& type, const std::string& hint)
        : nodeType(type), optimizationHint(hint) {}
    
    std::string debugName() const override {
        return "BarLogicalNode[" + nodeType + "]";
    }
    
    std::string explain() const override {
        return "LOGICAL_PLAN:\n"
               "  Operation: BarOperation\n"
               "  Type: " + nodeType + "\n"
               "  Optimization: " + optimizationHint + "\n"
               "  Can use index: true\n"
               "  Estimated Cost: 50 units";
    }
};
```

**File: `src/logical_nodes/bar_logical_node.cpp`**
```cpp
#include "bar_logical_node.h"
#include "logical_node.h"
#include <memory>

// Register the bar logical node - runs before main()
REGISTER_LOGICAL_NODE(bar, [](const std::string& nodeType, const std::string& optimizationHint) {
    return std::make_unique<BarLogicalNode>(nodeType, optimizationHint);
});
```

### Step 4: Update BUILD.bazel

Add your new files to the respective libraries:

```python
cc_library(
    name = "parse_nodes_impl",
    srcs = [
        "src/parse_nodes/foo_node.cpp",
        "src/parse_nodes/bar_node.cpp",  # Add this
    ],
    hdrs = [
        "src/parse_nodes/foo_node.h",
        "src/parse_nodes/bar_node.h",    # Add this
    ],
    includes = ["include"],
    deps = [":parse_node"],
    visibility = ["//visibility:public"],
    alwayslink = 1,  # Required for static registration
)

cc_library(
    name = "ast_nodes_impl",
    srcs = [
        "src/ast_nodes/foo_ast_node.cpp",
        "src/ast_nodes/bar_ast_node.cpp",  # Add this
    ],
    hdrs = [
        "src/ast_nodes/foo_ast_node.h",
        "src/ast_nodes/bar_ast_node.h",    # Add this
    ],
    includes = ["include"],
    deps = [":ast_node"],
    visibility = ["//visibility:public"],
    alwayslink = 1,  # Required for static registration
)

cc_library(
    name = "logical_nodes_impl",
    srcs = [
        "src/logical_nodes/foo_logical_node.cpp",
        "src/logical_nodes/bar_logical_node.cpp",  # Add this
    ],
    hdrs = [
        "src/logical_nodes/foo_logical_node.h",
        "src/logical_nodes/bar_logical_node.h",    # Add this
    ],
    includes = ["include"],
    deps = [":logical_node"],
    visibility = ["//visibility:public"],
    alwayslink = 1,  # Required for static registration
)
```

### Step 5: Use It

```cpp
int main() {
    // Create a bar node from input
    auto parseNode = parse_node::createFromInput("bar", "my_data");
    
    // Transform through the pipeline
    auto astNode = ast_node::create(parseNode->astParams());
    auto logicalNode = logical_node::create(astNode->logicalParams());
    
    // Display results
    std::cout << parseNode->get_shape() << std::endl;
    std::cout << astNode->debugName() << std::endl;
    std::cout << logicalNode->explain() << std::endl;
    
    return 0;
}
```

## Key Points

### Static Registration

Each `.cpp` file uses a registration macro that creates a static object:
- `REGISTER_NODE(name, factory)` - For parse nodes
- `REGISTER_AST_NODE(name, factory)` - For AST nodes
- `REGISTER_LOGICAL_NODE(name, factory)` - For logical nodes

These static objects' constructors run **before `main()`**, automatically registering the factories.

### The `alwayslink = 1` Attribute

**Critical:** Each `*_nodes_impl` library must have `alwayslink = 1` in BUILD.bazel. Without this:
- The linker may optimize out files with no directly-referenced symbols
- Static initialization won't run
- Node types won't be registered
- You'll get "Unknown node type" errors at runtime

### Parameter Structs

The parameter structs (`AstParams`, `LogicalParams`) act as interfaces between layers:
- They decouple layers (AST doesn't need to know about parse node internals)
- They're simple structs with just the data needed for the next layer
- They can be extended without breaking existing code

### Factory Patterns

Each layer uses factory functions:
- **Parse layer**: `Parser = function(string) -> unique_ptr<Node>`
- **AST layer**: `Factory = function(nodeType, debugInfo) -> unique_ptr<Node>`
- **Logical layer**: `Factory = function(nodeType, optimizationHint) -> unique_ptr<Node>`

Factories are stored in maps and looked up by node type name.

## Real-World Database Analogy

This pattern mirrors database query processing:

| Layer | Database Equivalent | Purpose |
|-------|-------------------|---------|
| Parse | SQL Parser | Convert text query to structured format |
| AST | Query Tree | Represent query structure |
| Logical | Logical Plan | Apply optimizations, cost estimation |
| (Future) Physical | Physical Plan | Choose algorithms, access methods |

## Extension Ideas

1. **Add a Physical layer** - Convert logical nodes to executable operators
2. **Add validation** - Validate nodes at each layer
3. **Add statistics** - Track costs, cardinalities
4. **Add rewrite rules** - Transform AST/logical nodes for optimization
5. **Add serialization** - Save/load execution plans

## Benefits of This Architecture

✅ **Extensibility** - Add new node types without modifying existing code
✅ **Separation of concerns** - Each layer has a clear responsibility
✅ **Testability** - Each layer can be tested independently
✅ **Type safety** - Compile-time checks for node interfaces
✅ **No central registry** - No need to modify a central initialization function

## Common Pitfalls

❌ Forgetting `alwayslink = 1` → Nodes won't register
❌ Wrong parameter struct types → Compilation errors
❌ Circular header dependencies → Use forward declarations
❌ Misspelling node type names → Runtime "Unknown type" errors

