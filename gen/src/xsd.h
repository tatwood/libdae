#ifndef XSD_H_
#define XSD_H_

#include <stddef.h>

#define xsd_DATA_NONE NULL
#define xsd_DATA_BOOL   "xs:boolean"
#define xsd_DATA_HEX    "xs:hexBinary"
#define xsd_DATA_FLOAT  "xs:float"
#define xsd_DATA_INT8   "xs:byte"
#define xsd_DATA_INT16  "xs:short"
#define xsd_DATA_INT32  "xs:int"
#define xsd_DATA_STRING "xs:string"
#define xsd_DATA_UINT8  "xs:unsignedByte"
#define xsd_DATA_UINT16 "xs:unsignedShort"
#define xsd_DATA_UINT32 "xs:unsignedInt"

enum
{
    xsd_UNBOUNDED = -1,
    xsd_UNSET     = -2
};

enum xsd_base_relation_e
{
    xsd_BASE_NONE,
    xsd_BASE_EXTEND,
    xsd_BASE_RESTRICTION,
    xsd_BASE_RENAME
};

typedef enum xsd_base_relation_e xsd_base_relation;
typedef struct xsd_attrib_s xsd_attrib;
typedef struct xsd_element_s xsd_element;
typedef struct xsd_group_s xsd_group;
typedef struct xsd_type_s xsd_type;
typedef struct xsd_schema_s xsd_schema;

struct xsd_attrib_s
{
    char* name;
    char* type;
    char* ref;
    char* deflt;
    int required;
};

struct xsd_element_s
{
    char* name;
    char* ref;
    char* type;
    int minoccurs;
    int maxoccurs;
    int seq;
};

struct xsd_group_s
{
    char* name;
    char* ref;
    xsd_element** elements;
    size_t numelements;
    int seqmax;
    int seq;
};

struct xsd_type_s
{
    char* name;
    char* base;
    xsd_base_relation baserelation;
    char isanonymous;
    char isbuiltin;
    char hassimple;
    char hascomplex;
    char anycontent;
    struct
    {
        char* itemtype;
        int minoccurs;
        int maxoccurs;
    } simple;
    struct
    {
        xsd_attrib** attribs;
        xsd_element** elements;
        xsd_group** groups;
        size_t numattribs;
        size_t numelements;
        size_t numgroups;
        int seqmax;
    } complex;
};

struct xsd_schema_s
{
    /// top level attributes
    xsd_attrib** attribs;
    /// top level elements
    xsd_element** elements;
    /// top level groups
    xsd_group** groups;
    /// list of types defined by schema
    xsd_type** types;
    size_t numattribs;
    size_t numelements;
    size_t numgroups;
    size_t numtypes;
};

//****************************************************************************

xsd_attrib* xsd_add_attrib_global(
    xsd_schema* schema,
    const char* name,
    const char* type,
    const char* ref,
    const char* deflt,
    int required);

xsd_attrib* xsd_add_attrib_to_type(
    xsd_schema* schema,
    xsd_type* t,
    const char* name,
    const char* type,
    const char* ref,
    const char* deflt,
    int required);

xsd_element* xsd_add_element_global(
    xsd_schema* schema,
    const char* name,
    const char* type,
    const char* ref,
    int minoccurs,
    int maxoccurs);

xsd_element* xsd_add_element_to_group(
    xsd_schema* schema,
    xsd_group* grp,
    const char* name,
    const char* type,
    const char* ref,
    int minoccurs,
    int maxoccurs,
    int seq);

xsd_element* xsd_add_element_to_type(
    xsd_schema* schema,
    xsd_type* t,
    const char* name,
    const char* type,
    const char* ref,
    int minoccurs,
    int maxoccurs,
    int seq);

xsd_group* xsd_add_group_global(
    xsd_schema* schema,
    const char* name,
    const char* ref);

xsd_group* xsd_add_group_to_type(
    xsd_schema* schema,
    xsd_type* t,
    const char* name,
    const char* ref,
    int seq);

xsd_type* xsd_add_type(
    xsd_schema* schema,
    const char* name);

void xsd_create_schema(
    xsd_schema* schema_out);

void xsd_destroy_schema(
    xsd_schema* schema);

xsd_attrib* xsd_find_attrib(
    xsd_schema* schema,
    const char* name);

xsd_element* xsd_find_element(
    xsd_schema* schema,
    const char* name);

xsd_group* xsd_find_group(
    xsd_schema* schema,
    const char* name);

xsd_type* xsd_find_type(
    xsd_schema* schema,
    const char* name);

void xsd_flatten_schema(
    xsd_schema* schema);

void xsd_remove_type(
    xsd_schema* schema,
    size_t index);

void xsd_remove_unreferenced_types(
    xsd_schema* schema);

char* xsd_strdup(
    const char* str);

int xsd_validate_schema(
    xsd_schema* schema);

#endif // XSD_H_
