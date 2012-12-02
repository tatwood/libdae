#include "xsd.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//****************************************************************************
static xsd_attrib* xsd_add_attrib(
    xsd_schema* schema,
    xsd_attrib*** arrayref,
    size_t* sizeref,
    const char* name,
    const char* type,
    const char* ref,
    const char* deflt,
    int required)
{
    enum { CHUNK = 16 };
    xsd_attrib* at = (xsd_attrib*) calloc(1, sizeof(*at));
    size_t n = *sizeref;
    size_t cap = (n + CHUNK-1) & ~(CHUNK-1);
    size_t ncap = (n + CHUNK) & ~(CHUNK-1);
    if(ncap > cap)
    {
        *arrayref = (xsd_attrib**) realloc(*arrayref, ncap * sizeof(at));
    }
    (*arrayref)[n] = at;
    *sizeref = (*sizeref) + 1;
    at->name = xsd_strdup(name);
    at->type = xsd_strdup(type);
    at->ref = xsd_strdup(ref);
    at->deflt = xsd_strdup(deflt);
    at->required = required;
    return at;
}

//****************************************************************************
static xsd_element* xsd_add_element(
    xsd_schema* schema,
    xsd_element*** arrayref,
    size_t* sizeref,
    const char* name,
    const char* type,
    const char* ref,
    int minoccurs,
    int maxoccurs,
    int seq)
{
    enum { CHUNK = 16 };
    xsd_element** elements = *arrayref;
    xsd_element* el = (xsd_element*) calloc(1, sizeof(*el));
    size_t n = *sizeref;
    size_t cap = (n + CHUNK-1) & ~(CHUNK-1);
    size_t ncap = (n + CHUNK) & ~(CHUNK-1);
    if(ncap > cap)
    {
        elements = (xsd_element**) realloc(elements, ncap * sizeof(el));
        *arrayref = elements;
    }
    el->name = xsd_strdup(name);
    el->type = xsd_strdup(type);
    el->ref = xsd_strdup(ref);
    el->minoccurs = minoccurs;
    el->maxoccurs = maxoccurs;
    el->seq = seq;
    // sort element into correct sequence order
    while(n > 0 && elements[n-1]->seq > seq)
    {
        elements[n] = elements[n-1];
        --n;
    }
    elements[n] = el;
    *sizeref = (*sizeref) + 1;
    return el;
}

//****************************************************************************
static xsd_group* xsd_add_group(
    xsd_schema* schema,
    xsd_group*** arrayref,
    size_t* sizeref,
    const char* name,
    const char* ref,
    int seq)
{
    enum { CHUNK = 16 };
    xsd_group** groups = *arrayref;
    xsd_group* grp = (xsd_group*) calloc(1, sizeof(*grp));
    size_t n = *sizeref;
    size_t cap = (n + CHUNK-1) & ~(CHUNK-1);
    size_t ncap = (n + CHUNK) & ~(CHUNK-1);
    if(ncap > cap)
    {
        groups = (xsd_group**) realloc(groups, ncap * sizeof(grp));
        *arrayref = groups;
    }
    grp->name = xsd_strdup(name);
    grp->ref = xsd_strdup(ref);
    grp->seq = seq;
    // sort element into correct sequence order
    while(n > 0 && groups[n-1]->seq > seq)
    {
        groups[n] = groups[n-1];
        --n;
    }
    groups[n] = grp;
    *sizeref = (*sizeref) + 1;
    return grp;
}

//****************************************************************************
static void xsd_destroy_attribs(
    xsd_attrib** attribs,
    size_t numattribs)
{
    xsd_attrib** atitr = attribs;
    xsd_attrib** atend = atitr + numattribs;
    while(atitr != atend)
    {
        xsd_attrib* at = *atitr;
        free(at->name);
        free(at->type);
        free(at->ref);
        free(at->deflt);
        free(at);
        ++atitr;
    }
    free(attribs);
}

//****************************************************************************
static void xsd_destroy_elements(
    xsd_element** elements,
    size_t numelements)
{
    xsd_element** elitr = elements;
    xsd_element** elend = elitr + numelements;
    while(elitr != elend)
    {
        xsd_element* el = *elitr;
        free(el->name);
        free(el->type);
        free(el->ref);
        free(el);
        ++elitr;
    }
    free(elements);
}

//****************************************************************************
static void xsd_destroy_groups(
    xsd_group** groups,
    size_t numgroups)
{
    xsd_group** grpitr = groups;
    xsd_group** grpend = grpitr + numgroups;
    while(grpitr != grpend)
    {
        xsd_group* grp = *grpitr;
        free(grp->name);
        free(grp->ref);
        xsd_destroy_elements(grp->elements, grp->numelements);
        free(grp);
        ++grpitr;
    }
    free(groups);
}

//****************************************************************************
static void xsd_destroy_type(
    xsd_type* t)
{
    if(t->hassimple)
    {
        free(t->simple.itemtype);
    }
    if(t->hascomplex)
    {
        xsd_destroy_attribs(t->complex.attribs, t->complex.numattribs);
        xsd_destroy_elements(t->complex.elements, t->complex.numelements);
        xsd_destroy_groups(t->complex.groups, t->complex.numgroups);
    }
    free(t->name);
    free(t->base);
    free(t);
}

