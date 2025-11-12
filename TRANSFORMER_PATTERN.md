# Transformer Pattern - Symmetric Pipeline Architecture

## Overview

The pipeline uses **two symmetric transformer libraries** to handle polymorphic conversions between layers:

1. **`node_transformer`** - Parse → AST
2. **`ast_to_logical_transformer`** - AST → Logical

Both follow the same design pattern, creating a consistent and elegant architecture.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     Complete Pipeline                        │
└─────────────────────────────────────────────────────────────┘

Input String
    ↓
┌────────────────┐
│ parse_node::   │
│ createFromInput│ ← Runtime factory (string → concrete type)
└────────┬───────┘
         ↓
    parse_node::Node (polymorphic)
         ↓
┌────────────────────────┐
│  node_transformer::    │
│  parseToAst()          │ ← Virtual dispatch: node.createAstNode()
└──────────┬─────────────┘
           ↓
    ast_node::Node (polymorphic)
           ↓
┌──────────────────────────────┐
│ ast_to_logical_transformer:: │
│ astToLogical()                │ ← Virtual dispatch: node.createLogicalNode()
└──────────┬───────────────────┘
           ↓
    logical_node::Node (polymorphic)
           ↓
    Execution / Explain
```

## Symmetric Design

### Transformer 1: Parse → AST

**Interface:**
```cpp
namespace node_transformer {
    std::unique_ptr<ast_node::Node> parseToAst(const parse_node::Node& parseNode);
}
```

**Implementation:**
```cpp
std::unique_ptr<ast_node::Node> parseToAst(const parse_node::Node& parseNode) {
    return parseNode.createAstNode();  // Virtual dispatch
}
```

**Node-side implementation:**
```cpp
// In FooNode
std::unique_ptr<ast_node::Node> FooNode::createAstNode() const {
    return ast_node::create<FooAstParams>(astParams());
}
```

### Transformer 2: AST → Logical

**Interface:**
```cpp
namespace ast_to_logical_transformer {
    std::unique_ptr<logical_node::Node> astToLogical(const ast_node::Node& astNode);
}
```

**Implementation:**
```cpp
std::unique_ptr<logical_node::Node> astToLogical(const ast_node::Node& astNode) {
    return astNode.createLogicalNode();  // Virtual dispatch
}
```

**Node-side implementation:**
```cpp
// In FooAstNode
std::unique_ptr<logical_node::Node> FooAstNode::createLogicalNode() const {
    return logical_node::create(logicalParams());
}
```

## Pass-Through Parameter Pattern

### The Optimization

For **simple node types**, parameters can be "passed through" without transformation:

```cpp
// FooNode (simple node)
FooAstParams astParams() const override {
    return FooAstParams{
        .nodeType = "foo",
        .debugInfo = "...",
        .fooSpecificData = 42  // Node-specific field
    };
}

// FooAstNode (simple node)
LogicalParams logicalParams() const override {
    return LogicalParams{
        .nodeType = "foo",
        .optimizationHint = "can_be_pushed_down"  // Standard field
    };
}
```

**Key insight:** 
- Parse→AST uses **custom parameters** (FooAstParams with node-specific fields)
- AST→Logical uses **standard parameters** (LogicalParams with common fields)
- Simple nodes can use the same LogicalParams structure!

### Parameter Type Comparison

| Transition | Parameter Type | Variety |
|------------|---------------|---------|
| **Parse → AST** | Node-specific (FooAstParams, BarAstParams) | High variety - each node has unique fields |
| **AST → Logical** | Standard (LogicalParams) | Low variety - simple nodes use same struct |

### When to Use Pass-Through

**✅ Use standard LogicalParams when:**
- Node is a simple operator (filter, project, scan)
- No complex optimization state needed
- Standard optimization hints suffice
- Node doesn't require special logical handling

**❌ Create custom logical params when:**
- Complex join algorithms (need join strategy details)
- Aggregation operations (need grouping keys, aggregates)
- Window functions (need window frame specifications)
- Custom operators (need operator-specific state)

### Example: Complex Node with Custom Logical Params

For more complex nodes, you could create custom logical parameter types:

```cpp
// Hypothetical complex join node
struct JoinLogicalParams {
    std::string nodeType = "join";
    std::string optimizationHint;
    
    // Join-specific fields
    std::string joinAlgorithm;  // "hash", "merge", "nested_loop"
    std::vector<std::string> leftKeys;
    std::vector<std::string> rightKeys;
    double estimatedCardinality;
};

