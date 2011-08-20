/**
 * @brief     functions for libdae header code generation
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

//****************************************************************************
static void gen_writechoiceunion(
    const gen_schema* schema,
    gen_typeid choice,
    FILE* fp)
{
    const gen_choice* pchoice = &schema->types[choice].choice;
    const gen_typeid* titr = pchoice->types;
    const gen_typeid* tend = titr + pchoice->numtypes;
    const char* name = pchoice->header.name;
    fprintf(fp, "union dae_%s_u\n{\n", name);
    fprintf(fp, "    dae_typebase daebase;\n");
    while(titr != tend)
    {
        const gen_type* pitem = schema->types + *titr; 
        const char* itemname = pitem->header.name;
        fprintf(fp, "    dae_%s %s;\n", itemname, itemname);
        ++titr;
    }
    fprintf(fp, "};\n\n");
}

//****************************************************************************
static void gen_writecomplexstruct(
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
    fprintf(fp, "struct dae_%s_s\n{\n", name);
    fprintf(fp, "    dae_typebase daebase;\n");
    while(attr != attrend)
    {
        switch(attr->datatype)
        {
        case gen_DATA_NONE:
            assert(0);
            break;
        case gen_DATA_BOOL:
            fprintf(fp, "    int %s;\n", attr->name);
            break;
        case gen_DATA_FLOAT:
            fprintf(fp, "    float %s;\n", attr->name);
            break;
        case gen_DATA_INT:
            fprintf(fp, "    int %s;\n", attr->name);
            break;
        case gen_DATA_STRING:
            fprintf(fp, "    char* %s;\n", attr->name);
            break;
        case gen_DATA_UINT:
            fprintf(fp, "    unsigned %s;\n", attr->name);
            break;
        }
        ++attr;
    }
    if(pcomplex->data != gen_DATA_NONE)
    {
        const char* data = "";
        switch(pcomplex->data)
        {
        case gen_DATA_NONE:
            break;
        case gen_DATA_BOOL:
            data = "int";
            break;
        case gen_DATA_FLOAT:
            data = "float";
            break;
        case gen_DATA_INT:
            data = "int";
            break;
        case gen_DATA_STRING:
            data = "char*";
            break;
        case gen_DATA_UINT:
            data = "unsigned";
            break;
        default:
            assert(0);
            break;
        }
        if(pcomplex->maxdata == 1)
        {
            fprintf(fp, "    %s data;\n", data);
        }
        else if(pcomplex->mindata == pcomplex->maxdata)
        {
            fprintf(fp, "    %s data[%d];\n", data, pcomplex->maxdata);
        }
        else
        {
            fprintf(fp, "    %s* data;\n", data);
            fprintf(fp, "    size_t datasize;\n");
        }
    }
    while(elem != elemend)
    {
        gen_type* eltype = schema->types + elem->type;
        const char* elemname = elem->name;
        const char* elemtype = eltype->header.name;
        if(elem->minoccurs == 0 && elem->maxoccurs == 1)
        {
            fprintf(fp,"    dae_%s* %s;\n",elemtype, elemname);
        }
        else if(elem->minoccurs == 1 && elem->maxoccurs == 1)
        {
            fprintf(fp,"    dae_%s %s;\n",elemtype, elemname);
        }
        else
        {
            // should have been converted to list
            assert(0);
        }
        ++elem;
    }
    fprintf(fp, "};\n\n");
}

//****************************************************************************
static void gen_writeliststruct(
    const gen_schema* schema,
    gen_typeid list,
    FILE* fp)
{
    const gen_list* plist = &schema->types[list].list;
    const gen_type* pitem = schema->types + plist->itemtype;
    const char* name = plist->header.name;
    const char* itemname = pitem->header.name;
    fprintf(fp, "struct dae_%s_s\n{\n", name);
    fprintf(fp, "    dae_listbase daebase;\n");
    fprintf(fp, "    dae_%s* values;\n", itemname);
    fprintf(fp, "    size_t size;\n");
    fprintf(fp, "};\n\n");
}

//****************************************************************************
static void gen_writeenums(
    const gen_schema* schema,
    FILE* fp)
{
    char* uprstr = NULL;
    size_t uprsize = 0;
    const gen_type* ptype = schema->types;
    const gen_type* ptypeend = ptype + schema->numtypes;
    fprintf(fp, "enum dae_typeid_e\n{\n");
    fprintf(fp, "    dae_ID_INVALID = -1,\n");
    while(ptype != ptypeend)
    {
        gen_typeclass tc = ptype->header.typeclass;
        if(tc==gen_TYPECLASS_COMPLEX || tc==gen_TYPECLASS_CHOICE)
        {
            const char* lwr = ptype->header.name;
            char* upr;
            size_t size = strlen(lwr) + 1;
            if(uprsize != size)
            {
                uprsize = (size + 32) & (~31);
                uprstr = (char*) realloc(uprstr, uprsize);
            }
            upr = uprstr;
            while(*lwr != '\0')
            {
                *upr = (char) toupper(*lwr);
                ++upr;
                ++lwr;
            }
            *upr = '\0';
            fprintf(fp, "    dae_ID_%s,\n",uprstr);
        }
        ++ptype;
    }
    fprintf(fp, "};\n\n");
    free(uprstr);
}

//****************************************************************************
static void gen_writetypedefs(
    const gen_schema* schema,
    FILE* fp)
{
    const gen_type* ptype = schema->types;
    const gen_type* ptypeend = ptype + schema->numtypes;
    while(ptype != ptypeend)
    {
        const char* s = ptype->header.name;
        switch(ptype->header.typeclass)
        {
        case gen_TYPECLASS_CHOICE:
            fprintf(fp, "typedef union dae_%s_u dae_%s;\n", s, s);
            break;
        case gen_TYPECLASS_COMPLEX:
            fprintf(fp, "typedef struct dae_%s_s dae_%s;\n", s, s);
            break;
        case gen_TYPECLASS_LIST:
            fprintf(fp, "typedef struct dae_%s_s dae_%s;\n", s, s);
            break;
        }
        ++ptype;
    }
    fprintf(fp, "\n");
}

//****************************************************************************
static void gen_writestructs(
    const gen_schema* schema,
    FILE* fp)
{
    int* isexported = (int*) calloc(sizeof(*isexported), schema->numtypes);
    size_t numexported = 0;

    fprintf(fp, "struct dae_listbase_s\n");
    fprintf(fp, "{\n");
    fprintf(fp, "    dae_typeid type;\n");
    fprintf(fp, "};\n\n");
    fprintf(fp, "struct dae_typebase_s\n");
    fprintf(fp, "{\n");
    fprintf(fp, "    dae_typeid type;\n");
    fprintf(fp, "};\n\n");

    while(numexported != schema->numtypes)
    {
        gen_type* ptype = schema->types;
        gen_type* ptypeend = ptype + schema->numtypes;
        gen_typeid i = 0;
        while(ptype != ptypeend)
        {
            int canexport = !isexported[i];
            gen_typeid* ditr = ptype->header.dependencies;
            gen_typeid* dend = ditr + ptype->header.numdependencies;
            if(canexport)
            {
                while(ditr != dend)
                {
                    if(!isexported[*ditr])
                    {
                        canexport = 0;
                        break;
                    }
                    ++ditr;
                }
            }
            if(canexport)
            {
                switch(ptype->header.typeclass)
                {
                case gen_TYPECLASS_CHOICE:
                    gen_writechoiceunion(schema, i, fp);
                    break;
                case gen_TYPECLASS_COMPLEX:
                    gen_writecomplexstruct(schema, i, fp);
                    break;
                case gen_TYPECLASS_LIST:
                    gen_writeliststruct(schema, i, fp);
                    break;
                }
                isexported[i] = 1;
                ++numexported;
            }
            ++ptype;
            ++i;
        }
    }

    free(isexported);
}

void gen_writeheader(
    const gen_schema* schema,
    FILE* fp)
{
    gen_writeenums(schema, fp);
    gen_writetypedefs(schema, fp);
    gen_writestructs(schema, fp);
}