//****************************************************************************
static void xsd_flatten_attribs(
    xsd_schema* schema)
{
    // expandes all attrib references within type defintions into attrib defs
    xsd_type** titr = schema->types;
    xsd_type** tend = titr + schema->numtypes;
    while(titr != tend)
    {
        xsd_type* t = *titr;
        if(t->hascomplex)
        {
            xsd_attrib** atitr = t->complex.attribs;
            xsd_attrib** atend = atitr + t->complex.numattribs;
            while(atitr != atend)
            {
                xsd_attrib* at = *atitr;
                xsd_attrib* atref = NULL;
                char* ref = at->ref;
                while(ref != NULL)
                {
                    atref = xsd_find_attrib(schema, ref);
                    assert(atref != NULL);
                    ref = atref->ref;
                }
                if(atref != NULL)
                {
                    at->name = xsd_strdup(atref->name);
                    at->type = xsd_strdup(atref->type);
                    at->deflt = xsd_strdup(atref->deflt);
                    free(at->ref);
                    at->ref = NULL;
                }
                ++atitr;
            }
        }
        ++titr;
    }
}

//****************************************************************************
static void xsd_flatten_elements(
    xsd_schema* schema)
{
    // replaces all element refs within groups and types with element defs
    xsd_element** elitr;
    xsd_element** elend;
    xsd_group** grpitr;
    xsd_group** grpend;
    xsd_type** titr;
    xsd_type** tend;
    grpitr = schema->groups;
    grpend = grpitr + schema->numgroups;
    while(grpitr != grpend)
    {
        xsd_group* grp = *grpitr;
        elitr = grp->elements;
        elend = elitr + grp->numelements;
        while(elitr != elend)
        {
            xsd_element* el = *elitr;
            xsd_element* elref = NULL;
            char* ref = el->ref;
            while(ref != NULL)
            {
                elref = xsd_find_element(schema, ref);
                assert(elref != NULL);
                ref = elref->ref;
            }
            if(elref != NULL)
            {
                el->name = xsd_strdup(elref->name);
                el->type = xsd_strdup(elref->type);
                free(el->ref);
                el->ref = NULL;
            }
            ++elitr;
        }
        ++grpitr;
    }
    titr = schema->types;
    tend = titr + schema->numtypes;
    while(titr != tend)
    {
        xsd_type* t = *titr;
        if(t->hascomplex)
        {
            elitr = t->complex.elements;
            elend = elitr + t->complex.numelements;
            while(elitr != elend)
            {
                xsd_element* el = *elitr;
                xsd_element* elref = NULL;
                char* ref = el->ref;
                while(ref != NULL)
                {
                    elref = xsd_find_element(schema, ref);
                    assert(elref != NULL);
                    ref = elref->ref;
                }
                if(elref != NULL)
                {
                    el->name = xsd_strdup(elref->name);
                    el->type = xsd_strdup(elref->type);
                    free(el->ref);
                    el->ref = NULL;
                }
                ++elitr;
            }
            grpitr = t->complex.groups;
            grpend = grpitr + t->complex.numgroups;
            while(grpitr != grpend)
            {
                xsd_group* grp = *grpitr;
                elitr = grp->elements;
                elend = elitr + grp->numelements;
                while(elitr != elend)
                {
                    xsd_element* el = *elitr;
                    xsd_element* elref = NULL;
                    char* ref = el->ref;
                    while(ref != NULL)
                    {
                        elref = xsd_find_element(schema, ref);
                        assert(elref != NULL);
                        ref = elref->ref;
                    }
                    if(elref != NULL)
                    {
                        el->name = xsd_strdup(elref->name);
                        el->type = xsd_strdup(elref->type);
                        free(el->ref);
                        el->ref = NULL;
                    }
                    ++elitr;
                }
                ++grpitr;
            }
        }
        ++titr;
    }
}

//****************************************************************************
static void xsd_flatten_groups(
    xsd_schema* schema)
{
    // expandes all groups within type defintions into elements
    xsd_type** titr = schema->types;
    xsd_type** tend = titr + schema->numtypes;
    while(titr != tend)
    {
        xsd_type* t = *titr;
        xsd_group** grpitr;
        xsd_group** grpend;
        xsd_element** elitr;
        xsd_element** elend;
        // offset the sequence numbers of all the groups
        grpitr = t->complex.groups;
        grpend = grpitr + t->complex.numgroups;
        while(grpitr != grpend)
        {
            xsd_group* grp = *grpitr;
            xsd_group** inrgrpitr = t->complex.groups;
            xsd_group** inrgrpend = inrgrpitr + t->complex.numgroups;
            while(inrgrpitr != inrgrpend)
            {
                xsd_group* inrgrp = *inrgrpitr;
                if(grp->seq > inrgrp->seq)
                {
                    // if the group instance is a reference, the sequence
                    // number of the instance comes from the reference
                    if(inrgrp->ref != NULL)
                    {
                        // the number of sequenced elements in the group 
                        // comes from the reference target
                        inrgrp = xsd_find_group(schema, inrgrp->ref);
                        assert(inrgrp != NULL);
                    }
                    grp->seq += inrgrp->seqmax;
                }
                ++inrgrpitr;
            }
            ++grpitr;
        }
        // offset the sequence numbers of all the existing elements
        elitr = t->complex.elements;
        elend = elitr + t->complex.numelements;
        while(elitr != elend)
        {
            xsd_element* el = *elitr;
            xsd_group** inrgrpitr = t->complex.groups;
            xsd_group** inrgrpend = inrgrpitr + t->complex.numgroups;
            while(inrgrpitr != inrgrpend)
            {
                xsd_group* inrgrp = *inrgrpitr;
                if(el->seq > inrgrp->seq)
                {
                    // if the group instance is a reference, the sequence
                    // number of the instance comes from the reference
                    if(inrgrp->ref != NULL)
                    {
                        // the number of sequenced elements in the group 
                        // comes from the reference target
                        inrgrp = xsd_find_group(schema, inrgrp->ref);
                        assert(inrgrp != NULL);
                    }
                    el->seq += inrgrp->seqmax;
                }
                ++inrgrpitr;
            }
            ++elitr;
        }
        grpitr = t->complex.groups;
        grpend = grpitr + t->complex.numgroups;
        while(grpitr != grpend)
        {
            xsd_group* grp = *grpitr;
            int seqoffset = grp->seq; // save before resolving reference
            if(grp->ref != NULL)
            {
                grp = xsd_find_group(schema, grp->ref);
                assert(grp != NULL);
            }
            elitr = grp->elements;
            elend = elitr + grp->numelements;
            while(elitr != elend)
            {
                xsd_element* el = *elitr;
                xsd_add_element_to_type(
                    schema,
                    t,
                    el->name,
                    el->type,
                    el->ref,
                    el->minoccurs,
                    el->maxoccurs,
                    seqoffset + el->seq);
                ++elitr;
            }
            ++grpitr;
        }
        xsd_destroy_groups(t->complex.groups, t->complex.numgroups);
        t->complex.groups = NULL;
        t->complex.numgroups = 0;
        ++titr;
    }
    xsd_destroy_groups(schema->groups, schema->numgroups);
    schema->groups = NULL;
    schema->numgroups = 0;
}

