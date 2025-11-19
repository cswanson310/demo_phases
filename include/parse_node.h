#pragma once
#include <string>
#include <functional>
#include <memory>
#include <concepts>
#include "ast_params.h"

// Forward declarations
struct AstNode;

// Base class for parse nodes
struct ParseNode {
    virtual ~ParseNode() = default;
    virtual std::string get_shape() const = 0;
    
    // Returns parameters for creating the corresponding AstNode
    virtual AstParams astParams() const = 0;
};

// Factory registration for polymorphic usage
using ParseNodeFactory = std::function<std::unique_ptr<ParseNode>(std::string)>;
void registerParseNode(std::string name, ParseNodeFactory factory);
std::unique_ptr<ParseNode> createParseNodeFromInput(std::string name, std::string argString);

// Helper class for automatic registration
class ParseNodeRegistrar {
public:
    ParseNodeRegistrar(std::string name, ParseNodeFactory factory) {
        registerParseNode(name, factory);
    }
};

#define REGISTER_PARSE_NODE(name, factory) \
    static ParseNodeRegistrar _parse_registrar_##name(#name, factory)
