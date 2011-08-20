/**
 * @brief     functions for libdae c code generation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#include "gen.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* gen_makeuprstr(const char* s)
{
    size_t size = strlen(s) + 1;
    char* uprstr = (char*) malloc(size);
    char* upr = uprstr;
    while(*s != '\0')
    {
        *upr = (char) toupper(*s);
        ++s;
        ++upr;
    }
    *upr = '\0';
    return uprstr;
}

static void gen_writechoiceinfo(
    const gen_schema* schema,
    gen_typeid choice,
    FILE* fp)
{
    const gen_choice* pchoice = &schema->types[choice].choice;
    const gen_typeid* titr = pchoice->types;
    const gen_typeid* tend = titr + pchoice->numtypes;
    const char* name = pchoice->header.name;
    char* uprname = gen_makeuprstr(name);
    int i;
    fprintf(fp, "    {\n");
    fprintf(fp, "        dae_structinfo* def = structs + dae_ID_%s;\n",uprname);
    if(pchoice->numtypes != 0)
    {
        size_t n = pchoice->numtypes;
        fprintf(fp, "        static dae_eleminfo elem[%d];\n", n);
    }
    i = 0;
    while(titr != tend)
    {
        const gen_type* ttype = schema->types + *titr; 
        char* tupr = gen_makeuprstr(ttype->header.name);
        const char* s = ttype->header.name;
        const char* e = name;
        fprintf(fp, "        elem[%d].name = \"%s\";\n",i,s);
        fprintf(fp, "        elem[%d].dae_type = dae_ID_%s;\n",i,tupr);
        fprintf(fp, "        elem[%d].restrict = dae_RESTRICT_CHOICE;\n",i);
        fprintf(fp, "        elem[%d].offset = offsetof(dae_%s,%s);\n",i,e,s);
        free(tupr);
        ++titr;
        ++i;
    }
    fprintf(fp, "        def->attrs = NULL;\n");
    if(pchoice->numtypes != 0)
    {
        fprintf(fp, "        def->elems = elem;\n");
    }
    else
    {
        fprintf(fp, "        def->elems = NULL;\n");
    }
    fprintf(fp, "        def->datatype = dae_DATA_NONE;\n");
    fprintf(fp, "        def->dataoffset = -1;\n");
    fprintf(fp, "        def->mindata = 0;\n");
    fprintf(fp, "        def->maxdata = 0;\n");
    fprintf(fp, "        def->isunion = 1;\n");
    fprintf(fp, "        def->numattrs = 0;\n");
    fprintf(fp, "        def->numelems = %d;\n", pchoice->numtypes);
    fprintf(fp, "        def->size = sizeof(dae_%s);\n", name);
    fprintf(fp, "    }\n");
    free(uprname);
}

static void gen_writecomplexinfo(
    const gen_schema* schema,
    gen_typeid complex,
    FILE* fp)
{
    const gen_complex* pcomplex = &schema->types[complex].complex;
    gen_attr* attr = pcomplex->attributes;
    gen_attr* attrend = attr + pcomplex->numattributes;
    gen_element* elem = pcomplex->elements;
    gen_element* elemend = elem + pcomplex->numelements;
    const char* name = pcomplex->header.name;
    char* uprname = gen_makeuprstr(name);
    int i;
    fprintf(fp, "    {\n");
    fprintf(fp, "        dae_structinfo* def = structs + dae_ID_%s;\n",uprname);
    if(pcomplex->numattributes != 0)
    {
        size_t n = pcomplex->numattributes;
        fprintf(fp, "        static dae_attrinfo attr[%d];\n", n);
    }
    if(pcomplex->numelements != 0)
    {
        size_t n = pcomplex->numelements;
        fprintf(fp, "        static dae_eleminfo elem[%d];\n", n);
    }
    i = 0;
    while(attr != attrend)
    {
        const char* s = name;
        const char* a = attr->name;
        fprintf(fp, "        attr[%d].name = \"%s\";\n", i, attr->xml);
        fprintf(fp, "        attr[%d].offset = offsetof(dae_%s,%s);\n",i,s,a);
        switch(attr->datatype)
        {
        case gen_DATA_NONE:
            assert(0);
            break;
        case gen_DATA_BOOL:
            fprintf(fp, "        attr[%d].datatype = dae_DATA_BOOL;\n", i);
            fprintf(fp, "        attr[%d].defaultval.bool_val = %d;\n",i,0);
        case gen_DATA_FLOAT:
            fprintf(fp, "        attr[%d].datatype = dae_DATA_FLOAT;\n", i);
            fprintf(fp, "        attr[%d].defaultval.float_val = %f;\n",i,0.0f);
        case gen_DATA_INT:
            fprintf(fp, "        attr[%d].datatype = dae_DATA_INT;\n", i);
            fprintf(fp, "        attr[%d].defaultval.int_val = %d;\n",i,0);
            break;
        case gen_DATA_STRING:
            fprintf(fp, "        attr[%d].datatype = dae_DATA_STRING;\n", i);
            fprintf(fp, "        attr[%d].defaultval.string_val = %s;\n",i,"NULL");
            break;
        case gen_DATA_UINT:
            fprintf(fp, "        attr[%d].datatype = dae_DATA_UINT;\n", i);
            fprintf(fp, "        attr[%d].defaultval.uint_val = %d;\n",i,0);
            break;
        }
        ++attr;
        ++i;
    }
    i = 0;
    while(elem != elemend)
    {
        gen_type* eltype = schema->types + elem->type;
        char* elupr;
        const char* s = name;
        const char* e = elem->name;
        const char* restrict = "";
        if(eltype->header.typeclass == gen_TYPECLASS_LIST)
        {
            restrict = "dae_RESTRICT_LIST";
            eltype = schema->types + eltype->list.itemtype;
        }
        else if(elem->minoccurs == 0 && elem->maxoccurs == 1)
        {
            restrict = "dae_RESTRICT_OPTIONAL";
        }
        else if(elem->minoccurs == 1 && elem->maxoccurs == 1)
        {
            restrict = "dae_RESTRICT_REQUIRED";
        }
        else
        {
            assert(0);
        }
        elupr = gen_makeuprstr(eltype->header.name);
        fprintf(fp, "        elem[%d].name = \"%s\";\n",i,elem->xml);
        fprintf(fp, "        elem[%d].dae_type = dae_ID_%s;\n",i,elupr);
        fprintf(fp, "        elem[%d].restrict = %s;\n",i,restrict);
        fprintf(fp, "        elem[%d].offset = offsetof(dae_%s,%s);\n",i,s,e);
        free(elupr);
        ++elem;
        ++i;
    }
    if(pcomplex->numattributes != 0)
    {
        fprintf(fp, "        def->attrs = attr;\n");
    }
    else
    {
        fprintf(fp, "        def->attrs = NULL;\n");
    }
    if(pcomplex->numelements != 0)
    {
        fprintf(fp, "        def->elems = elem;\n");
    }
    else
    {
        fprintf(fp, "        def->elems = NULL;\n");
    }
    if(pcomplex->data != gen_DATA_NONE)
    {
        const char* data = "";
        switch(pcomplex->data)
        {
        case gen_DATA_NONE: break;
        case gen_DATA_BOOL: data = "dae_DATA_BOOL"; break;
        case gen_DATA_FLOAT: data = "dae_DATA_FLOAT"; break;
        case gen_DATA_INT: data = "dae_DATA_INT"; break;
        case gen_DATA_STRING: data = "dae_DATA_STRING"; break;
        case gen_DATA_UINT: data = "dae_DATA_UINT"; break;
        }
        fprintf(fp, "        def->datatype = %s;\n", data);
        fprintf(fp, "        def->dataoffset = offsetof(dae_%s,data);\n",name);
    }
    else
    {
        fprintf(fp, "        def->datatype = dae_DATA_NONE;\n");
        fprintf(fp, "        def->dataoffset = -1;\n");
    }
    fprintf(fp, "        def->mindata = %d;\n", pcomplex->mindata);
    fprintf(fp, "        def->maxdata = %d;\n", pcomplex->maxdata);
    fprintf(fp, "        def->isunion = 0;\n");
    fprintf(fp, "        def->numattrs = %d;\n", pcomplex->numattributes);
    fprintf(fp, "        def->numelems = %d;\n", pcomplex->numelements);
    fprintf(fp, "        def->size = sizeof(dae_%s);\n", name);
    fprintf(fp, "    }\n");
    free(uprname);
}

static void gen_writestructinfo(
    const gen_schema* schema,
    FILE* fp)
{
    const gen_type* ptype = schema->types;
    const gen_type* ptypeend = ptype + schema->numtypes;
    gen_typeid i = 0;
    while(ptype != ptypeend)
    {
        switch(ptype->header.typeclass)
        {
        case gen_TYPECLASS_CHOICE:
            gen_writechoiceinfo(schema, i, fp);
            break;
        case gen_TYPECLASS_COMPLEX:
            gen_writecomplexinfo(schema, i, fp);
            break;
        case gen_TYPECLASS_LIST:
            break;
        }
        ++ptype;
        ++i;
    }
}

void gen_writesource(
    const gen_schema* schema,
    FILE* fp)
{
    fprintf(fp, "static const dae_schemainfo* dae_construct_schema()\n{\n");
    fprintf(fp, "    static dae_schemainfo schema;\n");
    fprintf(fp, "    static dae_structinfo structs[%d];\n", schema->numtypes);
    gen_writestructinfo(schema, fp);
    fprintf(fp, "    schema.structs = structs;\n");
    fprintf(fp, "    schema.numstructs = sizeof(structs)/sizeof(*structs);\n");
    fprintf(fp, "    return &schema;\n");
    fprintf(fp, "}\n");
}