//****************************************************************************
static void xsd_flatten_relation(
    xsd_schema* schema,
    xsd_type* t,
    xsd_type* tbase)
{
    // relation type should never be set to rename until after type is
    // flattened
    assert(t->baserelation != xsd_BASE_RENAME);
    if(tbase->hassimple)
    {
        // copy simple content from base type
        t->simple.itemtype=xsd_strdup(tbase->simple.itemtype);
        if(t->simple.minoccurs == xsd_UNSET)
        {
            t->simple.minoccurs = tbase->simple.minoccurs;
        }
        if(t->simple.maxoccurs == xsd_UNSET)
        {
            t->simple.maxoccurs = tbase->simple.maxoccurs;
        }
        t->hassimple = 1;
    }
    if(tbase->hascomplex && tbase->complex.numattribs > 0)
    {
        // copy attributes from base type
        xsd_attrib** atitr = tbase->complex.attribs;
        xsd_attrib** atend = atitr+tbase->complex.numattribs;
        while(atitr != atend)
        {
            xsd_attrib* at = *atitr;
            xsd_add_attrib_to_type(
                schema,
                t,
                at->name,
                at->type,
                at->ref,
                at->deflt,
                at->required);
            ++atitr;
        }
        t->hascomplex = 1;
    }
    if(tbase->hascomplex && t->baserelation == xsd_BASE_EXTEND)
    {
        // only copy groups and elements if relationship is extends
        xsd_group** grpitr;
        xsd_group** grpend;
        xsd_element** elitr;
        xsd_element** elend;
        // offset sequence number of all child groups
        grpitr = t->complex.groups;
        grpend = grpitr + t->complex.numgroups;
        while(grpitr != grpend)
        {
            xsd_group* grp = *grpitr;
            grp->seq += tbase->complex.seqmax;
            ++grpitr;
        }
        // offset sequence number of all child elements
        elitr = t->complex.elements;
        elend = elitr + t->complex.numelements;
        while(elitr != elend)
        {
            xsd_element* el = *elitr;
            el->seq += tbase->complex.seqmax;
            ++elitr;
        }
        // add base type groups
        grpitr = tbase->complex.groups;
        grpend = grpitr + tbase->complex.numgroups;
        while(grpitr != grpend)
        {
            xsd_group* grp = *grpitr;
            xsd_group* cgrp = xsd_add_group_to_type(
                schema,
                t,
                grp->name,
                grp->ref,
                grp->seq);
            // copy base group elements to child group
            elitr = grp->elements;
            elend = elitr + grp->numelements;
            while(elitr != elend)
            {
                xsd_element* el = *elitr;
                xsd_add_element_to_group(
                    schema,
                    cgrp,
                    el->name,
                    el->type,
                    el->ref,
                    el->minoccurs,
                    el->maxoccurs,
                    el->seq);
                ++elitr;
            }
            ++grpitr;
        }
        // add base type elements
        elitr = tbase->complex.elements;
        elend = elitr + tbase->complex.numelements;
        while(elitr != elend)
        {
            xsd_element* el = *elitr;
            xsd_add_element_to_type(
                schema,
                t,
                el->name,
                el->type,
                el->ref,
                el->minoccurs,
                el->maxoccurs,
                el->seq);
            ++elitr;
        }
        t->hascomplex = 1;
        t->anycontent = tbase->anycontent;
    }
}

//****************************************************************************
static int xsd_is_rename(
    xsd_schema* schema,
    xsd_type* t)
{
     xsd_type* tbase = NULL;
     int rename = 0;
     if(t->baserelation != xsd_BASE_NONE)
     {
         rename = 1;
         tbase = xsd_find_type(schema, t->base);
         assert(tbase != NULL);
         if(t->hassimple)
         {
             if(t->simple.itemtype != NULL)
             {
                 if(tbase->simple.itemtype != NULL)
                 {
                     if(strcmp(t->simple.itemtype, tbase->simple.itemtype))
                     {
                         rename = 0;
                     }
                 }
                 else
                 {
                     rename = 0;
                 }
             }
             if(t->simple.minoccurs != xsd_UNSET)
             {
                 if(t->simple.minoccurs != tbase->simple.minoccurs)
                 {
                     rename = 0;
                 }
             }
             if(t->simple.maxoccurs != xsd_UNSET)
             {
                 if(t->simple.maxoccurs != tbase->simple.maxoccurs)
                 {
                     rename = 0;
                 }
             }
         }
         if(t->hascomplex)
         {
             if (t->complex.numattribs != 0)
             {
                 rename = 0;
             }
             if(t->complex.numgroups != 0)
             {
                 rename = 0;
             }
             if(t->complex.numelements != 0)
             {
                 rename = 0;
             }
             if(t->anycontent && !tbase->anycontent)
             {
                 rename = 0;
             }
         }
     }
     else if(t->hassimple && !t->hascomplex)
     {
         // no base type, but it's simple
         // flatten it if it does not deviate from primitive types
         assert(t->simple.itemtype != NULL);
         if(t->simple.minoccurs==xsd_UNSET || t->simple.minoccurs==1)
         {
             if(t->simple.maxoccurs==xsd_UNSET || t->simple.maxoccurs==1)
             {
                 if(strcmp(t->name, t->simple.itemtype))
                 {
                     // replace it if it is not the primitive definition
                     tbase = xsd_find_type(schema, t->simple.itemtype);
                     rename = 1;
                 }
             }
         }
     }
     return rename;
}

