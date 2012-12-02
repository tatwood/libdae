#ifndef XSDPARSE_H_
#define XSDPARSE_H_

#include "xsd.h"

//****************************************************************************

typedef struct xsdparse_node_s xsdparse_node;
typedef struct xsdparse_state_s xsdparse_state;

//****************************************************************************

struct xsdparse_node_s
{
    xsd_type* type;
    xsd_group* group;
    xsd_attrib* attrib;
    xsd_element* element;
    int min;
    int max;
    int inseq;
    int seq;
    int childseq;
};

struct xsdparse_state_s
{
    xsdparse_node* stack;
    int stacksize;
    xsd_schema* schema;
};

//****************************************************************************

void xsdparse_startelement(
    void* userdata,
    const char* el,
    const char** att);

void xsdparse_endelement(
    void* userdata,
    const char* el);


/**
 * @details Manually alter the schema for compatibility purposes
 */
void xsdparse_hacks(
    xsd_schema* schema);

#endif // XSDPARSE_H_
