// Ryan Alfa
#ifndef STRUCT_H
#define STRUCT_H

#include <stdlib.h> // For NULL definition

// Definition of a node in a Regular Expression Graph (REG)
typedef struct REG_node {
    struct REG_node *first_neighbor; // Pointer to the first neighbor
    char first_label;                // Label of the arrow to the first neighbor
    struct REG_node *second_neighbor;// Pointer to the second neighbor
    char second_label;               // Label of the arrow to the second neighbor
} REG_node;

// Definition of a REG graph
typedef struct REG {
    struct REG_node *start;  // Pointer to the start node of the REG
    struct REG_node *accept; // Pointer to the accept node of the REG
} REG;



// Declaration of the parse_expr function
// Parses a regular expression string and returns a pointer to the constructed REG
struct REG *parse_expr(const char *expression);

#endif 