//****************************************************************************
static void xsd_replace(
    xsd_schema* schema,
    const char* src,
    const char* target)
{
    xsd_type** titr = schema->types;
    xsd_type** tend = titr + schema->numtypes;
    xsd_attrib** atitr;
    xsd_attrib** atend;
    xsd_group** grpitr;
    xsd_group** grpend;
    xsd_element** elitr;
    xsd_element** elend;
    // replace matching top level attribs with target type
    atitr = schema->attribs;
    atend = atitr + schema->numattribs;
    while(atitr != atend)
    {
        xsd_attrib* at = *atitr;
        if(at->type != NULL && !strcmp(at->type,src))
        {
            free(at->type);
            at->type = xsd_strdup(target);
        }
        ++atitr;
    }
    // replace matching top level elements with target type
    elitr = schema->elements;
    elend = elitr + schema->numelements;
    while(elitr != elend)
    {
        xsd_element* el = *elitr;
        if(el->type != NULL && !strcmp(el->type,src))
        {
            free(el->type);
            el->type = xsd_strdup(target);
        }
        ++elitr;
    }
    // replace matching group elements with target type
    grpitr = schema->groups;
    grpend = grpitr + schema->numgroups;
    while(grpitr != grpend)
    {
        xsd_group* grp = *grpitr;
        elitr = grp->elements;
        elend = elitr + grp->numelements;
        while(elitr != elend)
        {
            xsd_element* el = *elitr;
            if(el->type != NULL && !strcmp(el->type,src))
            {
                free(el->type);
                el->type = xsd_strdup(target);
            }
            ++elitr;
        }
        ++grpitr;
    }
    while(titr != tend)
    {
        xsd_type* t = *titr;
        if(t->base != NULL)
        {
            if(!strcmp(t->base,src))
            {
                free(t->base);
                t->base = xsd_strdup(target);
            }
        }
        if(t->hassimple)
        {
            char* itemtype = t->simple.itemtype;
            if(itemtype != NULL && !strcmp(itemtype, src))
            {
                free(t->simple.itemtype);
                t->simple.itemtype = xsd_strdup(target);
            }
        }
        if(t->hascomplex)
        {
            // replace matching attribs with target type
            atitr = t->complex.attribs;
            atend = atitr + t->complex.numattribs;
            while(atitr != atend)
            {
                xsd_attrib* at = *atitr;
                if(at->type != NULL && !strcmp(at->type,src))
                {
                    free(at->type);
                    at->type = xsd_strdup(target);
                }
                ++atitr;
            }
            // replace matching elements with target type
            elitr = t->complex.elements;
            elend = elitr + t->complex.numelements;
            while(elitr != elend)
            {
                xsd_element* el = *elitr;
                if(el->type != NULL && !strcmp(el->type,src))
                {
                    free(el->type);
                    el->type = xsd_strdup(target);
                }
                ++elitr;
            }
            // replace matching group elements with target type
            grpitr = t->complex.groups;
            grpend = grpitr + t->complex.numgroups;
            while(grpitr != grpend)
            {
                xsd_group* grp = *grpitr;
                elitr = grp->elements;
                elend = elitr + grp->numelements;
                while(elitr != elend)
                {
                    xsd_element* el = *elitr;
                    if(el->type != NULL && !strcmp(el->type,src))
                    {
                        free(el->type);
                        el->type = xsd_strdup(target);
                    }
                    ++elitr;
                }
                ++grpitr;
            }
        }
        ++titr;
    }
}

