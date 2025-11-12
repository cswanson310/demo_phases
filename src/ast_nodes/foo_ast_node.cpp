#include "foo_ast_node.h"
#include "ast_node.h"
#include <memory>

// The create<FooAstParams> specialization is already in the header
// No static registration needed since we use template specialization

// If you want runtime polymorphic registration, you can add it here
// But the template approach is more type-safe and doesn't require alwayslink
