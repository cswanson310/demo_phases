#include <iostream>
#include "lib.h"
#include "parse_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "src/parse_nodes/foo_node.h"
#include "src/ast_nodes/foo_ast_node.h"

int main() {
    std::cout << "=== Concept-Based Node Pipeline Demo ===" << std::endl << std::endl;
    
    std::cout << "2 + 2 = " << add(2, 2) << std::endl << std::endl;
    
    // Step 1: Create a parse node with type-specific params
    std::cout << "Step 1: Creating parse node" << std::endl;
    auto myParseNode = parse_node::createFromInput("my_input_data", "arg1");
    std::cout << "Parse shape: " << myParseNode->get_shape() << std::endl;
    
    // Step 2: Get type-specific AST params from parse node
    std::cout << "\nStep 2: Getting AST params from parse node" << std::endl;
    auto astParams = myParseNode->astParams();
    std::cout << "AST params: nodeType=" << astParams.nodeType 
              << ", fooData=" << astParams.fooSpecificData << std::endl;
    
    // Step 3: Create AST node using template specialization
    std::cout << "\nStep 3: Creating AST node from params" << std::endl;
    auto myAstNode = ast_node::create<FooAstParams>(astParams);
    std::cout << "AST debug name: " << myAstNode->debugName() << std::endl;
    
    // Step 4: Create logical node (using the old system for now)
    std::cout << "\nStep 4: Creating logical node" << std::endl;
    auto logicalParams = static_cast<FooAstNode*>(myAstNode.get())->logicalParams();
    auto myLogicalNode = logical_node::create(logicalParams);
    std::cout << "Logical debug name: " << myLogicalNode->debugName() << std::endl;
    std::cout << "\n" << myLogicalNode->explain() << std::endl;
    
    std::cout << "\n=== Pipeline Complete ===" << std::endl;
    
    return 0;
}