//****************************************************************************
static void xsd_flatten_types(
    xsd_schema* schema)
{
    // expands child types to include all inherited properties from base type
    int changed; // controls outer loop
    int done; // redundant correctness check
    do
    {
        // this is an iterative process that must be repeated until
        // no changes are made
        xsd_type** titr = schema->types;
        xsd_type** tend = titr + schema->numtypes;
        changed = 0;
        done = 1;
        while(titr != tend)
        {
            xsd_type* t = *titr;
            xsd_base_relation rel = t->baserelation;
            int isrename = xsd_is_rename(schema, t);
            if(rel == xsd_BASE_EXTEND || rel == xsd_BASE_RESTRICTION)
            {
                done = 0;
            }
            if (rel == xsd_BASE_EXTEND ||
                rel == xsd_BASE_RESTRICTION ||
                (rel != xsd_BASE_RENAME && isrename))
            {
                // only process type if its base has already been flattened
                xsd_type* tbase;
                xsd_base_relation baserel;
                if(t->baserelation != xsd_BASE_NONE)
                {
                    tbase = xsd_find_type(schema, t->base);
                }
                else
                {
                    // it's possible that the type did not specify a base,
                    // but was a non-array simple type that did not specify
                    // any attributes. in that case, it's treated as a rename
                    tbase = xsd_find_type(schema, t->simple.itemtype);
                }
                baserel = tbase->baserelation;
                assert(tbase != NULL);
                if (baserel != xsd_BASE_EXTEND &&
                    baserel != xsd_BASE_RESTRICTION &&
                    (baserel==xsd_BASE_RENAME||!xsd_is_rename(schema,tbase)))
                {
                    // copy all members of the base down to the child type
                    xsd_flatten_relation(schema, t, tbase);
                    if(isrename)
                    {
                        // type has no unique content that differs from the
                        // base, so treat it as a rename
                        if(!t->isanonymous)
                        {
                            // if the type was explicity defined, keep it, but
                            // flatten the relationship and mark as a rename
                            t->baserelation = xsd_BASE_RENAME;
                            assert(tbase != NULL);
                            if(t->base == NULL)
                            {
                                t->base = xsd_strdup(tbase->name);
                            }
                        }
                        else
                        {
                            // if it was an implicitly defined type, replace
                            // all references to it with the base type and
                            // remove it completely
                            xsd_replace(schema, t->name, tbase->name);
                            xsd_remove_type(schema,(int)(titr-schema->types));
                        }
                    }
                    else
                    {
                        // remove relationship
                        free(t->base);
                        t->base = NULL;
                        t->baserelation = xsd_BASE_NONE;
                    }
                    // indicate change occurred
                    changed = 1;
                    // need to break the loop because iterator may be invalid
                    break;
                }
            }
            ++titr;
        }
    }
    while(changed);
    // there should not be any extend or restriction inheritence remaining
    if(done == 0)
    {
        assert(0);
    }
}

//****************************************************************************
static void xsd_predefine_types(
    xsd_schema* schema)
{
    xsd_type* t;
    t = xsd_add_type(schema, xsd_DATA_BOOL);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_BOOL);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_HEX);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_HEX);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_FLOAT);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_FLOAT);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_INT8);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_INT8);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_INT16);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_INT16);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_INT32);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_INT32);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_STRING);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_STRING);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_UINT8);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_UINT8);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_UINT16);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_UINT16);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    t = xsd_add_type(schema, xsd_DATA_UINT32);
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup(xsd_DATA_UINT32);
    t->simple.minoccurs = 1;
    t->simple.maxoccurs = 1;
    // add redefines of base types
    t = xsd_add_type(schema,  "xs:anyURI");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_STRING);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:dateTime");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_STRING);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:double");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_FLOAT);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:ID");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_STRING);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:IDREF");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_STRING);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:integer");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_INT32);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:long");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_INT32);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:Name");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_STRING);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:NCName");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_STRING);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:NMTOKEN");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_STRING);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:nonNegativeInteger");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_UINT32);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:positiveInteger");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_UINT32);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:token");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_STRING);
    t->baserelation = xsd_BASE_RESTRICTION;
    t = xsd_add_type(schema, "xs:unsignedLong");
    t->isbuiltin = 1;
    t->base = xsd_strdup(xsd_DATA_UINT32);
    t->baserelation = xsd_BASE_RESTRICTION;
    // additional misc types
    t = xsd_add_type(schema, "xs:IDREFS");
    t->isbuiltin = 1;
    t->hassimple = 1;
    t->simple.itemtype = xsd_strdup("xs:ID");
    t->simple.minoccurs = 0;
    t->simple.maxoccurs = -1;
    // top level attribs
    xsd_add_attrib_global(schema, "xml:base", "xs:anyURI", NULL, NULL, 0);
    // top level elements
}

//****************************************************************************
static int xsd_validate_attrib(
    xsd_schema* schema,
    xsd_attrib** list,
    size_t listsize,
    xsd_attrib* at)
{
    int result = 0;
    if(at->name != NULL)
    {
        // make sure all attributes in the list have a unique name
        xsd_attrib** atitr = list;
        xsd_attrib** atend = atitr + listsize;
        while(atitr != atend)
        {
            xsd_attrib* atother = *atitr;
            if(atother != at)
            {
                if(atother->name != NULL && !strcmp(atother->name, at->name))
                {
                    // name must be unique
                    assert(0);
                    result = -1;
                    break;
                }
            }
            ++atitr;
        }
    }
    if(at->type != NULL)
    {
        xsd_type* attype = xsd_find_type(schema, at->type);
        if(at->name == NULL)
        {
            // must have a name
            assert(0);
            result = -1;
        }
        if(attype != NULL)
        {
            if(attype->hascomplex)
            {
                // make sure the attribute type is not complex
                assert(0);
                result = -1;
            }
        }
        else
        {
            // make sure the attribute type actually exists
            assert(0);
            result = -1;
        }
    }
    else if(at->ref != NULL)
    {
        xsd_attrib* atref = xsd_find_attrib(schema, at->ref);
        if(atref == NULL)
        {
            // make sure the referenced attribute exists
            assert(0);
            result = -1;
        }
    }
    else
    {
        // make sure the attribute has a type or ref
        assert(0);
        result = -1;
    }
    return result;
}

