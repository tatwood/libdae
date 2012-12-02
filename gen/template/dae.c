/**
 * @brief     libdae source
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#include "dae.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dae_GET_HEADER(pobj_) \
    ((dae_obj_header*) (((ptrdiff_t) pobj_)-((sizeof(dae_obj_header)+7)&~7)))

#define dae_GET_PTR(phdr_) \
    ((dae_obj_ptr)  (((ptrdiff_t) phdr_)+((sizeof(dae_obj_header)+7)&~7)))

enum dae_obj_memberdeftype_e
{
    dae_MEMBER_ATTRIB,
    dae_MEMBER_ELEM
};

enum dae_obj_flags_e
{
    dae_XSD_ANY = 1 << 0,
};

typedef enum dae_obj_memberdeftype_e dae_obj_memberdeftype;
typedef enum dae_obj_flags_e dae_obj_flags;

typedef struct dae_obj_memberdef_s dae_obj_memberdef;
typedef struct dae_obj_typedef_s dae_obj_typedef;
typedef struct dae_obj_vector_s dae_obj_vector;
typedef struct dae_obj_list_s dae_obj_list;
typedef struct dae_obj_header_s dae_obj_header;

struct dae_obj_memberdef_s
{
    const char* name;
    const char* deflt;
    unsigned offset;
    short objtypeid;
    short min;
    short max;
    short seq;
};

struct dae_obj_typedef_s
{
    const char* name;
    dae_obj_memberdef* attribs;
    dae_obj_memberdef* elems;
    unsigned flags;
    unsigned size;
    unsigned dataoffset;
    short objtypeid;
    short datatypeid;
    unsigned short numattribs;
    unsigned short numelems;
    short datamin;
    short datamax;
};

struct dae_obj_vector_s
{
    void* values;
    size_t size;
};

struct dae_obj_list_s
{
    dae_obj_header* head;
    dae_obj_header* tail;
};

struct dae_obj_header_s
{
    dae_obj_header* parent;
    dae_obj_list* parentlist;
    const dae_obj_memberdef* memberdef;
    const char* membername;
    int structindex;
    const dae_obj_typedef* def;
    dae_obj_list attribs;
    dae_obj_list elems;
    dae_obj_header* prev;
    dae_obj_header* next;
};

static void dae_free(
    void* ptr);

static char* dae_strdup(
    const char* str);

static dae_obj_ptr dae_create_obj(
    const dae_obj_typedef* def);

static dae_obj_ptr dae_add_obj(
    dae_obj_header* parenthdr,
    dae_obj_list* parentlist,
    const dae_obj_memberdef* memberdef,
    const char* membername,
    const dae_obj_typedef* childdef,
    size_t childsize);

static void dae_destroy_obj(
    dae_obj_header* hdr);

static size_t dae_convert_string_floats(
    const char* str,
    float* floats_out);

static size_t dae_convert_string_int8s(
    const char* str,
    char* ints_out);

static size_t dae_convert_string_int16s(
    const char* str,
    short* ints_out);

static size_t dae_convert_string_int32s(
    const char* str,
    int* ints_out);

static size_t dae_convert_string_uint8s(
    const char* str,
    unsigned char* ints_out);

static size_t dae_convert_string_uint32s(
    const char* str,
    unsigned* ints_out);

static size_t dae_convert_string_words(
    const char* str,
    char** strings_out);

static size_t dae_count_string_floats(
    const char* str);

static size_t dae_count_string_int8s(
    const char* str);

static size_t dae_count_string_int16s(
    const char* str);

static size_t dae_count_string_int32s(
    const char* str);

static size_t dae_count_string_uint8s(
    const char* str);

static size_t dae_count_string_uint32s(
    const char* str);

static size_t dae_count_string_words(
    const char* str);

static void dae_get_schema(
    dae_obj_typedef** types,
    unsigned* numtypes);

static dae_obj_typedef* dae_get_type(
    unsigned objtypeid);

static dae_obj_typedef* dae_find_type(
    const char* name);

//****************************************************************************
/*GEN_SCHEMA_BGN*/
static void dae_build_schema(
    dae_obj_typedef** types_out,
    unsigned* numtypes_out)
{
}
/*GEN_SCHEMA_END*/

//****************************************************************************
static void dae_free(void* ptr)
{
    free(ptr);
}

