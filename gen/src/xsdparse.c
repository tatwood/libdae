#include "xsdparse.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//****************************************************************************
static void xsdparse_pop(
    xsdparse_state* st)
{
    assert(st->stacksize > 0);
    --st->stacksize;
}

//****************************************************************************
static void xsdparse_push(
    xsdparse_state* st,
    const xsdparse_node* nd)
{
    enum { CHUNK = 16 };
    size_t n = st->stacksize;
    size_t cap = (n + CHUNK - 1) & ~(CHUNK-1);
    size_t ncap = (n + CHUNK) & ~(CHUNK-1);
    if(ncap > cap)
    {
        st->stack = (xsdparse_node*) realloc(st->stack, ncap*sizeof(*nd));
    }
    st->stack[n] = *nd;
    ++st->stacksize;
}

//****************************************************************************
static int xsdparse_attribute(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    const char* name = NULL;
    const char* ref = NULL;
    const char* type = NULL;
    const char* deflt = NULL;
    int required = xsd_UNSET;
    while(*att != NULL)
    {
        if(!strcmp(att[0], "default"))
        {
            deflt = att[1];
        }
        else if(!strcmp(att[0], "name"))
        {
            name = att[1];
        }
        else if(!strcmp(att[0], "ref"))
        {
            ref = att[1];
        }
        else if(!strcmp(att[0], "type"))
        {
            type = att[1];
        }
        else if(!strcmp(att[0], "use"))
        {
            if(!strcmp(att[1], "required"))
            {
                required = 1;
            }
        }
        att += 2;
    }
    if((name != NULL && type != NULL) || (ref != NULL))
    {
        if(nd->type != NULL)
        {
            xsd_add_attrib_to_type(schema,nd->type,name,type,ref,deflt,required);
        }
        else
        {
            xsd_add_attrib_global(schema,name,type,ref,deflt,required);
        }
        success = 1;
    }
    return success;
}

//****************************************************************************
static int xsdparse_choice(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int min = xsd_UNSET;
    int max = xsd_UNSET;
    while(*att != NULL)
    {
        if(!strcmp(att[0], "maxOccurs"))
        {
            const char* val = att[1];
            if(!strcmp(val, "unbounded"))
            {
                max = xsd_UNBOUNDED;
            }
            else
            {
                sscanf(val, "%i", &max);
            }
        }
        if(!strcmp(att[0], "minOccurs"))
        {
            const char* val = att[1];
            sscanf(val, "%i", &min);
        }
        att += 2;
    }
    nd->inseq = 0;
    nd->min = min;
    nd->max = max;
    return 1;
}

//****************************************************************************
static int xsdparse_complextype(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    const char* name = NULL;
    while(*att != NULL)
    {
        if(!strcmp(att[0], "name"))
        {
            name = att[1];
        }
        att += 2;
    }
    if(name != NULL)
    {
        xsd_type* t = xsd_add_type(schema, name);
        t->hascomplex = 1;
        memset(nd, 0, sizeof(*nd));
        nd->type = t;
        success = 1;
    }
    else if(nd->element != NULL && nd->element->type == NULL)
    {
        // inside element with anonymous type
        char* containername = NULL;
        xsd_element* el = nd->element;
        xsd_type* t;
        if(nd->group != NULL && nd->group->name != NULL)
        {
            containername = nd->group->name;
        }
        else if(nd->type != NULL && nd->type->name != NULL)
        {
            containername = nd->type->name;
        }
        if(containername != NULL)
        {
            // inside a group or type definition
            // derive new type name from container name and element name
            size_t ndsz = strlen(containername);
            size_t elsz = strlen(el->name);
            char* name = (char*) malloc(ndsz + 1 + elsz + 1);
            strcpy(name, containername);
            name[ndsz] = '_';
            strcpy(name + ndsz + 1, el->name);
            t = xsd_add_type(schema, name);
            free(name);
        }
        else
        {
            // top level element
            // derive type name from element name
            t = xsd_add_type(schema, el->name);
        }
        t->isanonymous = 1;
        t->hascomplex = 1;
        el->type = xsd_strdup(t->name);
        memset(nd, 0, sizeof(*nd));
        nd->type = t;
        success = 1;
    }
    return success;
}

