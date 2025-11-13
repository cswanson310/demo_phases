#include <iostream>
#include "lib.h"
#include "parse_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "node_transformer.h"
#include "ast_to_logical_transformer.h"
#include "src/ast_nodes/limit_ast_node.h"
#include "src/ast_nodes/sort_ast_node.h"
#include "src/ast_nodes/set_metadata_ast_node.h"
#include "src/parse_nodes/limit_node.h"
#include "src/parse_nodes/sort_node.h"
#include "src/parse_nodes/set_metadata_node.h"

void processNode(const std::string& nodeType, const std::string& inputData) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Processing \"" << nodeType << "\" node" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Step 1: Create parse node polymorphically (from string input)
    std::cout << "\n[1] Creating parse node from input: \"" << inputData << "\"" << std::endl;
    auto parseNode = createParseNodeFromInput(nodeType, inputData);
    std::cout << "    Shape: " << parseNode->get_shape() << std::endl;
    
    // Step 2: Transform to AST node using the transformer
    std::cout << "\n[2] Transforming to AST node (via node_transformer)" << std::endl;
    auto astNode = parseToAst(*parseNode);
    std::cout << "    " << astNode->debugName() << std::endl;
    
    // Step 3: Transform to logical node using the AST→Logical transformer
    std::cout << "\n[3] Transforming to logical node (via ast_to_logical_transformer)" << std::endl;
    auto logicalNode = astToLogical(*astNode);
    std::cout << "    " << logicalNode->debugName() << std::endl;
    
    // Step 4: Show execution plan
    std::cout << "\n[4] Execution Plan:" << std::endl;
    std::cout << logicalNode->explain() << std::endl;
}

int main() {
    std::cout << "╔════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Multi-Type Node Pipeline Demonstration   ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════╝" << std::endl;
    
    // Demonstrate three different node types with different parameter structures
    processNode("limit", "100");
    processNode("sort", "field1,field2:asc");
    processNode("set_metadata", "score:sum(user_score, daily_bonus)");
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n✅ All three node types processed successfully!" << std::endl;
    std::cout << "\nKey observations:" << std::endl;
    std::cout << "  • LimitNode: Single int parameter" << std::endl;
    std::cout << "    - LimitParams { limitValue: int }" << std::endl;
    std::cout << "  • SortNode: Vector of strings + bool" << std::endl;
    std::cout << "    - SortParams { sortKeys: vector<string>, ascending: bool }" << std::endl;
    std::cout << "  • SetMetadataNode: Two strings" << std::endl;
    std::cout << "    - SetMetadataParams { metaName: string, expression: string }" << std::endl;
    std::cout << "  • Each uses the SAME param struct through all phases (Parse → AST → Logical)" << std::endl;
    std::cout << "  • Transformers handle all node types polymorphically" << std::endl;
    std::cout << "  • No runtime type checks or casts needed!" << std::endl;
    std::cout << "  • No namespaces - just clean distinct names!" << std::endl;
    
    return 0;
}