struct JoinAstNode : public ast_node::Node {
    JoinLogicalParams logicalParams() const override {
        return JoinLogicalParams{
            .joinAlgorithm = selectBestJoinAlgorithm(),
            .leftKeys = extractLeftKeys(),
            .rightKeys = extractRightKeys(),
            .estimatedCardinality = computeCardinality()
        };
    }
};
```

## Current Implementation: Pass-Through

Both Foo and Bar use the **pass-through pattern** with standard LogicalParams:

### FooNode (Static Optimization)
```cpp
LogicalParams logicalParams() const override {
    return LogicalParams{
        .nodeType = "foo",
        .optimizationHint = "can_be_pushed_down"  // Fixed
    };
}
```

### BarNode (Dynamic Optimization)
```cpp
LogicalParams logicalParams() const override {
    return LogicalParams{
        .nodeType = "bar",
        .optimizationHint = flag ? "can_use_index" : "full_scan"  // Dynamic!
    };
}
```

**Note:** Even though both use LogicalParams, Bar computes the optimization hint dynamically based on its state. This shows flexibility within the pass-through pattern.

## Benefits of Symmetric Design

### ✅ Consistency
- Both transformers follow the same pattern
- Developers learn one pattern, apply it twice
- Code is easier to understand and maintain

### ✅ Extensibility
- Add new layers? Just add a new transformer
- Same pattern: virtual method + factory
- No special cases needed

### ✅ Testability
Each transformer can be tested independently:

```cpp
// Test parse → AST
auto parseNode = std::make_unique<FooNode>("data");
auto astNode = node_transformer::parseToAst(parseNode);
EXPECT_EQ(astNode->debugName(), "FooAstNode[foo]: ...");

// Test AST → Logical
auto astNode = std::make_unique<FooAstNode>(params);
auto logicalNode = ast_to_logical_transformer::astToLogical(astNode);
EXPECT_EQ(logicalNode->debugName(), "FooLogicalNode[foo]");
```

### ✅ Clean Dependencies
Each transformer only depends on adjacent layers:

```
node_transformer:
  deps: [parse_node, ast_node]

ast_to_logical_transformer:
  deps: [ast_node, logical_node]
```

No cross-dependencies or circular deps!

### ✅ Polymorphic All the Way
```cpp
// Complete polymorphic pipeline
auto parseNode = parse_node::createFromInput("foo", "data");  // Returns base pointer
auto astNode = node_transformer::parseToAst(parseNode);       // Returns base pointer
auto logicalNode = ast_to_logical_transformer::astToLogical(astNode);  // Returns base pointer

// Type-specific logic preserved at each step!
```

## Usage Pattern

### Individual Transformations
```cpp
// Parse → AST
auto parseNode = parse_node::createFromInput("foo", "data");
auto astNode = node_transformer::parseToAst(parseNode);

// AST → Logical
auto logicalNode = ast_to_logical_transformer::astToLogical(astNode);
```

### Complete Pipeline
```cpp
void processQuery(const std::string& nodeType, const std::string& input) {
    auto parseNode = parse_node::createFromInput(nodeType, input);
    auto astNode = node_transformer::parseToAst(parseNode);
    auto logicalNode = ast_to_logical_transformer::astToLogical(astNode);
    
    std::cout << logicalNode->explain() << std::endl;
}
```

### Type-Safe Direct Access (When Concrete Type Known)
```cpp
// If you know the concrete type at compile time, use it directly:
FooNode parseNode("data");
FooAstParams params = parseNode.astParams();  // Type-safe!
auto astNode = ast_node::create<FooAstParams>(params);  // Template specialization

// Or use the transformer for polymorphism:
auto astNode2 = node_transformer::parseToAst(parseNode);  // Also works!
```

## Extension: Adding More Transformers

To add a Physical layer, follow the same pattern:

### Step 1: Create PhysicalParams
```cpp
// include/physical_params.h
struct PhysicalParams {
    std::string nodeType;
    std::string executionMode;
    int numThreads;
};
```

### Step 2: Add createPhysicalNode() to Logical Nodes
```cpp
namespace logical_node {
    struct Node {
        virtual std::unique_ptr<physical_node::Node> createPhysicalNode() const = 0;
    };
}
```

### Step 3: Create Transformer
```cpp
// include/logical_to_physical_transformer.h
namespace logical_to_physical_transformer {
    std::unique_ptr<physical_node::Node> logicalToPhysical(
        const logical_node::Node& logicalNode
    ) {
        return logicalNode.createPhysicalNode();
    }
}
```

**Same pattern, just one more layer!**

## Dependency Graph

```
                    ┌──────────┐
                    │ toy_app  │
                    └────┬─────┘
                         │
        ┌────────────────┼────────────────┐
        │                │                │
        ↓                ↓                ↓
┌───────────────┐ ┌─────────────┐ ┌──────────────────────┐
│node_transformer│ │ast_to_logical│ │*_nodes_impl libraries│
└───┬────────┬──┘ └──┬────────┬─┘ └──────────────────────┘
    │        │       │        │
    ↓        ↓       ↓        ↓
┌──────┐ ┌────────┐┌────────┐┌───────────┐
│parse │ │  ast   ││  ast   ││  logical  │
│_node │ │ _node  ││ _node  ││  _node    │
└──────┘ └────────┘└────────┘└───────────┘
```

Clean, modular, and each transformer only touches two adjacent layers!

## Summary

The symmetric transformer pattern provides:

✅ **Consistent Design** - Same pattern at each layer boundary
✅ **Clean Separation** - Each transformer is independent
✅ **Type Safety** - Compile-time checks throughout
✅ **Polymorphism** - Work with base pointers when needed
✅ **Performance** - Single virtual call per transformation
✅ **Extensibility** - Easy to add more layers
✅ **Pass-Through Optimization** - Simple nodes share parameter types

This architecture elegantly bridges polymorphic input (string-based creation) with type-safe processing (template specializations) while maintaining clean module boundaries! 🌉