//****************************************************************************
static int xsdparse_element(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    const char* name = NULL;
    const char* type = NULL;
    const char* ref = NULL;
    int max = parent->max;
    int min = parent->min;
    while(*att != NULL)
    {
        if(!strcmp(att[0], "maxOccurs"))
        {
            const char* val = att[1];
            if(!strcmp(val, "unbounded"))
            {
                max = xsd_UNBOUNDED;
            }
            else
            {
                sscanf(val, "%i", &max);
            }
        }
        if(!strcmp(att[0], "minOccurs"))
        {
            const char* val = att[1];
            sscanf(val, "%i", &min);
        }
        if(!strcmp(att[0], "name"))
        {
            name = att[1];
        }
        else if(!strcmp(att[0], "ref"))
        {
            ref = att[1];
        }
        else if(!strcmp(att[0], "type"))
        {
            type = att[1];
        }
        att += 2;
    }
    if(name != NULL || ref != NULL)
    {
        xsd_element* el;
        if(nd->group != NULL)
        {
            xsd_group* grp = nd->group;
            int seq = nd->seq;
            el=xsd_add_element_to_group(schema,grp,name,type,ref,min,max,seq);
            if(nd->inseq)
            {
                ++nd->seq;
            }
        }
        else if(nd->type != NULL)
        {
            xsd_type* t = nd->type;
            int seq = nd->seq;
            el = xsd_add_element_to_type(schema,t,name,type,ref,min,max,seq);
            if(nd->inseq)
            {
                ++nd->seq;
            }
        }
        else
        {
            if(name != NULL && type != NULL)
            {
                xsd_type* tsrc = xsd_find_type(schema, type);
                xsd_type* t = xsd_add_type(schema, name);
                t->base = xsd_strdup(type);
                t->baserelation = xsd_BASE_RESTRICTION;
                if(tsrc != NULL)
                {
                    t->hascomplex = tsrc->hascomplex;
                    t->hassimple = tsrc->hassimple;
                }
                type = name;
            }
            el = xsd_add_element_global(schema,name,type,ref,min,max);
        }
        nd->element = el;
        success = 1;
    }
    return success;
}

//****************************************************************************
static int xsdparse_extension(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    if(nd->type != NULL)
    {
        const char* base = NULL;
        while(*att != NULL)
        {
            if(!strcmp(att[0], "base"))
            {
                base = att[1];
            }
            att += 2;
        }
        if(base != NULL)
        {
            nd->type->base = xsd_strdup(base);
            nd->type->baserelation = xsd_BASE_EXTEND;
            success = 1;
        }
    }
    return success;
}

//****************************************************************************
static int xsdparse_group(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    const char* name = NULL;
    const char* ref = NULL;
    while(*att != NULL)
    {
        if(!strcmp(att[0], "name"))
        {
            name = att[1];
        }
        if(!strcmp(att[0], "ref"))
        {
            ref = att[1];
        }
        att += 2;
    }
    if(name != NULL || ref != NULL)
    {
        xsd_group* grp;
        if(nd->type != NULL)
        {
            xsd_type* t = nd->type;
            int seq = nd->seq;
            grp = xsd_add_group_to_type(schema, t, name, ref, seq);
            if(nd->inseq)
            {
                ++nd->seq;
            }
        }
        else
        {
            grp = xsd_add_group_global(schema, name, ref);
        }
        nd->group = grp;
        success = 1;
    }
    return success;
}

//****************************************************************************
static int xsdparse_import(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    const char* ns = NULL;
    while(*att != NULL)
    {
        if(!strcmp(att[0], "namespace"))
        {
            ns = att[1];
        }
        att += 2;
    }
    if(ns != NULL)
    {
        if(!strcmp(ns, "http://www.w3.org/1998/Math/MathML"))
        {
            // MathML import, define math element and type with any content
            xsd_type* t;
            t = xsd_add_type(schema, "math:math");
            t->hascomplex = 1;
            t->anycontent = 1;
            xsd_add_element_global(schema,"math:math","math:math",NULL,0,-1);
        }
        success = 1;
    }
    return success;
}

//****************************************************************************
static int xsdparse_list(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    if(nd->type != NULL && nd->type->hassimple)
    {
        xsd_type* t = nd->type;
        const char* itemtype = NULL;
        while(*att != NULL)
        {
            if(!strcmp(att[0], "itemType"))
            {
                itemtype = att[1];
            }
            att += 2;
        }
        if(itemtype != NULL)
        {
            t->simple.itemtype = xsd_strdup(itemtype);
            t->simple.minoccurs = 0;
            t->simple.maxoccurs = xsd_UNBOUNDED;
            success = 1;
        }
    }
    return success;
}

//****************************************************************************
static int xsdparse_restriction(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    if(nd->type != NULL)
    {
        const char* base = NULL;
        while(*att != NULL)
        {
            if(!strcmp(att[0], "base"))
            {
                base = att[1];
            }
            att += 2;
        }
        if(base != NULL)
        {
            nd->type->base = xsd_strdup(base);
            nd->type->baserelation = xsd_BASE_RESTRICTION;
            success = 1;
        }
    }
    return success;
}

