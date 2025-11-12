# Foo → Limit Node Refactoring

## Summary

Refactored the "foo" node into a more realistic "limit" node (similar to SQL's LIMIT operation) with a single `int` parameter wrapped in type-safe structs at each pipeline phase.

## Motivation

The "limit" node is a much more realistic database operation than the generic "foo" node, and demonstrates:
- **Single parameter simplicity**: Just one `int` value (the row limit)
- **Type safety**: Even simple parameters are wrapped in structs to prevent accidental confusion
- **Database realism**: LIMIT is a fundamental SQL operation for result set truncation

## Files Created

### Parameter Structs

1. **src/ast_params/limit_ast_params.h**
   ```cpp
   struct LimitAstParams : AstParams {
       int limitValue = 0;  // The number of rows to limit
   };
   ```

2. **src/logical_params/limit_logical_params.h**
   ```cpp
   struct LimitLogicalParams {
       std::string nodeType = "limit";
       std::string optimizationHint;
       int rowLimit = 0;  // How many rows to return
   };
   ```

### Node Implementations

3. **src/parse_nodes/limit_node.{h,cpp}**
   - `LimitNode` parses input and produces `LimitAstParams`
   - Sets `limitValue = 100` as an example

4. **src/ast_nodes/limit_ast_node.{h,cpp}**
   - `LimitAstNode` performs AST-level analysis
   - Computes optimization hint based on limit value:
     - Small limits (≤1000): `"can_materialize"`
     - Large limits (>1000): `"may_need_streaming"`
   - Produces `LimitLogicalParams` with `rowLimit`

5. **src/logical_nodes/limit_logical_node.{h,cpp}**
   - `LimitLogicalNode` generates execution plan
   - Shows:
     - Row limit
     - Push-down capability
     - Estimated memory usage (rowLimit × 100 bytes)

## Files Deleted

Removed all "foo" related files:
- `src/ast_params/foo_ast_params.h`
- `src/logical_params/foo_logical_params.h`
- `src/parse_nodes/foo_node.{h,cpp}`
- `src/ast_nodes/foo_ast_node.{h,cpp}`
- `src/logical_nodes/foo_logical_node.{h,cpp}`

## Files Modified

### BUILD.bazel
- Renamed all `foo_*` targets to `limit_*`:
  - `limit_ast_params`
  - `limit_logical_params`
  - `limit_parse_node`
  - `limit_ast_nodes`
  - `limit_logical_nodes`
- Updated `parse_nodes_impl`, `ast_nodes_impl`, `logical_nodes_impl` dependencies

### src/main.cpp
- Changed node processing from `"foo"` to `"limit"`
- Updated summary text to highlight the simplicity vs complexity contrast:
  - **LimitNode**: Simple single `int` parameter
  - **BarNode**: Complex multi-type parameters

## Key Features

### 1. Type Safety with Minimal Overhead

Even though the limit is just an `int`, it's wrapped in type-safe structs:

```cpp
// Parse phase
struct LimitAstParams : AstParams {
    int limitValue = 0;
};

// Logical phase
struct LimitLogicalParams {
    int rowLimit = 0;
};
```

This prevents accidentally passing the wrong integer to the wrong node type.

### 2. Semantic Clarity

Different parameter names at each phase clarify the purpose:
- **Parse phase**: `limitValue` (raw input)
- **Logical phase**: `rowLimit` (execution plan detail)

### 3. Optimization Logic

The AST node makes optimization decisions based on the limit value:

```cpp
LimitLogicalParams logicalParams() const {
    return LimitLogicalParams{
        .nodeType = "limit",
        .optimizationHint = limitValue > 1000 ? "may_need_streaming" : "can_materialize",
        .rowLimit = limitValue
    };
}
```

### 4. Realistic Database Operation

The execution plan shows database-relevant information:

```
LOGICAL_PLAN:
  Operation: Limit
  Type: limit
  Row Limit: 100
  Optimization: can_materialize
  Can Push Down: Yes
  Estimated Memory: 10000 bytes
```

## Contrast: Simple vs Complex Parameters

The system now demonstrates two extremes:

### LimitNode (Simple)
```
Parse → AST → Logical
  int → int → int (each wrapped in type-safe struct)
```

**Use case**: Straightforward operations with minimal state

### BarNode (Complex)
```
Parse → AST → Logical
  vector<string>, bool, double → bool, int, double (multiple fields)
```

**Use case**: Complex operations requiring rich metadata

## Testing

All tests pass:
- ✅ Unit tests
- ✅ Integration test (toy_app)
- ✅ No linter errors

Output shows the limit node working correctly:

```
[Startup] Registering node type: limit
[Startup] Registering node type: bar

Processing "limit" node
  Shape: limit_shape
  LimitAstNode[limit]: ... (limit=100)
  LimitLogicalNode[limit]
  
  LOGICAL_PLAN:
    Operation: Limit
    Row Limit: 100
    Can Push Down: Yes
    Estimated Memory: 10000 bytes
```

## Benefits

### 1. **More Realistic Example**
Limit is a real database operation everyone understands

### 2. **Demonstrates Simplicity**
Shows that even simple operations benefit from the type-safe architecture

### 3. **Type Safety Maintained**
Even a single `int` gets proper struct wrapping to prevent errors

### 4. **Clear Contrast**
LimitNode (simple) vs BarNode (complex) showcases the flexibility

### 5. **No Loss of Generality**
The architecture handles both simple and complex cases equally well

## Related Documentation

- [TYPE_SPECIFIC_PARAMS.md](TYPE_SPECIFIC_PARAMS.md) - Type-specific parameter pattern
- [NODE_PIPELINE_GUIDE.md](NODE_PIPELINE_GUIDE.md) - How to add new node types
- [ARCHITECTURE.md](ARCHITECTURE.md) - Overall system architecture
- [CONCEPTS_GUIDE.md](CONCEPTS_GUIDE.md) - C++20 concepts usage

