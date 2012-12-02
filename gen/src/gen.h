#ifndef GEN_H_
#define GEN_H_

#include "xsd.h"
#include <stdio.h>

void gen_print_typeids(
    FILE* fp,
    xsd_schema* schema);

void gen_print_typedefs(
    FILE* fp,
    xsd_schema* schema);

void gen_print_structs(
    FILE* fp,
    xsd_schema* schema);

void gen_print_schema(
    FILE* fp,
    xsd_schema* schema);

#endif // GEN_H_