//****************************************************************************
static int xsdparse_sequence(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int min = xsd_UNSET;
    int max = xsd_UNSET;
    while(*att != NULL)
    {
        if(!strcmp(att[0], "maxOccurs"))
        {
            const char* val = att[1];
            if(!strcmp(val, "unbounded"))
            {
                max = xsd_UNBOUNDED;
            }
            else
            {
                sscanf(val, "%i", &max);
            }
        }
        if(!strcmp(att[0], "minOccurs"))
        {
            const char* val = att[1];
            sscanf(val, "%i", &min);
        }
        att += 2;
    }
    nd->inseq = 1;
    nd->min = min;
    nd->max = max;
    return 1;
}

//****************************************************************************
static int xsdparse_simpletype(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    const char* name = NULL;
    while(*att != NULL)
    {
        if(!strcmp(att[0], "name"))
        {
            name = att[1];
        }
        att += 2;
    }
    if(name != NULL)
    {
        xsd_type* t = xsd_add_type(schema, name);
        t->hassimple = 1;
        memset(nd, 0, sizeof(*nd));
        nd->type = t;
        success = 1;
    }
    else if(nd->element != NULL && nd->element->type == NULL)
    {
        // inside element with anonymous type
        xsd_element* el = nd->element;
        xsd_type* t;
        if(nd->type != NULL)
        {
            // inside a type definition
            // derive new type with name from parent type and element
            size_t ndsz = strlen(nd->type->name);
            size_t elsz = strlen(el->name);
            char* name = (char*) malloc(ndsz + 1 + elsz + 1);
            strcpy(name, nd->type->name);
            name[ndsz] = '_';
            strcpy(name + ndsz + 1, el->name);
            t = xsd_add_type(schema, name);
            t->isanonymous = 1;
            free(name);
        }
        else
        {
            // top level element
            // derive type name from element name
            t = xsd_add_type(schema, el->name);
        }
        t->hassimple = 1;
        el->type = xsd_strdup(t->name);
        memset(nd, 0, sizeof(*nd));
        nd->type = t;
        success = 1;
    }
    return success;
}

//****************************************************************************
static int xsdparse_union(
    const xsdparse_state* st,
    const xsdparse_node* parent,
    xsd_schema* schema,
    xsdparse_node* nd,
    const char** att)
{
    int success = 0;
    if(nd->type != NULL && nd->type->hassimple)
    {
        xsd_type* t = nd->type;
        const char* membertypes = NULL;
        while(*att != NULL)
        {
            if(!strcmp(att[0], "memberTypes"))
            {
                membertypes = att[1];
            }
            att += 2;
        }
        if(membertypes != NULL && t->simple.itemtype == NULL)
        {
            // for now, make all unions strings that the client can parse
            t->simple.itemtype = xsd_strdup(xsd_DATA_STRING);
            success = 1;
        }
    }
    return success;
}

