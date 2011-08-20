/**
 * @brief     core functions for libdae code generation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#include "gen.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

//****************************************************************************
static char* gen_dupstr(
     const char* s)
{
     char* dup = NULL;
     if(s != NULL)
     {
          size_t size = strlen(s) + 1;
          dup = (char*) malloc(sizeof(*dup) * size);
          memcpy(dup, s, sizeof(*dup) * size);
     }
     return dup;
}

//****************************************************************************
gen_typeid gen_add_type(
    gen_schema* schema,
    gen_typeclass typeclass,
    const char* name)
{
    size_t id = schema->numtypes;
    size_t cap = (id + 127) & (~127);
    size_t ncap = (id + 128) & (~127);
    gen_type* ptype = schema->types;
    gen_typeheader* ph;
    if(ncap > cap)
    {
        ptype = (gen_type*) realloc(ptype, sizeof(*ptype) * ncap);
        schema->types = ptype;
    }
    ptype += id;
    memset(ptype, 0, sizeof(*ptype));
    ph = &ptype->header;
    ph->typeclass = typeclass;
    ph->name = gen_dupstr(name);

    ++schema->numtypes;
    return id;
}

//****************************************************************************
void gen_add_dependency(
    gen_schema* schema,
    gen_typeid type,
    gen_typeid depends)
{
    gen_typeheader* ph = &schema->types[type].header;
    gen_typeid* pd = ph->dependencies;
    size_t n = ph->numdependencies;
    size_t cap = (n + 15) & (~15);
    size_t ncap = (n + 16) & (~15);
    if(ncap > cap)
    {
        pd = (gen_typeid*) realloc(pd, sizeof(*pd) * ncap);
        ph->dependencies = pd;
    }
    pd[n] = depends;
    ++ph->numdependencies;
}

//****************************************************************************
gen_typeid gen_add_choice(
    gen_schema* schema,
    const char* name,
    const char** types,
    size_t numtypes)
{
    int id = gen_find_type(schema, name);
    if(id < 0)
    {
        gen_typeid nid = gen_add_type(schema, gen_TYPECLASS_CHOICE, name);
        gen_choice* pchoice = &schema->types[nid].choice;
        const char** tname = types;
        gen_typeid* titr = (gen_typeid*) malloc(sizeof(*types) * numtypes);
        gen_typeid* tend = titr + numtypes;
        pchoice->types = titr;
        pchoice->numtypes = numtypes;
        while(titr != tend)
        {
            size_t t = gen_add_complex(schema, *tname);
            *titr = t;
            gen_add_dependency(schema, nid, t);
            ++titr;
            ++tname;
        }
        id = nid;
    }
    assert(schema->types[id].header.typeclass == gen_TYPECLASS_CHOICE);
    assert(schema->types[id].choice.numtypes == numtypes);
    return id;
}

//****************************************************************************
gen_typeid gen_add_complex(
    gen_schema* schema,
    const char* name)
{
    int id = gen_find_type(schema, name);
    if(id < 0)
    {
        gen_typeid nid = gen_add_type(schema, gen_TYPECLASS_COMPLEX, name);
        gen_complex *pcomplex = &schema->types[nid].complex;
        char* uprstr = gen_dupstr(name);
        pcomplex->data = gen_DATA_NONE;
        pcomplex->enumname = uprstr;
        while(*uprstr != '\0')
        {
            *uprstr = (char) toupper(*uprstr);
            ++uprstr;
        }
        id = nid;
    }
    assert(schema->types[id].header.typeclass == gen_TYPECLASS_COMPLEX);
    return id;
}

//****************************************************************************
size_t gen_add_list(
    gen_schema* schema,
    const char* name,
    gen_typeid itemtype)
{
    int id = gen_find_type(schema, name);
    if(id < 0)
    {
        gen_typeid nid = gen_add_type(schema, gen_TYPECLASS_LIST, name);
        gen_list* plist = &schema->types[nid].list;
        plist->itemtype = itemtype;
        id = nid;
    }
    assert(schema->types[id].header.typeclass == gen_TYPECLASS_LIST);
    assert(schema->types[id].list.itemtype == itemtype);
    return id;
}

//****************************************************************************
int gen_find_list(
    gen_schema* schema,
    gen_typeid itemtype)
{
    int n = -1;
    gen_type* itr = schema->types;
    gen_type* end = itr + schema->numtypes;
    while(itr != end)
    {
        if(itr->header.typeclass == gen_TYPECLASS_LIST)
        {
            if(itr->list.itemtype == itemtype)
            {
                n = (int) (itr - schema->types);
                break;
            }
        }
        ++itr;
    }
    return n;
}

//****************************************************************************
int gen_find_type(
    gen_schema* schema,
    const char* name)
{
    int n = -1;
    gen_type* itr = schema->types;
    gen_type* end = itr + schema->numtypes;
    while(itr != end)
    {
        if(!strcmp(itr->header.name, name))
        {
            n = (int) (itr - schema->types);
            break;
        }
        ++itr;
    }
    return n;
}

//****************************************************************************
size_t gen_add_attr(
    gen_schema* schema,
    gen_typeid complex,
    const char* xml,
    const char* name,
    gen_datatype datatype)
{
    gen_complex* pcomplex = &schema->types[complex].complex;
    gen_attr* pattr = pcomplex->attributes;
    size_t n = pcomplex->numattributes;
    size_t cap = (n + 15) & (~15);
    size_t ncap = (n + 16) & (~15);
    name = (name != NULL) ? name : xml;
    assert(pcomplex->header.typeclass == gen_TYPECLASS_COMPLEX);
    if(ncap > cap)
    {
        pattr = (gen_attr*) realloc(pattr, sizeof(*pattr) * ncap);
        pcomplex->attributes = pattr;
    }
    pattr += n;
    memset(pattr, 0, sizeof(*pattr));
    pattr->xml = gen_dupstr(xml);
    pattr->name = (name != xml) ? gen_dupstr(name) : pattr->xml;
    pattr->datatype = datatype;
    ++pcomplex->numattributes;
    return n;
}

//****************************************************************************
int gen_add_element(
    gen_schema* schema,
    gen_typeid complex,
    const char* xml,
    const char* name,
    const char* type,
    int minoccurs,
    int maxoccurs)
{
    int elemtype = -1;
    int n = -1;
    name = (name != NULL) ? name : xml;
    type = (type != NULL) ? type : xml;
    elemtype = gen_find_type(schema, type);
    if(elemtype < 0)
    {
        elemtype = gen_add_complex(schema, type);
    }
    if(schema->types[complex].header.typeclass == gen_TYPECLASS_COMPLEX)
    {
        gen_complex* pcomplex = &schema->types[complex].complex;
        gen_element* pelem = pcomplex->elements;
        size_t cap;
        size_t ncap;
        n = pcomplex->numelements;
        cap = (n + 15) & (~15);
        ncap = (n + 16) & (~15);
        if(ncap > cap)
        {
            pelem = (gen_element*) realloc(pelem, sizeof(*pelem)*ncap);
            pcomplex->elements = pelem;
        }
        pelem += n;
        memset(pelem, 0, sizeof(*pelem));
        ++pcomplex->numelements;
        pelem->xml = gen_dupstr(xml);
        pelem->name = (xml != name) ? gen_dupstr(name) : pelem->xml;
        if(maxoccurs < 0 || (maxoccurs > 1 && minoccurs != maxoccurs))
        {
            // list
            int list = gen_find_list(schema, elemtype);
            if(list < 0)
            {
                size_t len = strlen(type);
                char* listname = (char*) malloc(len+5+1);
                strcpy(listname, type);
                strcpy(listname + len, "_list");
                list = gen_add_list(schema, listname, elemtype);
                free(listname);
                pcomplex = NULL; // address may have changed by adding list
            }
            gen_add_dependency(schema, complex, list);
            pelem->type = list;
            pelem->minoccurs = 1;
            pelem->maxoccurs = 1; 
        }
        else
        {
            if(minoccurs != 0)
            {
                gen_add_dependency(schema, complex, elemtype);
            }
            pelem->type = elemtype;
            pelem->minoccurs = minoccurs;
            pelem->maxoccurs = maxoccurs;
        }
    }
    return n;
}

//****************************************************************************
void gen_set_data(
    gen_schema* schema,
    gen_typeid complex,
    gen_datatype datatype,
    int minoccurs,
    int maxoccurs)
{
    gen_complex* pcomplex = &schema->types[complex].complex;
    assert(pcomplex->header.typeclass == gen_TYPECLASS_COMPLEX);
    if(pcomplex->header.typeclass == gen_TYPECLASS_COMPLEX)
    {
        pcomplex->data = datatype;
        pcomplex->mindata = minoccurs;
        pcomplex->maxdata = maxoccurs;
    }
}
