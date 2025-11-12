# Multi-Type Node Demonstration

## Overview

This document showcases how the concept-based pipeline system supports multiple node types with **completely different parameter structures**, demonstrating the flexibility and type safety of the architecture.

## Node Type Comparison

### FooNode Parameters

```cpp
struct FooAstParams {
    std::string nodeType = "foo";
    std::string debugInfo;
    int fooSpecificData = 0;  // Simple integer field
};
```

**Characteristics:**
- Simple parameter structure
- Single integer field for node-specific data
- Lightweight and fast
- Low cost estimate (100 units)
- Optimization hint: "can_be_pushed_down"

### BarNode Parameters

```cpp
struct BarAstParams {
    std::string nodeType = "bar";
    std::string debugInfo;
    std::vector<std::string> barItems;  // Collection of items
    bool barFlag = false;                // Boolean flag
    double barCostEstimate = 0.0;        // Cost calculation
};
```

**Characteristics:**
- Complex parameter structure
- Vector of strings for multiple items
- Boolean flag for state tracking
- Double for precise cost estimation
- Dynamic cost based on items (items.size() * 10.5)
- Optimization hint varies based on flag: "can_use_index" or "full_scan"
- Conditional cost: 50 units (with index) vs 200 units (without)

## Pipeline Execution Comparison

### Running Both Node Types

```bash
$ bazel run //:toy_app
[Startup] Registering node type: foo
[Startup] Registering node type: bar
[Startup] Registering Logical node type: foo
[Startup] Registering Logical node type: bar

Processing "foo" node
========================================
[1] Creating parse node from input: "my_input_data"
    Shape: foo_shape

[2] Transforming to AST node (via node_transformer)
    FooAstNode[foo]: FooNode from parse layer with data: my_input_data (data=42)

[3] Creating logical node
    FooLogicalNode[foo]

[4] Execution Plan:
LOGICAL_PLAN:
  Operation: FooOperation
  Type: foo
  Optimization: can_be_pushed_down
  Estimated Cost: 100 units

Processing "bar" node
========================================
[1] Creating parse node from input: "bar_input_data"
    Shape: bar_shape

[2] Transforming to AST node (via node_transformer)
    BarAstNode[bar]: BarNode from parse layer with 2 items (items=2, flag=true, cost=21)

[3] Creating logical node
    BarLogicalNode[bar]

[4] Execution Plan:
LOGICAL_PLAN:
  Operation: BarOperation
  Type: bar
  Optimization: can_use_index
  Index Available: Yes
  Estimated Cost: 50 units
```

## Key Differences Highlighted

| Aspect | FooNode | BarNode |
|--------|---------|---------|
| **Param Fields** | 1 int | vector<string>, bool, double |
| **Parse Logic** | Simple assignment | Parses items from input |
| **Shape** | "foo_shape" | "bar_shape" |
| **AST Data** | data=42 | items=2, flag=true, cost=21 |
| **Optimization** | can_be_pushed_down | can_use_index (dynamic) |
| **Cost** | Fixed: 100 | Dynamic: 50 or 200 |
| **Index Usage** | Not applicable | Yes/No based on flag |

## Architecture Benefits Demonstrated

### ✅ **Type Diversity**
Each node type can have completely different parameter structures:
- Foo: Simple scalar value
- Bar: Complex collection + flags + calculations

### ✅ **Type Safety**
Despite different structures, the compiler ensures:
```cpp
// This works:
auto fooAst = ast_node::create<FooAstParams>(fooParams);  ✅

// This won't compile:
auto wrongAst = ast_node::create<BarAstParams>(fooParams);  ❌
// Error: no matching function for call
```

### ✅ **Polymorphic Input, Type-Safe Processing**
```cpp
// Both created polymorphically:
auto fooNode = parse_node::createFromInput("foo", "data");
auto barNode = parse_node::createFromInput("bar", "data");

// Both transformed with same API:
auto fooAst = node_transformer::parseToAst(fooNode);  // Uses FooAstParams
auto barAst = node_transformer::parseToAst(barNode);  // Uses BarAstParams
```

### ✅ **Independent Evolution**
- Add fields to FooAstParams → doesn't affect Bar
- Add fields to BarAstParams → doesn't affect Foo
- Add new node types → doesn't affect existing ones

### ✅ **No Runtime Overhead**
- No variant types or std::any
- No type_info or dynamic_cast
- Template specialization resolved at compile time
- Virtual dispatch only where needed (polymorphic creation)

## Code Organization