//****************************************************************************
static int xsd_validate_element(
    xsd_schema* schema,
    xsd_element** list,
    size_t listsize,
    xsd_element* el)
{
    int result = 0;
    if(el->name != NULL)
    {
        // make sure all elements in the list have a unique name
        xsd_element** elitr = list;
        xsd_element** elend = elitr + listsize;
        while(elitr != elend)
        {
            xsd_element* elother = *elitr;
            if(elother != el)
            {
                if(elother->name != NULL && !strcmp(elother->name, el->name))
                {
                    if(el->type == NULL || elother->type == NULL)
                    {
                        // must have types
                        assert(0);
                        result = -1;
                        break;
                    }
                    if(strcmp(el->type, elother->type))
                    {
                        // must have same type
                        // this is artificial constraint not imposed by schema
                        // but required for code generation
                        assert(0);
                        result = -1;
                        break;
                    }
                }
            }
            ++elitr;
        }
    }
    if(el->type != NULL)
    {
        xsd_type* eltype = xsd_find_type(schema, el->type);
        if(el->name == NULL)
        {
            // must have a name
            assert(0);
            result = -1;
        }
        if(eltype == NULL)
        {
            // make sure the element type exists
            assert(0);
            result = -1;
        }
    }
    else if(el->ref != NULL)
    {
        xsd_element* elref = xsd_find_element(schema, el->ref);
        if(elref == NULL)
        {
            // make sure the referenced element exists
            assert(0);
            result = -1;
        }
    }
    else
    {
        assert(0);
        result = -1;
    }
    return result;
}

//****************************************************************************
static int xsd_validate_group(
    xsd_schema* schema,
    xsd_group** list,
    size_t listsize,
    xsd_group* grp)
{
    int result = 0;
    if(grp->name != NULL)
    {
        // make sure all groups in the list have a unique name
        xsd_group** grpitr = list;
        xsd_group** grpend = grpitr + listsize;
        while(grpitr != grpend)
        {
            xsd_group* grpother = *grpitr;
            if(grpother != grp)
            {
                if(grpother->name!=NULL && !strcmp(grpother->name,grp->name))
                {
                    // name must be unique
                    assert(0);
                    result = -1;
                    break;
                }
            }
            ++grpitr;
        }
    }
    if(grp->ref != NULL)
    {
        xsd_group* grpref = xsd_find_group(schema, grp->ref);
        if(grpref == NULL)
        {
            // make sure the referenced group exists
            assert(0);
            result = -1;
        }
        if(grp->numelements > 0)
        {
            // references should not define elements themselves
            assert(0);
            result = -1;
        }
    }
    else
    {
        xsd_element** elitr = grp->elements;
        xsd_element** elend = elitr + grp->numelements;
        while(elitr != elend && result == 0)
        {
            result = xsd_validate_element(
                schema,
                grp->elements,
                grp->numelements,
                *elitr);
            ++elitr;
        }
    }
    return result;
}


//****************************************************************************
xsd_attrib* xsd_add_attrib_global(
    xsd_schema* schema,
    const char* name,
    const char* type,
    const char* ref,
    const char* deflt,
    int required)
{
    return xsd_add_attrib(
        schema,
        &schema->attribs,
        &schema->numattribs,
        name,
        type,
        ref,
        deflt,
        required);
}

//****************************************************************************
xsd_attrib* xsd_add_attrib_to_type(
    xsd_schema* schema,
    xsd_type* t,
    const char* name,
    const char* type,
    const char* ref,
    const char* deflt,
    int required)
{
    assert(t->hascomplex);
    return xsd_add_attrib(
        schema,
        &t->complex.attribs,
        &t->complex.numattribs,
        name,
        type,
        ref,
        deflt,
        required);
}

//****************************************************************************
xsd_element* xsd_add_element_global(
    xsd_schema* schema,
    const char* name,
    const char* type,
    const char* ref,
    int minoccurs,
    int maxoccurs)
{
    return xsd_add_element(
        schema,
        &schema->elements,
        &schema->numelements,
        name,
        type,
        ref,
        minoccurs,
        maxoccurs,
        0);
}

//****************************************************************************
xsd_element* xsd_add_element_to_group(
    xsd_schema* schema,
    xsd_group* grp,
    const char* name,
    const char* type,
    const char* ref,
    int minoccurs,
    int maxoccurs,
    int seq)
{
    if(seq > grp->seqmax)
    {
        grp->seqmax = seq;
    }
    return xsd_add_element(
        schema,
        &grp->elements,
        &grp->numelements,
        name,
        type,
        ref,
        minoccurs,
        maxoccurs,
        seq);
}

//****************************************************************************
xsd_element* xsd_add_element_to_type(
    xsd_schema* schema,
    xsd_type* t,
    const char* name,
    const char* type,
    const char* ref,
    int minoccurs,
    int maxoccurs,
    int seq)
{
    assert(t->hascomplex);
    if(seq > t->complex.seqmax)
    {
        t->complex.seqmax = seq;
    }
    return xsd_add_element(
        schema,
        &t->complex.elements,
        &t->complex.numelements,
        name,
        type,
        ref,
        minoccurs,
        maxoccurs,
        seq);
}

//****************************************************************************
xsd_group* xsd_add_group_global(
    xsd_schema* schema,
    const char* name,
    const char* ref)
{
    return xsd_add_group(
        schema,
        &schema->groups,
        &schema->numgroups,
        name,
        ref,
        0);
}

//****************************************************************************
xsd_group* xsd_add_group_to_type(
    xsd_schema* schema,
    xsd_type* t,
    const char* name,
    const char* ref,
    int seq)
{
    assert(t->hascomplex);
    if(seq > t->complex.seqmax)
    {
        t->complex.seqmax = seq;
    }
    return xsd_add_group(
        schema,
        &t->complex.groups,
        &t->complex.numgroups,
        name,
        ref,
        seq);
}

