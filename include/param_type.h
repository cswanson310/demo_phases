#pragma once
#include <type_traits>

// Concept defining what it means to be a valid parameter type
template <typename T>
concept ParamType = std::is_default_constructible_v<T> && 
                    std::is_copy_constructible_v<T> && 
                    std::is_copy_assignable_v<T>;
