/**
 * @brief     header for libdae code generation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef GEN_H_
#define GEN_H_

#include <stddef.h>

typedef size_t gen_typeid;
typedef enum gen_datatype_e gen_datatype;
typedef enum gen_typeclass_e gen_typeclass;
typedef struct gen_attribute_s gen_attr;
typedef struct gen_choice_s gen_choice;
typedef struct gen_complex_s gen_complex;
typedef struct gen_element_s gen_element;
typedef struct gen_list_s gen_list;
typedef struct gen_schema_s gen_schema;
typedef struct gen_typeheader_s gen_typeheader;
typedef union gen_type_u gen_type;

enum gen_datatype_e
{
    gen_DATA_NONE,
    gen_DATA_BOOL,
    gen_DATA_FLOAT,
    gen_DATA_INT,
    gen_DATA_STRING,
    gen_DATA_UINT
};

enum gen_typeclass_e
{
    gen_TYPECLASS_CHOICE,
    gen_TYPECLASS_COMPLEX,
    gen_TYPECLASS_LIST
};

struct gen_attribute_s
{
    char* xml;
    char* name;
    gen_datatype datatype;
};

struct gen_element_s
{
    char* xml;
    char* name;
    gen_typeid type;
    int minoccurs;
    int maxoccurs;
};

struct gen_typeheader_s
{
    gen_typeclass typeclass;
    char* name;
    gen_typeid* dependencies;
    size_t numdependencies;
};

struct gen_choice_s
{
    gen_typeheader header;
    gen_typeid* types;
    size_t numtypes;
};

struct gen_complex_s
{
    gen_typeheader header;
    char* enumname;
    gen_attr* attributes;
    gen_element* elements;
    size_t numattributes;
    size_t numelements;
    gen_datatype data;
    int mindata;
    int maxdata;
};

struct gen_list_s
{
    gen_typeheader header;
    gen_typeid itemtype;
};

union gen_type_u
{
    gen_typeheader header;
    gen_choice choice;
    gen_list list;
    gen_complex complex;
};

struct gen_schema_s
{
    gen_type* types;
    size_t numtypes;
};

gen_typeid gen_add_type(
    gen_schema* schema,
    gen_typeclass typeclass,
    const char* name);

void gen_add_dependency(
    gen_schema* schema,
    gen_typeid type,
    gen_typeid depends);

gen_typeid gen_add_choice(
    gen_schema* schema,
    const char* name,
    const char** types,
    size_t numtypes);

gen_typeid gen_add_complex(
    gen_schema* schema,
    const char* name);

size_t gen_add_list(
    gen_schema* schema,
    const char* name,
    gen_typeid itemtype);

int gen_find_list(
    gen_schema* schema,
    gen_typeid itemtype);

int gen_find_type(
    gen_schema* schema,
    const char* name);

size_t gen_add_attr(
    gen_schema* schema,
    gen_typeid complex,
    const char* xml,
    const char* name,
    gen_datatype datatype);

int gen_add_element(
    gen_schema* schema,
    gen_typeid complex,
    const char* xml,
    const char* name,
    const char* type,
    int minoccurs,
    int maxoccurs);

void gen_set_data(
    gen_schema* schema,
    gen_typeid complex,
    gen_datatype datatype,
    int minoccurs,
    int maxoccurs);

#endif // DAEGEN_H_