//****************************************************************************
xsd_type* xsd_add_type(
    xsd_schema* schema,
    const char* name)
{
    enum { CHUNK = 1024 };
    xsd_type* t = (xsd_type*) calloc(1, sizeof(*t));
    size_t n = schema->numtypes;
    size_t cap = (n + CHUNK-1) & ~(CHUNK-1);
    size_t ncap = (n + CHUNK) & ~(CHUNK-1);
    if(ncap > cap)
    {
        schema->types = (xsd_type**) realloc(schema->types, ncap*sizeof(t));
    }
    schema->types[n] = t;
    ++schema->numtypes;
    t->name = xsd_strdup(name);
    t->simple.minoccurs = xsd_UNSET;
    t->simple.maxoccurs = xsd_UNSET;
    return t;
}

//****************************************************************************
void xsd_create_schema(
    xsd_schema* schema_out)
{
    memset(schema_out, 0, sizeof(*schema_out));
    xsd_predefine_types(schema_out);
}

//****************************************************************************
void xsd_destroy_schema(
    xsd_schema* schema)
{
    xsd_type** titr = schema->types;
    xsd_type** tend = titr + schema->numtypes;
    xsd_destroy_attribs(schema->attribs, schema->numattribs);
    xsd_destroy_elements(schema->elements, schema->numelements);
    xsd_destroy_groups(schema->groups, schema->numgroups);
    while(titr != tend)
    {
        xsd_type* t = *titr;
        xsd_destroy_type(t);
        ++titr;
    }
    free(schema->types);
}

//****************************************************************************
xsd_attrib* xsd_find_attrib(
    xsd_schema* schema,
    const char* name)
{
    xsd_attrib* result = NULL;
    xsd_attrib** atitr = schema->attribs;
    xsd_attrib** atend = atitr + schema->numattribs;
    while(atitr != atend)
    {
        xsd_attrib* at = *atitr;
        if(!strcmp(at->name, name))
        {
            result = at;
            break;
        }
        ++atitr;
    }
    return result;
}

//****************************************************************************
xsd_element* xsd_find_element(
    xsd_schema* schema,
    const char* name)
{
    xsd_element* result = NULL;
    xsd_element** elitr = schema->elements;
    xsd_element** elend = elitr + schema->numelements;
    while(elitr != elend)
    {
        xsd_element* el = *elitr;
        if(!strcmp(el->name, name))
        {
            result = el;
            break;
        }
        ++elitr;
    }
    return result;
}

//****************************************************************************
xsd_group* xsd_find_group(
    xsd_schema* schema,
    const char* name)
{
    xsd_group* result = NULL;
    xsd_group** grpitr = schema->groups;
    xsd_group** grpend = grpitr + schema->numgroups;
    while(grpitr != grpend)
    {
        xsd_group* grp = *grpitr;
        if(!strcmp(grp->name, name))
        {
            result = grp;
            break;
        }
        ++grpitr;
    }
    return result;
}

//****************************************************************************
xsd_type* xsd_find_type(
    xsd_schema* schema,
    const char* name)
{
    xsd_type* result = NULL;
    xsd_type** titr = schema->types;
    xsd_type** tend = titr + schema->numtypes;
    while(titr != tend)
    {
        xsd_type* t = *titr;
        if(!strcmp(t->name, name))
        {
            result = t;
            break;
        }
        ++titr;
    }
    return result;
}

//****************************************************************************
void xsd_flatten_schema(
    xsd_schema* schema)
{
    xsd_flatten_attribs(schema);
    xsd_flatten_elements(schema);
    xsd_flatten_groups(schema);
    xsd_flatten_types(schema);
}

//****************************************************************************
void xsd_remove_type(
    xsd_schema* schema,
    size_t index)
{
    xsd_type* t = schema->types[index];
    xsd_type** titr = schema->types + index;
    xsd_type** tend = schema->types + schema->numtypes - 1;
    xsd_destroy_type(t);
    while(titr != tend)
    {
        *titr = titr[1];
        ++titr;
    }
    --schema->numtypes;
}

