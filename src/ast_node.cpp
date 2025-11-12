#include <map>
#include <iostream>
#include "include/ast_node.h"

namespace ast_node {

// The create function is now specialized per param type in the node headers
// This file just provides the registration infrastructure if needed

// For backward compatibility, we can keep a type-erased factory map
// but the primary mechanism is template specialization

} // namespace ast_node