```
src/
├── ast_params/
│   ├── foo_ast_params.h    # Foo's unique parameters
│   └── bar_ast_params.h    # Bar's unique parameters (different!)
│
├── parse_nodes/
│   ├── foo_node.{h,cpp}    # Foo parse logic
│   └── bar_node.{h,cpp}    # Bar parse logic (different!)
│
├── ast_nodes/
│   ├── foo_ast_node.{h,cpp}  # Foo AST creation
│   └── bar_ast_node.{h,cpp}  # Bar AST creation (different!)
│
└── logical_nodes/
    ├── foo_logical_node.{h,cpp}  # Foo execution plan
    └── bar_logical_node.{h,cpp}  # Bar execution plan (different!)
```

**Each node type is completely self-contained!**

## Build Modularity

```python
# BUILD.bazel structure:

# Independent parameter libraries
cc_library(name = "foo_ast_params", ...)
cc_library(name = "bar_ast_params", ...)  # Separate!

# Independent parse nodes
cc_library(name = "foo_parse_node", deps = [":foo_ast_params", ...])
cc_library(name = "bar_parse_node", deps = [":bar_ast_params", ...])

# Independent AST nodes
cc_library(name = "foo_ast_nodes", deps = [":foo_ast_params", ...])
cc_library(name = "bar_ast_nodes", deps = [":bar_ast_params", ...])

# Combined only at the top level
cc_library(name = "parse_nodes_impl", deps = [":foo_parse_node", ":bar_parse_node"])
cc_library(name = "ast_nodes_impl", deps = [":foo_ast_nodes", ":bar_ast_nodes"])
```

**Benefits:**
- Compile foo independently of bar
- Team A works on foo, Team B on bar
- Minimal rebuild when modifying one type

## Extensibility Example

Want to add a "baz" node with even more different parameters?

```cpp
// src/ast_params/baz_ast_params.h
struct BazAstParams {
    std::string nodeType = "baz";
    std::string debugInfo;
    
    // Completely different from foo and bar!
    std::map<std::string, int> bazMetrics;
    std::chrono::duration bazTimeout;
    std::optional<std::string> bazFallback;
};
```

**Steps to add:**
1. Create `baz_ast_params.h`
2. Create `baz_node.{h,cpp}` implementing `ParseNodeType` concept
3. Create `baz_ast_node.{h,cpp}` with `create<BazAstParams>` specialization
4. Create `baz_logical_node.{h,cpp}` with registration
5. Add to BUILD.bazel
6. **Done!** No changes to infrastructure or other nodes.

## Real-World Analogy

Think of this like a **database query optimizer**:

| Node Type | Database Equivalent | Parameters |
|-----------|-------------------|------------|
| **FooNode** | Simple SELECT | column, filter |
| **BarNode** | JOIN operation | tables[], join_conditions[], use_index |
| **BazNode** (future) | Aggregate | group_by[], aggregates[], window_spec |

Each operation has **completely different optimization parameters**, but they all flow through the same pipeline infrastructure!

## Performance Characteristics

### Compile Time
- Each node type compiles independently
- Template instantiations are separate
- Parallel compilation possible
- Adding bar doesn't slow down foo compilation

### Runtime
- Zero overhead from type system
- Virtual dispatch only at polymorphic boundaries
- Template specializations inline to optimal code
- No runtime type checking

### Binary Size
- Dead code elimination removes unused specializations
- Each node type adds only its specific code
- No bloat from unused types

## Testing Strategy

Each node type can be tested independently:

```cpp
// Test foo in isolation
TEST(FooNodeTest, CreatesCorrectParams) {
    FooNode node("test");
    auto params = node.astParams();
    EXPECT_EQ(params.fooSpecificData, 42);
}

// Test bar in isolation
TEST(BarNodeTest, CreatesCorrectParams) {
    BarNode node("test");
    auto params = node.astParams();
    EXPECT_EQ(params.barItems.size(), 2);
    EXPECT_TRUE(params.barFlag);
    EXPECT_DOUBLE_EQ(params.barCostEstimate, 21.0);
}
```

**No interaction between tests!**

## Summary

The multi-type demonstration shows:

✅ **Flexibility** - Node types can have completely different parameter structures
✅ **Type Safety** - Compile-time verification prevents type mismatches
✅ **Modularity** - Each node type is self-contained and independent
✅ **Extensibility** - Adding new types doesn't require modifying existing code
✅ **Performance** - Zero runtime overhead from the type system
✅ **Maintainability** - Clear structure makes it easy to understand and modify

This architecture scales from simple nodes (Foo) to complex nodes (Bar) to arbitrarily sophisticated nodes (future Baz, Qux, etc.) **without sacrificing type safety or performance**! 🚀