//****************************************************************************
static char* dae_strdup(
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
static dae_obj_ptr dae_create_obj(
    const dae_obj_typedef* def)
{
    size_t objsize = (def != NULL) ? def->size : sizeof(char*);
    size_t bufsize = ((ptrdiff_t) dae_GET_PTR(0)) + objsize;
    dae_obj_header* hdr = (dae_obj_header*) malloc(bufsize);
    dae_obj_ptr obj = dae_GET_PTR(hdr);
    memset(hdr, 0, bufsize);
    hdr->structindex = -1;
    hdr->def = def;
    return obj;
}

//****************************************************************************
static dae_obj_ptr dae_add_obj(
    dae_obj_header* parenthdr,
    dae_obj_list* parentlist,
    const dae_obj_memberdef* memberdef,
    const char* membername,
    const dae_obj_typedef* childdef,
    size_t childsize)
{
    void* parentobj = dae_GET_PTR(parenthdr);
    void* childobj = dae_create_obj(childdef);
    dae_obj_header* childhdr = dae_GET_HEADER(childobj);
    // initialize header information
    childhdr->parent = parenthdr;
    childhdr->parentlist = parentlist;
    childhdr->memberdef = memberdef;
    // add element to parent's header
    childhdr->prev = parentlist->tail;
    childhdr->next = NULL;
    if(parentlist->tail != NULL)
    {
        parentlist->tail->next = childhdr;
    }
    else
    {
        parentlist->head = childhdr;
    }
    parentlist->tail = childhdr;
    // add element to parent's struct
    if(memberdef != NULL)
    {
        int offset = memberdef->offset;
        int max = memberdef->max;
        if(max == 1)
        {
            // single instance pointer
            void** mbr = (void**) (((ptrdiff_t) parentobj)+offset);
            if(*mbr == NULL)
            {
                childhdr->structindex = 0;
                *mbr = childobj;
            }
        }
        else if(max < 0)
        {
            // variable sized vector
            dae_obj_vector* vec;
            void** buf;
            size_t i;
            vec = (dae_obj_vector*) (((ptrdiff_t) parentobj)+offset);
            i = vec->size;
            childhdr->structindex = i;
            buf = (void**) realloc(vec->values, (i+1) * sizeof(*buf));
            buf[i] = childobj;
            vec->values = buf;
            ++vec->size;
        }
        else
        {
            // fixed length array
            void** mbr = (void**) (((ptrdiff_t) parentobj)+offset);
            void** mbritr = mbr;
            void** mbrend = mbritr + max;
            while(mbritr != mbrend)
            {
                if(*mbritr == NULL)
                {
                    childhdr->structindex = (int) (mbritr - mbr);
                    *mbritr = childobj;
                    break;
                }
                ++mbritr;
            }
        }
        // if member definition exists, re-use name string from definition
        childhdr->membername = memberdef->name;
    }
    else
    {
        // if no member definition exists, need to make a copy of name string
        childhdr->membername = dae_strdup(membername);
    }
    return (dae_obj_ptr) childobj;
}

//****************************************************************************
static void dae_destroy_obj(
    dae_obj_header* hdr)
{
    dae_obj_header* itr = hdr;
    while(1)
    {
        dae_obj_header* next;
        // move down to bottom of tree
        while(itr->elems.head != NULL)
        {
            itr = itr->elems.head;
        }
        while(itr->attribs.head != NULL)
        {
            itr = itr->attribs.head;
        }
        if(itr->next != NULL)
        {
            next = itr->next;
        }
        else
        {
            next = itr->parent;
            if(itr->parentlist != NULL)
            {
                itr->parentlist->head = NULL;
                itr->parentlist->tail = NULL;
            }
        }
        if(itr->attribs.head == NULL && itr->elems.head == NULL)
        {
            void* obj = dae_GET_PTR(itr);
            dae_obj_typeid datatype = dae_ID_INVALID;
            int datamax;
            size_t dataoff;
            if(itr->memberdef == NULL)
            {
                // if the object had a member definition, the member name
                // was derived from the def and does not need to be freed.
                // otherwise, it does
                dae_free((char*) itr->membername);
            }
            if(itr->def != NULL)
            {
                // clean up struct members
                const dae_obj_typedef* def = itr->def;
                const dae_obj_memberdef* mbritr = def->elems;
                const dae_obj_memberdef* mbrend = mbritr + def->numelems;
                while(mbritr != mbrend)
                {
                    if(mbritr->max < 0)
                    {
                        // if the struct member is a variable sized vector
                        // the buffer needs to be freed.
                        dae_obj_vector* vec;
                        size_t off;
                        off = mbritr->offset;
                        vec = (dae_obj_vector*) (((ptrdiff_t) obj) + off);
                        if(vec->values != NULL)
                        {
                            dae_free(vec->values);
                            // set buffer to NULL to prevent multiple free as
                            // multiple element definitions may reference it
                            vec->values = NULL;
                        }
                    }
                    ++mbritr;
                }
                datatype = (dae_obj_typeid) def->datatypeid;
                datamax = def->datamax;
                dataoff = def->dataoffset;
            }
            else
            {
                datatype = dae_ID_STRING;
                datamax = 1;
                dataoff = 0;
            }
            if(datatype != dae_ID_INVALID)
            {
                if(datamax == -1)
                {
                    // if it's a variable sized vector, free the buffer
                    dae_obj_vector* vec;
                    vec = (dae_obj_vector*) (((ptrdiff_t) obj) + dataoff);
                    if(datatype == dae_ID_STRING)
                    {
                        // if it's a vector of strings, all the strings need
                        // to be freed individually
                        char** sitr = (char**) vec->values;
                        char** send = sitr + vec->size;
                        while(sitr != send)
                        {
                            dae_free(*sitr);
                            ++sitr;
                        }
                    }
                    dae_free(vec->values);
                }
                else if(datatype == dae_ID_STRING)
                {
                    // single or fixed size array of strings
                    char** sitr = (char**) (((ptrdiff_t) obj) + dataoff);
                    char** send = sitr + datamax;
                    while(sitr != send)
                    {
                        dae_free(*sitr);
                        ++sitr;
                    }
                }
            }
            dae_free(itr);
            if(itr == hdr)
            {
                // if the freed item was the selected object, stop
                break;
            }
            itr = next;
        }
    }
}

//****************************************************************************
static size_t dae_convert_string_floats(
    const char* str,
    float* floats_out)
{
    // parses the string into an array of floats
    // the array must be large enough to hold the result of
    // dae_count_float_string
    size_t n = 0;
    while(*str != '\0')
    {
        float f = 0.0f;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to float
        if(sscanf(str, "%f", &f) == 1)
        {
            floats_out[n] = f;
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_convert_string_int8s(
    const char* str,
    char* ints_out)
{
    // counts the number of ints that can be parsed from the string
    size_t n = 0;
    while(*str != '\0')
    {
        int i = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to int
        if(sscanf(str, "%i", &i) == 1)
        {
            ints_out[n] = (char) i;
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_convert_string_int16s(
    const char* str,
    short* ints_out)
{
    // counts the number of ints that can be parsed from the string
    size_t n = 0;
    while(*str != '\0')
    {
        int i = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to int
        if(sscanf(str, "%i", &i) == 1)
        {
            ints_out[n] = (short) i;
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_convert_string_int32s(
    const char* str,
    int* ints_out)
{
    // counts the number of ints that can be parsed from the string
    size_t n = 0;
    while(*str != '\0')
    {
        int i = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to int
        if(sscanf(str, "%i", &i) == 1)
        {
            ints_out[n] = i;
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_convert_string_uint8s(
    const char* str,
    unsigned char* ints_out)
{
    // counts the number of ints that can be parsed from the string
    size_t n = 0;
    while(*str != '\0')
    {
        unsigned i = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to int
        if(sscanf(str, "%i", &i) == 1)
        {
            ints_out[n] = (unsigned char) i;
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_convert_string_uint32s(
    const char* str,
    unsigned* ints_out)
{
    // counts the number of ints that can be parsed from the string
    size_t n = 0;
    while(*str != '\0')
    {
        unsigned i = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to int
        if(sscanf(str, "%u", &i) == 1)
        {
            ints_out[n] = i;
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_convert_string_words(
    const char* str,
    char** strings_out)
{
    size_t n = 0;
    while(*str != '\0')
    {
        size_t len = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // determine length of trimmed word
        while(str[len] != '\0' && !isspace(str[len]))
        {
            ++len;
        }
        // duplicate trimmed word
        if(len > 0)
        {
            char* dup = (char*) malloc(len + 1);
            memcpy(dup, str, len);
            dup[len] = '\0';
            strings_out[n] = dup;
            ++n;
        }
        // find the next set of whitespace
        str += len;
    }
    return n;
}

//****************************************************************************
static size_t dae_count_string_floats(
    const char* str)
{
    // counts the number of floats that can be parsed from the string
    size_t n = 0;
    while(*str != '\0')
    {
        float f = 0.0f;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to float
        if(sscanf(str, "%f", &f) == 1)
        {
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_count_string_int8s(
    const char* str)
{
    return dae_count_string_int32s(str);
}

//****************************************************************************
static size_t dae_count_string_int16s(
    const char* str)
{
    return dae_count_string_int32s(str);
}

//****************************************************************************
static size_t dae_count_string_int32s(
    const char* str)
{
    // counts the number of ints that can be parsed from the string
    size_t n = 0;
    while(*str != '\0')
    {
        int i = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to int
        if(sscanf(str, "%i", &i) == 1)
        {
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_count_string_uint8s(
    const char* str)
{
    return dae_count_string_uint32s(str);
}

//****************************************************************************
static size_t dae_count_string_uint32s(
    const char* str)
{
    // counts the number of ints that can be parsed from the string
    size_t n = 0;
    while(*str != '\0')
    {
        int i = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // attempt to convert string word to int
        if(sscanf(str, "%u", &i) == 1)
        {
            ++n;
        }
        else
        {
            break;
        }
        // find the next set of whitespace
        while(*str != '\0' && !isspace(*str))
        {
            ++str;
        }
    }
    return n;
}

//****************************************************************************
static size_t dae_count_string_words(
    const char* str)
{
    size_t n = 0;
    while(*str != '\0')
    {
        size_t len = 0;
        // move past any leading whitespace
        while(*str != '\0' && isspace(*str)) 
        {
            ++str;
        }
        // determine length of trimmed word
        while(str[len] != '\0' && !isspace(str[len]))
        {
            ++len;
        }
        // count it
        if(len > 0)
        {
            ++n;
        }
        // find the next set of whitespace
        str += len;
    }
    return n;
}

//****************************************************************************
static void dae_get_schema(
    dae_obj_typedef** types,
    unsigned* numtypes)
{
    static dae_obj_typedef* s_types;
    static unsigned s_numtypes;
    if(s_types == NULL)
    {
        dae_build_schema(&s_types, &s_numtypes);
    }
    *types = s_types;
    *numtypes = s_numtypes;
}

//****************************************************************************
static dae_obj_typedef* dae_get_type(
    unsigned objtypeid)
{
    dae_obj_typedef* types;
    unsigned numtypes;
    dae_get_schema(&types, &numtypes);
    return (objtypeid < numtypes) ? types + objtypeid : NULL;
}

//****************************************************************************
static dae_obj_typedef* dae_find_type(
    const char* name)
{
    dae_obj_typedef* result = NULL;
    dae_obj_typedef* titr;
    dae_obj_typedef* tend;
    unsigned numtypes;
    dae_get_schema(&titr, &numtypes);
    tend = titr + numtypes;
    while(titr != tend)
    {
        if(!strcmp(titr->name, name))
        {
            result = titr;
            break;
        }
        ++titr;
    }
    return result;
}

//****************************************************************************
dae_obj_ptr dae_add_attrib(
    dae_obj_ptr parent,
    const char* name,
    const char* value)
{
    dae_obj_header* parenthdr = dae_GET_HEADER(parent);
    const dae_obj_typedef* parentdef = parenthdr->def;
    const dae_obj_memberdef* at = NULL;
    const dae_obj_typedef* def = NULL;
    dae_obj_ptr obj = NULL;
    if(parentdef != NULL)
    {
        // predefined content type
        // need to find a predefined attribute
        dae_obj_memberdef* atitr = parentdef->attribs;
        dae_obj_memberdef* atend = atitr + parentdef->numattribs;
        while(atitr != atend)
        {
            if(!strcmp(name, atitr->name))
            {
                at = atitr;
                def = dae_get_type(atitr->objtypeid);
                break;
            }
            ++atitr;
        }
    }
    else
    {
        // undefined type, assume all attributes are strings
        def = dae_get_type(dae_ID_STRING);
    }
    if(def != NULL)
    {
        obj = dae_add_obj(
            parenthdr,
            &parenthdr->attribs,
            at,
            name,
            def,
            def->size);
        if(value == NULL && at != NULL)
        {
            value = at->deflt;
        }
        if(value != NULL)
        {
            dae_set_string(obj, value);
        }
    }
    return obj;
}

//****************************************************************************
dae_obj_ptr dae_add_element(
    dae_obj_ptr parent,
    const char* name)
{
    dae_obj_header* parenthdr = dae_GET_HEADER(parent);
    const dae_obj_typedef* parentdef = parenthdr->def;
    const dae_obj_memberdef* el = NULL;
    const dae_obj_typedef* def = NULL;
    dae_obj_ptr obj = NULL;
    if(parentdef!=NULL && (parentdef->flags&dae_XSD_ANY)==0)
    {
        // predefined content type
        // need to find a predefined element
        dae_obj_memberdef* elitr = parentdef->elems;
        dae_obj_memberdef* elend = elitr + parentdef->numelems;
        while(elitr != elend)
        {
            if(!strcmp(name, elitr->name))
            {
                el = elitr;
                def = dae_get_type(elitr->objtypeid);
                break;
            }
            ++elitr;
        }
    }
    else
    {
        // any element content is allowed
        def = dae_find_type(name);
    }
    if(parentdef == NULL || parentdef->datamax == 0)
    {
        obj = dae_add_obj(
            parenthdr,
            &parenthdr->elems,
            el,
            name,
            def,
            (def != NULL) ? def->size : 0);
    }
    else
    {
        assert(0);
    }
    return obj;
}

//****************************************************************************
dae_COLLADA* dae_create()
{
    const dae_obj_typedef* def = dae_get_type(dae_ID_COLLADA);
    return (dae_COLLADA*) dae_create_obj(def);
}

//****************************************************************************
void dae_destroy(
    dae_COLLADA* doc)
{
    dae_destroy_obj(dae_GET_HEADER(doc));
}

//****************************************************************************
size_t dae_get_data(
    dae_obj_ptr obj,
    dae_native_typeid* type_out,
    void** data_out,
    size_t* datalen_out)
{
    size_t datalen = 0;
    const dae_obj_typedef* def = dae_GET_HEADER(obj)->def;
    if(def != NULL)
    {
        void* data = (void*) (((ptrdiff_t) obj) + def->dataoffset);
        if(def->datamax >= 0)
        {
            // fixed size array
            datalen = def->datamax;
        }
        else
        {
            // dynamically sized vector
            dae_obj_vector* vec = (dae_obj_vector*) data;
            data = vec->values;
            datalen = vec->size;
        }
        *type_out = (dae_native_typeid) def->datatypeid;
        *data_out = data;
        *datalen_out = datalen;
    }
    return datalen;
}

//****************************************************************************
dae_obj_ptr dae_get_first_attrib(
    dae_obj_ptr obj)
{
    dae_obj_header* childhdr = dae_GET_HEADER(obj)->attribs.head;
    return (childhdr != NULL) ? dae_GET_PTR(childhdr) : NULL;
}

//****************************************************************************
dae_obj_ptr dae_get_first_element(
    dae_obj_ptr obj)
{
    dae_obj_header* childhdr = dae_GET_HEADER(obj)->elems.head;
    return (childhdr != NULL) ? dae_GET_PTR(childhdr) : NULL;
}

//****************************************************************************
const char* dae_get_name(
    dae_obj_ptr obj)
{
    return dae_GET_HEADER(obj)->membername;
}

//****************************************************************************
dae_obj_ptr dae_get_next(
    dae_obj_ptr obj)
{
    dae_obj_header* nexthdr = dae_GET_HEADER(obj)->next;
    return (nexthdr != NULL) ? dae_GET_PTR(nexthdr) : NULL;
}

//****************************************************************************
dae_obj_ptr dae_get_parent(
    dae_obj_ptr child)
{
    dae_obj_header* parenthdr = dae_GET_HEADER(child)->parent;
    return (parenthdr != NULL) ? dae_GET_PTR(parenthdr) : NULL;
}

//****************************************************************************
dae_obj_typeid dae_get_typeid(
    dae_obj_ptr obj)
{
    const dae_obj_typedef* def = dae_GET_HEADER(obj)->def;
    return (def != NULL) ? (dae_obj_typeid) def->objtypeid : dae_ID_INVALID;
}

//****************************************************************************
void dae_set_string(
    dae_obj_ptr obj,
    const char* data)
{
    dae_obj_header* hdr = dae_GET_HEADER(obj);
    const dae_obj_typedef* def = hdr->def;
    dae_native_typeid datatype = dae_NATIVE_STRING;
    int dataoffset = -1;
    int max;
    if(def != NULL)
    {
        if(def->datatypeid != dae_ID_INVALID)
        {
            datatype = (dae_native_typeid) def->datatypeid;
            dataoffset = def->dataoffset;
            max = def->datamax;
        }
    }
    else
    {
        // if this is a generic element with no type definition, make it a
        // pointer to a string
        datatype = dae_NATIVE_STRING;
        dataoffset = 0;
        max = 1;
    }
    if(dataoffset >= 0)
    {
        if(max == 1)
        {
            void* p = (void*) (((ptrdiff_t) obj) + dataoffset);
            // trim leading space
            while(*data != '\0' && isspace(*data))
            {
                ++data;
            }
            switch(datatype)
            {
            case dae_NATIVE_BOOL32:
                // TODO
                break;
            case dae_NATIVE_HEX8:
                // TODO
                break;
            case dae_NATIVE_FLOAT:
                {
                    float f = 0;
                    if(sscanf(data, "%f", &f) == 1)
                    {
                        float* fp = (float*) p;
                        *fp = (float) f;
                    }
                }
                break;
            case dae_NATIVE_INT8:
                {
                    int i = 0;
                    if(sscanf(data, "%i", &i) == 1)
                    {
                        char* ip = (char*) p;
                        *ip = (char) i;
                    }
                }
                break;
            case dae_NATIVE_INT16:
                {
                    int i = 0;
                    if(sscanf(data, "%i", &i) == 1)
                    {
                        short* ip = (short*) p;
                        *ip = (short) i;
                    }
                }
                break;
            case dae_NATIVE_INT32:
                {
                    int i = 0;
                    if(sscanf(data, "%i", &i) == 1)
                    {
                        int* ip = (int*) p;
                        *ip = (int) i;
                    }
                }
                break;
            case dae_NATIVE_STRING:
                {
                    // TODO: free old pointer?
                    size_t len = 0;
                    // trim trailing space
                    while(data[len] != '\0' && !isspace(data[len]))
                    {
                        ++len;
                    }
                    if(len > 0)
                    {
                        char** sp = (char**) p;
                        char* dup = (char*) malloc(len + 1);
                        memcpy(dup, data, len);
                        dup[len] = '\0';
                        *sp = dup;
                    }
                }
                break;
            case dae_NATIVE_UINT8:
                {
                    unsigned i = 0;
                    if(sscanf(data, "%u", &i) == 1)
                    {
                        unsigned char* ip = (unsigned char*) p;
                        *ip = (unsigned char) i;
                    }
                }
                break;
            case dae_NATIVE_UINT32:
                {
                    unsigned i = 0;
                    if(sscanf(data, "%u", &i) == 1)
                    {
                        unsigned* ip = (unsigned *) p;
                        *ip = (unsigned) i;
                    }
                }
                break;
            default:
                assert(0);
                break;
            }
        }
        else if(max == -1)
        {
            dae_obj_vector* vec;
            vec = (dae_obj_vector*) (((ptrdiff_t) obj) + dataoffset);
            switch(datatype)
            {
            case dae_NATIVE_BOOL32:
                // TODO
                break;
            case dae_NATIVE_HEX8:
                // TODO
                break;
            case dae_NATIVE_FLOAT:
                {
                    size_t n = dae_count_string_floats(data);
                    if(n > 0)
                    {
                        float* fv = (float*) vec->values;
                        if(n > vec->size)
                        {
                            fv = (float*) realloc(fv, n*sizeof(*fv));
                            vec->values = fv;
                        }
                        vec->size = dae_convert_string_floats(data, fv);
                    }
                }
                break;
            case dae_NATIVE_INT8:
                {
                    size_t n = dae_count_string_int8s(data);
                    if(n > 0)
                    {
                        char* iv = (char*) vec->values;
                        if(n > vec->size)
                        {
                            iv = (char*) realloc(iv, n*sizeof(*iv));
                            vec->values = iv;
                        }
                        vec->size = dae_convert_string_int8s(data, iv);
                    }
                }
                break;
            case dae_NATIVE_INT16:
                {
                    size_t n = dae_count_string_int16s(data);
                    if(n > 0)
                    {
                        short* iv = (short*) vec->values;
                        if(n > vec->size)
                        {
                            iv = (short*) realloc(iv, n*sizeof(*iv));
                            vec->values = iv;
                        } 
                        vec->size = dae_convert_string_int16s(data, iv);
                    }
                }
                break;
            case dae_NATIVE_INT32:
                {
                    size_t n = dae_count_string_int32s(data);
                    if(n > 0)
                    {
                        int* iv = (int*) vec->values;
                        if(n > vec->size)
                        {
                            iv = (int*) realloc(iv, n*sizeof(*iv));
                            vec->values = iv;
                        }
                        vec->size = dae_convert_string_int32s(data, iv);
                    }
                }
                break;
            case dae_NATIVE_STRING:
                {
                    size_t n = dae_count_string_words(data);
                    if(n > 0)
                    {
                        // TODO: free old pointers?
                        char** sv = (char**) vec->values;
                        if(n > vec->size)
                        {
                            sv = (char**) realloc(sv, n*sizeof(*sv));
                            vec->values = sv;
                        }
                        vec->size = dae_convert_string_words(data, sv);
                    }
                }
                break;
            case dae_NATIVE_UINT8:
                {
                    size_t n = dae_count_string_uint8s(data);
                    if(n > 0)
                    {
                        unsigned char* iv = (unsigned char*) vec->values;
                        if(n > vec->size)
                        {
                            iv = (unsigned char*) realloc(iv, n*sizeof(*iv));
                            vec->values = iv;
                        }
                        vec->size = dae_convert_string_uint8s(data, iv);
                    }
                }
                break;
            case dae_NATIVE_UINT32:
                {
                    size_t n = dae_count_string_uint32s(data);
                    if(n > 0)
                    {
                        unsigned* iv = (unsigned*) vec->values;
                        if(n > vec->size)
                        {
                            iv = (unsigned*) realloc(iv, n*sizeof(*iv));
                            vec->values = iv;
                        }
                        vec->size = dae_convert_string_uint32s(data, iv);
                    }
                }
                break;
            default:
                assert(0);
                break;
            }
        }
        else
        {
            // fixed size array
            void* ar = (void*) (((ptrdiff_t) obj) + dataoffset);
            switch(datatype)
            {
            case dae_NATIVE_BOOL32:
                // TODO
                break;
            case dae_NATIVE_HEX8:
                // TODO
                break;
            case dae_NATIVE_FLOAT:
                {
                    int n = dae_count_string_floats(data);
                    if(n == max)
                    {
                        float* fv = (float*) ar;
                        dae_convert_string_floats(data, fv);
                    }
                }
                break;
            case dae_NATIVE_INT8:
                {
                    int n = dae_count_string_int8s(data);
                    if(n == max)
                    {
                        char* iv = (char*) ar;
                        dae_convert_string_int8s(data, iv);
                    }
                }
                break;
            case dae_NATIVE_INT16:
                {
                    int n = dae_count_string_int16s(data);
                    if(n == max)
                    {
                        short* iv = (short*) ar;
                        dae_convert_string_int16s(data, iv);
                    }
                }
                break;
            case dae_NATIVE_INT32:
                {
                    int n = dae_count_string_int32s(data);
                    if(n == max)
                    {
                        int* iv = (int*) ar;
                        dae_convert_string_int32s(data, iv);
                    }
                }
                break;
            case dae_NATIVE_STRING:
                {
                    int n = dae_count_string_words(data);
                    if(n == max)
                    {
                        // TODO: free old pointers?
                        char** sv = (char**) ar;
                        dae_convert_string_words(data, sv);
                    }
                }
                break;
            case dae_NATIVE_UINT8:
                {
                    int n = dae_count_string_uint8s(data);
                    if(n == max)
                    {
                        unsigned char* iv = (unsigned char*) ar;
                        dae_convert_string_uint8s(data, iv);
                    }
                }
                break;
            case dae_NATIVE_UINT32:
                {
                    int n = dae_count_string_uint32s(data);
                    if(n == max)
                    {
                        unsigned* iv = (unsigned*)ar;
                        dae_convert_string_uint32s(data, iv);
                    }
                }
                break;
            default:
                assert(0);
                break;
            }
        }
    }
}