//****************************************************************************
void xsd_remove_unreferenced_types(
    xsd_schema* schema)
{
    // culls types that are not referenced by anything
    int changed;
    do
    {
        // this is an iterative process that must be repeated until
        // no changes are made
        xsd_type** titr = schema->types + schema->numtypes - 1;
        xsd_type** tend = schema->types - 1;
        changed = 0;
        while(titr != tend)
        {
            // loop backward, removing from end
            xsd_type* t = *titr;
            char* type = t->name;
            xsd_attrib** atitr;
            xsd_attrib** atend;
            xsd_element** elitr;
            xsd_element** elend;
            xsd_group** grpitr;
            xsd_group** grpend;
            xsd_type** inrtitr;
            xsd_type** inrtend;
            int remove = 1;
            // look for top level attributes that reference t
            atitr = schema->attribs;
            atend = atitr + schema->numattribs;
            while(atitr != atend && remove)
            {
                xsd_attrib* at = *atitr;
                if(at->type != NULL && !strcmp(at->type, type))
                {
                    remove = 0;
                    break;
                }
                ++atitr;
            }
            // look for top level elements that reference t
            elitr = schema->elements;
            elend = elitr + schema->numelements;
            while(elitr != elend && remove)
            {
                xsd_element* el = *elitr;
                if(el->type != NULL && !strcmp(el->type, type))
                {
                    remove = 0;
                    break;
                }
                ++elitr;
            }
            // look for top level groups that reference t
            grpitr = schema->groups;
            grpend = grpitr + schema->numgroups;
            while(grpitr != grpend && remove)
            {
                xsd_group* grp = *grpitr;
                elitr = grp->elements;
                elend = elitr + grp->numelements;
                while(elitr != elend && remove)
                {
                    xsd_element* el = *elitr;
                    if(el->type != NULL && !strcmp(el->type, type))
                    {
                        remove = 0;
                        break;
                    }
                    ++elitr;
                }
                ++grpitr;
            }
            // look for types that reference t
            inrtitr = schema->types;
            inrtend = inrtitr + schema->numtypes;
            while(inrtitr != inrtend && remove)
            {
                xsd_type* inrt = *inrtitr;
                if(inrt != t)
                {
                    if(inrt->base != NULL)
                    {
                        if(!strcmp(inrt->base, type))
                        {
                            remove = 0;
                            break;
                        }
                    }
                    if(inrt->hassimple)
                    {
                        if(inrt->simple.itemtype != NULL)
                        {
                            if(!strcmp(inrt->simple.itemtype, type))
                            {
                                remove = 0;
                                break;
                            }
                        }
                    }
                    if(inrt->hascomplex)
                    {
                        // look for attributes that reference t
                        atitr = inrt->complex.attribs;
                        atend = atitr + inrt->complex.numattribs;
                        while(atitr != atend && remove)
                        {
                            xsd_attrib* at = *atitr;
                            if(at->type != NULL && !strcmp(at->type, type))
                            {
                                remove = 0;
                                break;
                            }
                            ++atitr;
                        }
                        // look for elements that reference t
                        elitr = inrt->complex.elements;
                        elend = elitr + inrt->complex.numelements;
                        while(elitr != elend && remove)
                        {
                            xsd_element* el = *elitr;
                            if(el->type != NULL && !strcmp(el->type, type))
                            {
                                remove = 0;
                                break;
                            }
                            ++elitr;
                        }
                        // look for groups that reference t
                        grpitr = inrt->complex.groups;
                        grpend = grpitr + inrt->complex.numgroups;
                        while(grpitr != grpend && remove)
                        {
                            xsd_group* grp = *grpitr;
                            elitr = grp->elements;
                            elend = elitr + grp->numelements;
                            while(elitr != elend && remove)
                            {
                                xsd_element* el = *elitr;
                                if(el->type!=NULL && !strcmp(el->type,type))
                                {
                                    remove = 0;
                                    break;
                                }
                                ++elitr;
                            }
                            ++grpitr;
                        }
                    }
                }
                ++inrtitr;
            }
            if(remove)
            {
                xsd_remove_type(schema, (int) (titr - schema->types));
                changed = 1;
            }
            --titr;
        }
    }
    while(changed);
}

//****************************************************************************
char* xsd_strdup(
    const char* str)
{
    char* dup = NULL;
    if(str != NULL && *str != '\0')
    {
        size_t sz = strlen(str) + 1;
        dup = (char*) malloc(sz);
        memcpy(dup, str, sz);
    }
    return dup;
}

//****************************************************************************
int xsd_validate_schema(
    xsd_schema* schema)
{
    int result = 0;
    xsd_attrib** atitr;
    xsd_attrib** atend;
    xsd_element** elitr;
    xsd_element** elend;
    xsd_group** grpitr;
    xsd_group** grpend;
    xsd_type** titr;
    xsd_type** tend;
    // validate top level attributes
    atitr = schema->attribs;
    atend = atitr + schema->numattribs;
    while(atitr != atend && result == 0)
    {
        result = xsd_validate_attrib(
            schema,
            schema->attribs,
            schema->numattribs,
            *atitr);
        ++atitr;
    }
    // validate top level elements
    elitr = schema->elements;
    elend = elitr + schema->numelements;
    if(elitr != NULL)
    {
        while(elitr != elend && result == 0)
        {
            result = xsd_validate_element(
                schema,
                schema->elements,
                schema->numelements,
                *elitr);
            ++elitr;
        }
    }
    else
    {
        // should have at least one top level element
        assert(0);
        result = -1;
    }
    // validate top level groups
    grpitr = schema->groups;
    grpend = grpitr + schema->numgroups;
    while(grpitr != grpend && result == 0)
    {
        result = xsd_validate_group(
            schema,
            schema->groups,
            schema->numgroups,
            *grpitr);
        ++grpitr;
    }
    // validate types
    titr = schema->types;
    tend = titr + schema->numtypes;
    while(titr != tend && result == 0)
    {
        xsd_type* t = *titr;
        xsd_type** inrtitr = schema->types;
        xsd_type** inrtend = inrtitr + schema->numtypes;
        const char* base = t->base;
        while(inrtitr != inrtend)
        {
            // search for items with duplicate type names
            xsd_type* inrt = *inrtitr;
            if(inrt != t && !strcmp(inrt->name, t->name))
            {
                assert(0);
                result = -1;
                break;
            }
            ++inrtitr;
        }
        if(t->hassimple)
        {
            if(t->simple.itemtype == NULL && base == NULL)
            {
                // make sure that it actually does have simple content
                assert(0);
                result = -1;
            }
        }
        if(t->hascomplex)
        {
            atitr = t->complex.attribs;
            atend = atitr + t->complex.numattribs;
            while(atitr != atend && result == 0)
            {
                result = xsd_validate_attrib(
                    schema,
                    t->complex.attribs,
                    t->complex.numattribs,
                    *atitr);
                ++atitr;
            }
            elitr = t->complex.elements;
            elend = elitr + t->complex.numelements;
            while(elitr != elend && result == 0)
            {
                result = xsd_validate_element(
                    schema,
                    t->complex.elements,
                    t->complex.numelements,
                    *elitr);
                ++elitr;
            }
            grpitr = t->complex.groups;
            grpend = grpitr + t->complex.numgroups;
            while(grpitr != grpend)
            {
                result = xsd_validate_group(
                    schema,
                    t->complex.groups,
                    t->complex.numgroups,
                    *grpitr);
                ++grpitr;
            }
        }
        ++titr;
    }
    return result;
}