//****************************************************************************
void xsdparse_startelement(
    void* userdata,
    const char* el,
    const char** att)
{
    xsdparse_state* st = (xsdparse_state*) userdata;
    xsd_schema* schema = st->schema;
    xsdparse_node* parent;
    xsdparse_node nd;
    int success;
    if(st->stacksize > 0)
    {
        parent = st->stack + st->stacksize - 1;
        nd = *parent;
        nd.childseq = 0;
    }
    else
    {
        parent = NULL;
        memset(&nd, 0, sizeof(nd));
    }
    success = 0;
    if(!strcmp(el, "xs:annotation"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:any"))
    {
        if(nd.type != NULL)
        {
            nd.type->anycontent = 1;
            success = 1;
        }
    }
    else if(!strcmp(el, "xs:appinfo"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:attribute"))
    {
        success = xsdparse_attribute(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:choice"))
    {
        success = xsdparse_choice(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:complexContent"))
    {
        if(nd.type != NULL && nd.type->hascomplex)
        {
            success = 1;
        }
    }
    else if(!strcmp(el, "xs:complexType"))
    {
        success = xsdparse_complextype(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:documentation"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:element"))
    {
        success = xsdparse_element(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:enumeration"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:extension"))
    {
        success = xsdparse_extension(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:group"))
    {
        success = xsdparse_group(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:import"))
    {
        success = xsdparse_import(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:list"))
    {
        success = xsdparse_list(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:maxExclusive"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:maxInclusive"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:maxLength"))
    {
        if(nd.type != NULL && !nd.type->hascomplex)
        {
            xsd_type* t = nd.type;
            const char* value = NULL;
            while(*att != NULL)
            {
                if(!strcmp(att[0], "value"))
                {
                    value = att[1];
                }
                att += 2;
            }
            if(value != NULL)
            {
                int i;
                if(sscanf(value, "%i", &i) == 1)
                {
                    t->simple.maxoccurs = i;
                    success = 1;
                }
            }
        }
    }
    else if(!strcmp(el, "xs:minExclusive"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:minInclusive"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:minLength"))
    {
        if(nd.type != NULL && !nd.type->hascomplex)
        {
            xsd_type* t = nd.type;
            const char* value = NULL;
            while(*att != NULL)
            {
                if(!strcmp(att[0], "value"))
                {
                    value = att[1];
                }
                att += 2;
            }
            if(value != NULL)
            {
                int i;
                if(sscanf(value, "%i", &i) == 1)
                {
                    t->simple.minoccurs = i;
                    success = 1;
                }
            }
        }
    }
    else if(!strcmp(el, "xs:pattern"))
    {
        success = 1;
    }
    else if(!strcmp(el, "xs:restriction"))
    {
        success = xsdparse_restriction(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:schema"))
    {
        if(st->stacksize == 0)
        {
            success = 1;
        }
    }
    else if(!strcmp(el, "xs:sequence"))
    {
        success = xsdparse_sequence(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:simpleContent"))
    {
        nd.type->hassimple = 1;
        success = 1;
    }
    else if(!strcmp(el, "xs:simpleType"))
    {
        success = xsdparse_simpletype(st, parent, schema, &nd, att);
    }
    else if(!strcmp(el, "xs:union"))
    {
        success = xsdparse_union(st, parent, schema, &nd, att);
    }
    xsdparse_push(st, &nd);
    if(success == 0)
    {
        assert(0);
    }
}

//****************************************************************************
void xsdparse_endelement(
    void* userdata,
    const char* el)
{
    xsdparse_state* st = (xsdparse_state*) userdata;
    xsd_schema* schema = st->schema;
    xsdparse_node* nd = st->stack + st->stacksize - 1;
    assert(st->stacksize > 0);
    if(!strcmp(el, "xs:element"))
    {
        xsd_element* el = nd->element;
        if(el->name != NULL && el->ref == NULL && el->type == NULL)
        {
            // this is a hack to make sure that if the element has no type
            // definition, an empty one exists for it
            xsd_type* t;
            if(nd->type != NULL)
            {
                // inside a type definition
                // derive new type with name from parent type and element
                size_t ndsz = strlen(nd->type->name);
                size_t elsz = strlen(el->name);
                char* name = (char*) malloc(ndsz + 1 + elsz + 1);
                strcpy(name, nd->type->name);
                name[ndsz] = '_';
                strcpy(name + ndsz + 1, el->name);
                t = xsd_add_type(schema, name);
                free(name);
            }
            else
            {
                // top level element
                // derive type name from element name
                t = xsd_add_type(schema, el->name);
            }
            el->type = xsd_strdup(t->name);
        }
    }
    if(st->stacksize > 1)
    {
        xsdparse_node* parent = nd - 1;
        if ((nd->type!=NULL && parent->type==nd->type) ||
            (nd->group!=NULL && parent->group==nd->group))
        {
            // only propagate child sequences up to parent if they share
            // the same type or group definition
            // update the counter of maximum child sequence to parent
            if(parent->childseq < nd->childseq)
            {
                parent->childseq = nd->childseq;
            }
            if(parent->childseq < nd->seq)
            {
                parent->childseq = nd->seq;
            }
            // if the parent node is part of a sequence, its next node must
            // be ordered after all the its previous children
            if(parent->inseq)
            {
                parent->seq = parent->childseq;
            }
        }
    }
    xsdparse_pop(st);
}

//****************************************************************************
void xsdparse_hacks(
    xsd_schema* schema)
{
    // add backward compatibility for 1.4.1 to common materials
    xsd_type* newparamt = xsd_find_type(schema, "fx_common_newparam_type");
    xsd_type* sampt = xsd_find_type(schema, "fx_sampler2D_type");
    if(newparamt != NULL)
    {
        xsd_type* surft = xsd_add_type(schema, "fx_surface_common_type");
        surft->hascomplex = 1;
        xsd_add_attrib_to_type(schema,surft,"type","xs:token",NULL,NULL,1);
        xsd_add_element_to_type(
            schema,
            surft,
            "init_from",
            "image_type_init_from",
            NULL,
            0,
            -1,
            0);
        xsd_add_element_to_type(
            schema,
            newparamt,
            "surface",
            "fx_surface_common_type",
            NULL,
            0,
            1,
            0);
    }
    if(sampt != NULL)
    {
        if(sampt->base != NULL)
        {
            sampt->baserelation = xsd_BASE_EXTEND;
        }
        xsd_add_element_to_type(
            schema,
            sampt,
            "source",
            "xs:NCName",
            NULL,
            0,
            1,
            0);
    }
}
