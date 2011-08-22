/**
 * @brief     libdae source
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#include <dae/dae.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//****************************************************************************

typedef enum dae_datatype_e dae_datatype;
typedef enum dae_restrict_e dae_restrict;
typedef union dae_data_u dae_data;
typedef struct dae_attrinfo_s dae_attrinfo;
typedef struct dae_eleminfo_s dae_eleminfo;
typedef struct dae_structinfo_s dae_structinfo;
typedef struct dae_schemainfo_s dae_schemainfo;
typedef struct dae_datalist_s dae_datalist;
typedef struct dae_elemlist_s dae_elemlist;
typedef struct dae_parseelement_s dae_parseelement;
typedef struct dae_parserinternal_s dae_parserinternal;

enum dae_datatype_e
{
    dae_DATA_NONE,
    dae_DATA_BOOL,
    dae_DATA_HEX,
    dae_DATA_INT,
    dae_DATA_FLOAT,
    dae_DATA_STRING,
    dae_DATA_UINT,
    dae_DATA_XML
};

enum dae_restrict_e
{
    dae_RESTRICT_CHOICE,   // union member
    dae_RESTRICT_LIST,     // multiple instances
    dae_RESTRICT_OPTIONAL, // single, optional element
    dae_RESTRICT_REQUIRED, // single, required element
};

union dae_data_u
{
    int bool_val;
    int int_val;
    float float_val;
    char* string_val;
    unsigned uint_val;
    char* xml_val;
};

struct dae_attrinfo_s
{
    const char* name;
    dae_datatype datatype;
    size_t offset;
    dae_data defaultval;
};

struct dae_eleminfo_s
{
    const char* name;
    dae_typeid dae_type;
    dae_restrict restrict;
    size_t offset;
};

struct dae_structinfo_s
{
    const dae_attrinfo* attrs;
    const dae_eleminfo* elems;
    dae_datatype datatype;
    int dataoffset;
    int mindata;
    int maxdata;
    int isunion;
    size_t numattrs;
    size_t numelems;
    size_t size;
};

struct dae_schemainfo_s
{
    dae_structinfo* structs;
    size_t numstructs;
};

struct dae_datalist_s
{
    void* values;
    size_t size;
};

struct dae_elemlist_s
{
    dae_typebase dae_base;
    void* values;
    size_t size;
};

struct dae_parseelement_s
{
    const dae_structinfo* def;
    unsigned char* buf;
};

struct dae_parserinternal_s
{
    unsigned char* out;
    dae_parseelement* stack;
    char* chardata;
    size_t stacksize;
    size_t chardatasize;
    size_t stackcapacity;
    size_t chardatacapacity;
};

//****************************************************************************
static const dae_schemainfo* dae_construct_schema();

static size_t dae_count_booldata(
    const char* chardata);

static size_t dae_count_floatdata(
    const char* chardata);

static size_t dae_count_intdata(
    const char* chardata);

static size_t dae_count_stringdata(
    const char* chardata);

static const dae_attrinfo* dae_FindattrDef(
    const dae_structinfo* def,
    dae_datatype datatype,
    const char* name);

static const dae_structinfo* dae_get_struct_def(
    dae_typeid type);

static void dae_parse_attr(
    const dae_structinfo* def,
    const char** attr,
    unsigned char* bufout);

static size_t dae_parse_booldata(
    const char* chardata,
    int* bout,
    size_t maxb);

static size_t dae_parse_floatdata(
    const char* chardata,
    float* fout,
    size_t maxf);

static size_t dae_parse_intdata(
    const char* chardata,
    int* iout,
    size_t maxi);

static size_t dae_parse_stringdata(
    const char* chardata,
    char** sout,
    size_t maxs);

static char* dae_strdup(
    const char* s);

//****************************************************************************
/*GENC_BGN*/
static const dae_schemainfo* dae_construct_schema()
{
    return NULL;
}
/*GENC_END*/

//****************************************************************************
static size_t dae_count_booldata(
    const char* chardata)
{
    const char* cd = chardata;
    size_t numb = 0;
    while(*cd != '\0')
    {
        const char* bgn;
        const char* end;
        while(*cd!='\0' && isspace(*cd)) { ++cd; }
        bgn = cd;
        while(*cd!='\0' && !isspace(*cd)){ ++cd; }
        end = cd;
        if(bgn != end)
        {
            ++numb;
        }
    }
    return numb;
}

//****************************************************************************
static size_t dae_count_floatdata(
    const char* chardata)
{
    const char* cd = chardata;
    size_t numf = 0;
    while(*cd != '\0')
    {
        float f = 0.0f;
        while(*cd!='\0' && isspace(*cd)) { ++cd; }
        if(sscanf(cd,"%f",&f) == 1) { ++numf; }
        while(*cd!='\0' && !isspace(*cd)){ ++cd; }
    }
    return numf;
}

//****************************************************************************
static size_t dae_count_intdata(
    const char* chardata)
{
    const char* cd = chardata;
    size_t numi = 0;
    while(*cd != '\0')
    {
        int i = 0;
        while(*cd!='\0' && isspace(*cd)) { ++cd; }
        if(sscanf(cd,"%d",&i) == 1) { ++numi; }
        while(*cd!='\0' && !isspace(*cd)){ ++cd; }
    }
    return numi;
}

//****************************************************************************
static size_t dae_count_stringdata(
    const char* chardata)
{
    const char* cd = chardata;
    size_t nums = 0;
    while(*cd != '\0')
    {
        const char* bgn;
        const char* end;
        while(*cd!='\0' && isspace(*cd)) { ++cd; }
        bgn = cd;
        while(*cd!='\0' && !isspace(*cd)){ ++cd; }
        end = cd;
        if(bgn != end)
        {
            ++nums;
        }
    }
    return nums;
}

//****************************************************************************
static const dae_attrinfo* dae_FindattrDef(
    const dae_structinfo* def,
    dae_datatype datatype,
    const char* name)
{
    const dae_attrinfo* result = NULL;
    const dae_attrinfo* attr = def->attrs;
    const dae_attrinfo* attrend = attr + def->numattrs;
    while(attr != attrend)
    {
        if(attr->datatype == datatype || datatype == dae_DATA_NONE)
        {
            if(!strcmp(attr->name, name))
            {
                result = attr;
                break;
            }
        }
        ++attr;
    }
    return result;
}

//****************************************************************************
static const dae_structinfo* dae_get_struct_def(dae_typeid type)
{
    static const dae_schemainfo* schema = NULL;
    if(schema == NULL)
    {
        schema = dae_construct_schema();
    }
    if(((size_t) type) >= schema->numstructs)
    {
        // invalid type!!
        assert(0);
        { char ch = *((char*) 0); ++ch; } // attempt to crash
        exit(-1);
    }
    return schema->structs + type;
}


//****************************************************************************
static void dae_parse_attr(
    const dae_structinfo* def,
    const char** attr,
    unsigned char* bufout)
{
    while(*attr != NULL)
    {
        const dae_attrinfo* attrdef = def->attrs;
        const dae_attrinfo* attrdefend = attrdef + def->numattrs;
        while(attrdef != attrdefend)
        {
            if(!strcmp(attrdef->name, *attr))
            {
                dae_data* d = (dae_data*) (bufout + attrdef->offset);
                const char* s = attr[1];
                switch(attrdef->datatype)
                {
                case dae_DATA_NONE:
                    assert(0);
                    break;
                case dae_DATA_BOOL:
                    d->bool_val = *s=='T' || *s == 't' || *s == '1';
                    break;
                case dae_DATA_INT:
                    sscanf(s, "%d", &d->int_val);
                    break;
                case dae_DATA_FLOAT:
                    sscanf(s, "%f", &d->float_val);
                    break;
                case dae_DATA_HEX:
                    assert(0);
                    break;
                case dae_DATA_STRING:
                    if(d->string_val != attrdef->defaultval.string_val)
                    {
                        free(d->string_val);
                    }
                    d->string_val = dae_strdup(s);
                    break;
                case dae_DATA_UINT:
                    sscanf(s, "%u", &d->uint_val);
                    break;
                case dae_DATA_XML:
                    assert(0);
                    break;
                }
                break;
            }
            ++attrdef;
        }
        attr += 2;
    }
}

//****************************************************************************
static size_t dae_parse_booldata(
    const char* chardata,
    int* bout,
    size_t maxb)
{
    const char* cd = chardata;
    int* pb = bout;
    int* pbend = pb + maxb;
    while(*cd != '\0')
    {
        const char* bgn;
        const char* end;
        while(*cd!='\0' && isspace(*cd)) { ++cd; }
        bgn = cd;
        while(*cd!='\0' && !isspace(*cd)){ ++cd; }
        end = cd;
        if(bgn != end)
        {
            *pb = *cd=='t' || *cd=='T' || *cd=='1';
            ++pb;
            if(pb == pbend) { break; }
        }
    }
    return (size_t) (pb - bout);
}

//****************************************************************************
static size_t dae_parse_floatdata(
    const char* chardata,
    float* fout,
    size_t maxf)
{
    const char* cd = chardata;
    float* pf = fout;
    float* pfend = pf + maxf;
    while(*cd != '\0')
    {
        while(*cd!='\0' && isspace(*cd)) { ++cd; }
        *pf = 0.0f;
        if(sscanf(cd, "%f", pf) == 1)
        {
            ++pf;
            if(pf == pfend) { break; }
        }
        while(*cd!='\0' && !isspace(*cd)){ ++cd; }
    }
    return (size_t) (pf - fout);
}

//****************************************************************************
static size_t dae_parse_intdata(
    const char* chardata,
    int* iout,
    size_t maxi)
{
    const char* cd = chardata;
    int* pi = iout;
    int* piend = pi + maxi;
    while(*cd != '\0')
    {
        while(*cd!='\0' && isspace(*cd)) {++cd;}
        *pi = 0;
        if(sscanf(cd, "%d", pi) == 1)
        {
            ++pi;
            if(pi == piend) { break; }
        }
        while(*cd!='\0' && !isspace(*cd)){++cd;}
    }
    return (size_t) (pi - iout);
}

//****************************************************************************
static size_t dae_parse_stringdata(
    const char* chardata,
    char** sout,
    size_t maxs)
{
    const char* cd = chardata;
    char** ps = sout;
    char** psend = ps + maxs;
    while(*cd != '\0')
    {
        const char* bgn;
        const char* end;
        while(*cd!='\0' && isspace(*cd)) { ++cd; }
        bgn = cd;
        while(*cd!='\0' && !isspace(*cd)){ ++cd; }
        end = cd;
        if(bgn != end)
        {
            size_t len = (size_t) (end - bgn);
            char* s = (char*) realloc(NULL, sizeof(*s)*(len+1));
            memcpy(s, bgn, sizeof(*s)*len);
            s[len] = '\0';
            *ps = s;
            ++ps;
            if(ps == psend) { break; }
        }
    }
    return (size_t) (ps - sout);
}

//****************************************************************************
static char* dae_strdup(
    const char* s)
{
    char* dup = NULL;
    if(s != NULL && *s != '\0')
    {
        size_t size = strlen(s) + 1;
        dup = (char*) realloc(NULL, sizeof(*dup) * size);
        memcpy(dup, s, sizeof(*dup) * size);
    }
    return dup;
}

//****************************************************************************
size_t dae_add(
    dae_typebase* plist,
    const dae_typebase* pval)
{
    const dae_structinfo* listdef = dae_get_struct_def(plist->type);
    dae_elemlist* list = (dae_elemlist*) plist;
    unsigned char* buf = (unsigned char*) list->values;
    size_t n = list->size;
    size_t cap = (n + 15) & (~15);
    size_t ncap = (n + 16) & (~15);
    if(cap < ncap)
    {
        buf = (unsigned char*) realloc(buf, listdef->size * ncap);
        list->values = buf;
    }
    buf += listdef->size * n;
    if(pval != NULL)
    {
        const dae_structinfo* valdef = dae_get_struct_def(pval->type);
        if(valdef->size <= listdef->size)
        {
            memcpy(buf, pval, valdef->size);
        }
        else
        {
            assert(0);
        }
    }
    else
    {
        dae_construct(plist->type, (dae_typebase*) buf);
    }
    ++list->size;
    return n;
}

//****************************************************************************
void dae_construct(
    dae_typeid type,
    dae_typebase* pout)
{
    unsigned char* buf = (unsigned char*) pout;
    const dae_structinfo* def = dae_get_struct_def(type);

    if(def->isunion)
    {
        // special case for unions
        assert(def->numattrs == 0); // union should have any attrs
        assert(def->datatype == dae_DATA_NONE); // union should have any data
        // clear the entire structure
        memset(buf, 0, def->size);
        pout->type = dae_ID_INVALID;
    }
    else
    {
        const dae_attrinfo* attr = def->attrs;
        const dae_attrinfo* attrend = attr + def->numattrs;
        const dae_eleminfo* elem = def->elems;
        const dae_eleminfo* elemend = elem + def->numelems;

        // set the typeid on the base struct
        pout->type = type;
        // initialize attributes to their default values
        while(attr != attrend)
        {
            dae_data* d = (dae_data*) (buf + attr->offset);
            // note: use switch as data union size might not match attr size
            switch(attr->datatype)
            {
            case dae_DATA_NONE:
                assert(0);
                break;
            case dae_DATA_BOOL:
                d->bool_val = attr->defaultval.bool_val;
                break;
            case dae_DATA_INT:
                d->int_val = attr->defaultval.int_val;
                break;
            case dae_DATA_FLOAT:
                d->float_val = attr->defaultval.float_val;
                break;
            case dae_DATA_HEX:
                assert(0);
                break;
            case dae_DATA_STRING:
                d->string_val = attr->defaultval.string_val;
                break;
            case dae_DATA_UINT:
                d->uint_val = attr->defaultval.uint_val;
                break;
            case dae_DATA_XML:
                assert(0);
                break;
            }
            ++attr;
        }
        // initialize elements
        while(elem != elemend)
        {
            switch(elem->restrict)
            {
            case dae_RESTRICT_CHOICE:
                assert(0);
                break;
            case dae_RESTRICT_LIST:
                // initialize list elements to empty
                {
                    dae_listbase* list = (dae_listbase*) (buf + elem->offset);
                    dae_construct_list(elem->dae_type, list);
                }
                break;
            case dae_RESTRICT_OPTIONAL:
                // set optional element pointers to NULL
                *((dae_typebase**) (buf + elem->offset)) = NULL;
                break;
            case dae_RESTRICT_REQUIRED:
                // construct required element members
                dae_construct(elem->dae_type, (dae_typebase*) (buf+elem->offset));
                break;
            default:
                assert(0);
                break;
            }
            ++elem;
        }
        // initialize data
        if(def->maxdata != 1 && def->mindata != def->maxdata)
        {
            // data list
            dae_datalist* list = (dae_datalist*) (buf + def->dataoffset);
            list->values = NULL;
            list->size = 0;
        }
        else if(def->datatype != dae_DATA_NONE)
        {
            // single data or fixed data size
            dae_data* d = (dae_data*) (buf + def->dataoffset);
            switch(def->datatype)
            {
            case dae_DATA_NONE:
                break;
            case dae_DATA_BOOL:
                memset(&d->bool_val, 0, sizeof(d->bool_val)*def->maxdata);
                break;
            case dae_DATA_INT:
                memset(&d->int_val, 0, sizeof(d->int_val)*def->maxdata);
                break;
            case dae_DATA_FLOAT:
                memset(&d->float_val, 0, sizeof(d->float_val)*def->maxdata);
                break;
            case dae_DATA_HEX:
                assert(0);
                break;
            case dae_DATA_STRING:
                memset(&d->string_val, 0, sizeof(d->string_val)*def->maxdata);
                break;
            case dae_DATA_UINT:
                memset(&d->uint_val, 0, sizeof(d->uint_val)*def->maxdata);
                break;
            case dae_DATA_XML:
                memset(&d->xml_val, 0, sizeof(d->xml_val)*def->maxdata);
                break;
            }
        }
    }
}

//****************************************************************************
void dae_construct_list(
    dae_typeid type,
    dae_listbase* pout)
{
    dae_elemlist* list = (dae_elemlist*) (pout);
    list->dae_base.type = type;
    list->values = NULL;
    list->size = 0;
}

//****************************************************************************
dae_parser dae_create_parser(
    dae_COLLADA* target)
{
    dae_parserinternal* pi;
    pi = (dae_parserinternal*) realloc(NULL, sizeof(*pi));
    memset(pi, 0, sizeof(*pi));
    pi->out = (unsigned char*) target;
    return pi;
}

//****************************************************************************
void dae_destroy_parser(
    dae_parser p)
{
    dae_parserinternal* pi = (dae_parserinternal*) p;
    free(pi->chardata);
    free(pi->stack);
    free(pi);
}

//****************************************************************************
void dae_destruct(
    dae_typebase* pval)
{
    unsigned char* buf = (unsigned char*) pval;
    const dae_structinfo* def = dae_get_struct_def(pval->type);
    const dae_attrinfo* attr = def->attrs;
    const dae_attrinfo* attrend = attr + def->numattrs;
    const dae_eleminfo* elem = def->elems;
    const dae_eleminfo* elemend = elem + def->numelems;
    while(attr != attrend)
    {
        if(attr->datatype == dae_DATA_STRING)
        {
            // if the attribute is a string, and is not the default, free it
            char* s = *((char**) (buf+attr->offset));
            if(s != NULL && s != attr->defaultval.string_val)
            {
                free(s);
            }
        }
        ++attr;
    }
    while(elem != elemend)
    {
        switch(elem->restrict)
        {
        case dae_RESTRICT_CHOICE:
            assert(0); // shouldn't be calling destruct on a union
            break;
        case dae_RESTRICT_LIST:
            // destruct list elements and free buffer
            dae_destruct_list((dae_listbase*) (buf + elem->offset));
            break;
        case dae_RESTRICT_OPTIONAL:
            // destruct and free optional element members
            {
                dae_typebase* elemval = *((dae_typebase**)(buf+elem->offset));
                if(elemval!= NULL)
                {
                    dae_destruct(elemval);
                    free(elemval);
                }
            }
            break;
        case dae_RESTRICT_REQUIRED:
            // destruct required element members
            dae_destruct((dae_typebase*) (buf+elem->offset));
            break;
        }
        ++elem;
    }
    if(def->datatype == dae_DATA_STRING || def->datatype == dae_DATA_XML)
    {
        if(def->maxdata == 1)
        {
            // if the data member is a single string, free it
            free(*((char**) (buf+def->dataoffset)));
        }
        else
        {
            // if the data member is a list of strings, free them
            dae_datalist* list = (dae_datalist*) (buf + def->dataoffset);
            char** str = (char**) list->values;
            char** strend = str + list->size;
            while(str != strend)
            {
                free(*str);
                ++str;
            }
            free(list->values);
        }
    }
    else if(def->datatype != dae_DATA_NONE)
    {
        // if the data member is a variably sized list, free it
        if(def->maxdata != 1 && def->mindata != def->maxdata)
        {
            free(*((void**) (buf+def->dataoffset)));
        }
    }
}

//****************************************************************************
void dae_destruct_list(
    dae_listbase* plist)
{
    dae_elemlist* list = (dae_elemlist*) (plist);
    const dae_structinfo* def = dae_get_struct_def(list->dae_base.type);
    unsigned char* buf = (unsigned char*) list->values;
    unsigned char* bufend = buf + (def->size * list->size);
    while(buf != bufend)
    {
        dae_destruct((dae_typebase*) buf);
        buf += def->size;
    }
    free(list->values);
}

//****************************************************************************
dae_typebase* dae_find_in_list(
    dae_listbase* plist,
    dae_typeid type,
    const char* attname,
    const char* attval)
{
    dae_typebase* result = NULL;
    const dae_structinfo* listdef = dae_get_struct_def(plist->type);
    dae_elemlist* list = (dae_elemlist*) plist;
    unsigned char* buf = (unsigned char*) list->values;
    unsigned char* bufend = buf + (listdef->size * list->size);
    const dae_attrinfo* curattr = NULL;
    dae_typeid curtype = dae_ID_INVALID;
    while(buf != bufend)
    {
        dae_typebase* base = (dae_typebase*) buf;
        if(base->type != curtype)
        {
            const dae_structinfo* itemdef = dae_get_struct_def(base->type);
            const dae_attrinfo* attr = itemdef->attrs;
            const dae_attrinfo* attrend = attr + itemdef->numattrs;
            curattr = NULL;
            curtype = base->type;
            if(base->type == type || type == dae_ID_INVALID)
            {
                while(attr != attrend)
                {
                    if(attr->datatype == dae_DATA_STRING)
                    {
                        if(!strcmp(attr->name, attname))
                        {
                            curattr = attr;
                            break;
                        }
                    }
                    ++attr;
                }
            }
        }
        if(curattr != NULL)
        {
            const char* itemval = *((const char**) (buf+curattr->offset));
            if(itemval != NULL && !strcmp(itemval, attval))
            {
                result = (dae_typebase*) buf;
                break;
            }
        }
        buf += listdef->size;
    }
    return result;
}

//****************************************************************************
size_t dae_search_sid(
    dae_typebase* searchroot,
    const char* ref,
    void** targetsout,
    dae_targettype* typesout,
    size_t maxtargets)
{
    const char* sep = "/.(";
    size_t result = 0;
    unsigned char* buf = (unsigned char*) searchroot;
    const dae_structinfo* def = dae_get_struct_def(searchroot->type);
    int isrootless = 0;
    int ismatch = 0;
    if(*ref == '.')
    {
        // relative to the current element
        ismatch = 1;
        ++ref;
    }
    else if(*ref == '/')
    {
        // a child element
        ++ref;
    }
    else
    {
        // the root of the path has not been found yet
        assert(strpbrk(ref, sep) != ref);
        isrootless = 1;
    }
    if(!ismatch)
    {
        // check to see if this is the current item in the path
        const char* attnames[] = { "sid", "id" };
        int i;
        for(i = 0; i < sizeof(attnames)/sizeof(*attnames); ++i)
        {
            const dae_attrinfo* attr;
            attr = dae_FindattrDef(def,dae_DATA_STRING,attnames[i]);
            if(attr != NULL)
            {
                const char* end = strpbrk(ref, sep);
                size_t len = (end != NULL) ? (size_t) (end-ref) : strlen(ref);
                const char* attval = *((char**) (buf + attr->offset));
                if(attval != NULL)
                {
                    ismatch = !memcmp(attval,ref,len) && attval[len]=='\0';
                    if(ismatch)
                    {
                        isrootless = 0;
                        ref += len;
                        break;
                    }
                }
            }
        }
    }
    if(ismatch || isrootless)
    {
        // if this item was in the path, or the path root has not been found
        if(*ref == '.')
        {
            // referencing a component of the current element
            ++ref;
            if(maxtargets > 0)
            {
                if(def->datatype == dae_DATA_FLOAT)
                {
                    int i = -1;
                    switch(*ref)
                    {
                    case 'A':
                        if(!strcmp(ref, "ANGLE") || ref[1] == '\0'){ i = 3; }
                        break;
                    case 'B':
                        if(ref[1] == '\0') { i = 2; }
                        break;
                    case 'G':
                        if(ref[1] == '\0') { i = 1; }
                        break;
                    case 'P':
                        if(ref[1] == '\0') { i = 2; }
                        break;
                    case 'Q':
                        if(ref[1] == '\0') { i = 3; }
                        break;
                    case 'R':
                        if(ref[1] == '\0') { i = 0; }
                        break;
                    case 'S':
                        if(ref[1] == '\0') { i = 0; }
                        break;
                    case 'T':
                        // NOTE: not handling 'TIME'
                        if(ref[1] == '\0') { i = 1; }
                        break;
                    case 'U':
                        if(ref[1] == '\0') { i = 0; }
                        break;
                    case 'V':
                        if(ref[1] == '\0') { i = 1; }
                        break;
                    case 'W':
                        if(ref[1] == '\0') { i = 3; }
                        break;
                    case 'X':
                        if(ref[1] == '\0') { i = 0; }
                        break;
                    case 'Y':
                        if(ref[1] == '\0') { i = 1; }
                        break;
                    case 'Z':
                        if(ref[1] == '\0') { i = 2; }
                        break;
                    }
                    if(i >= 0)
                    {
                        float* data = NULL;
                        if(i < def->maxdata)
                        {
                            data = (float*) (buf + def->dataoffset);
                        }
                        else if(def->maxdata < 0)
                        {
                            data = *((float**) (buf + def->dataoffset));
                        }
                        if(data != NULL)
                        {
                            *targetsout = data + i;
                            *typesout = dae_TARGET_FLOAT;
                            ++result;
                        }
                    }
                }
            }
        }
        else if(*ref == '(')
        {
            // referencing a component of the current element by index
            int i = 0;
            ++ref;
            if(maxtargets > 0 && sscanf(ref,"%d",&i) == 1)
            {
                if(def->datatype == dae_DATA_FLOAT)
                {
                    float* data = NULL;
                    if(i < def->maxdata)
                    {
                        data = (float*) (buf + def->dataoffset);
                    }
                    else if(def->maxdata < 0)
                    {
                        data = *((float**) (buf + def->dataoffset));
                    }
                    if(data != NULL)
                    {
                        *targetsout = data + i;
                        *typesout = dae_TARGET_FLOAT;
                        ++result;
                    }
                }
            }
        }
        else if(*ref == '/' || isrootless)
        {
            // traverse children
            const dae_eleminfo* elem = def->elems;
            const dae_eleminfo* elemend = elem + def->numelems;
            while(result == 0 && elem != elemend)
            {
                switch(elem->restrict)
                {
                case dae_RESTRICT_CHOICE:
                    {
                        result = dae_search_sid(
                            (dae_typebase*) (buf+elem->offset),
                            ref,
                            targetsout,
                            typesout,
                            maxtargets);
                    }
                    break;
                case dae_RESTRICT_LIST:
                    {
                        dae_elemlist* ellist;
                        const dae_structinfo* eldef;
                        unsigned char* dbuf;
                        unsigned char* elend;
                        ellist = (dae_elemlist*) (buf+elem->offset);
                        eldef = dae_get_struct_def(ellist->dae_base.type);
                        dbuf = (unsigned char*) ellist->values;
                        elend = dbuf + eldef->size*ellist->size;
                        while(result == 0 && dbuf != elend)
                        {
                            result = dae_search_sid(
                                (dae_typebase*) dbuf,
                                ref,
                                targetsout,
                                typesout,
                                maxtargets);
                            dbuf += eldef->size;
                        }
                    }
                    break;
                case dae_RESTRICT_OPTIONAL:
                    {
                        dae_typebase* elbase;
                        elbase = *((dae_typebase**) (buf+elem->offset));
                        if(elbase != NULL)
                        {
                            result = dae_search_sid(
                                elbase,
                                ref,
                                targetsout,
                                typesout,
                                maxtargets);
                        }
                    }
                    break;
                case dae_RESTRICT_REQUIRED:
                    {
                        result = dae_search_sid(
                            (dae_typebase*) (buf+elem->offset),
                            ref,
                            targetsout,
                            typesout,
                            maxtargets);
                    }
                    break;
                }
                ++elem;
            }
        }
    }
    if(ismatch && result < maxtargets)
    {
        // move the list over
        size_t i;
        for(i = result; i > 0; --i)
        {
            targetsout[i] = targetsout[i - 1];
            typesout[i] = typesout[i - 1];
        }
        // add this element
        targetsout[0] = searchroot;
        typesout[0] = dae_TARGET_ELEM;
        ++result;
    }
    return result;
}

//****************************************************************************
dae_typebase* dae_search_uri(
    dae_typebase* searchroot,
    dae_typeid type,
    const char* uri)
{
    dae_typebase* result = NULL;
    const dae_structinfo* def = dae_get_struct_def(searchroot->type);
    const dae_eleminfo* elem = def->elems;
    const dae_eleminfo* elemend = elem + def->numelems;
    unsigned char* buf = (unsigned char*) searchroot;
    assert(*uri == '#');
    if(searchroot->type == type)
    {
        const dae_attrinfo* attr = dae_FindattrDef(def, dae_DATA_STRING, "id");
        assert(attr != NULL);
        if(attr != NULL)
        {
            const char* key = uri;
            if(*key == '#')
            {
                ++key;
            }
            if(!strcmp(key, *((char**) (buf + attr->offset))))
            {
                result = searchroot;
            }
        }
    }
    while(result == NULL && elem != elemend)
    {
        switch(elem->restrict)
        {
        case dae_RESTRICT_CHOICE:
            {
                dae_typebase* elbase = (dae_typebase*) (buf+elem->offset);
                result = dae_search_uri(elbase, type, uri);
            }
            break;
        case dae_RESTRICT_LIST:
            {
                dae_elemlist* ellist = (dae_elemlist*) (buf+elem->offset);
                dae_typeid eltype = ellist->dae_base.type;
                const dae_structinfo* eldef = dae_get_struct_def(eltype);
                unsigned char* dbuf = (unsigned char*) ellist->values;
                unsigned char* elend = dbuf + eldef->size*ellist->size;
                while(result == NULL && dbuf != elend)
                {
                    result = dae_search_uri((dae_typebase*) dbuf, type, uri);
                    dbuf += eldef->size;
                }
            }
            break;
        case dae_RESTRICT_OPTIONAL:
            {
                dae_typebase* elbase = *((dae_typebase**) (buf+elem->offset));
                if(elbase != NULL)
                {
                    result = dae_search_uri(elbase, type, uri);
                }
            }
            break;
        case dae_RESTRICT_REQUIRED:
            {
                dae_typebase* elbase = (dae_typebase*) (buf+elem->offset);
                result = dae_search_uri(elbase, type, uri);
            }
            break;
        }
        ++elem;
    }
    return result;
}

//****************************************************************************
void dae_xml_startelement(
    dae_parser p,
    const char *el,
    const char **attr)
{
    dae_parserinternal* pi = (dae_parserinternal*) p;
    // push element onto stack
    dae_parseelement* child = pi->stack;
    size_t n = pi->stacksize;
    size_t cap = pi->stackcapacity;
    size_t ncap = (n + 16) & (~15);
    if(ncap > cap)
    {
        child = (dae_parseelement*) realloc(child, sizeof(*child)*ncap);
        pi->stack = child;
        pi->stackcapacity = ncap;
    }
    ++pi->stacksize;
    child += n;
    memset(child, 0, sizeof(*child));
    if(n > 0)
    {
        dae_parseelement* parent = pi->stack + (n-1);
        if(parent->def != NULL && parent->buf != NULL)
        {
            const dae_eleminfo* elem = parent->def->elems;
            const dae_eleminfo* elemend = elem + parent->def->numelems;
            while(elem != elemend && child->buf == NULL)
            {
                unsigned char* dbuf = parent->buf + elem->offset;
                switch(elem->restrict)
                {
                case dae_RESTRICT_CHOICE:
                    assert(0); // not supported (should be in list)
                    break;
                case dae_RESTRICT_LIST:
                    // add new item to list
                    {
                        dae_elemlist* list = (dae_elemlist*) dbuf;
                        const dae_structinfo* ldef;
                        ldef = dae_get_struct_def(list->dae_base.type);
                        if(ldef->isunion)
                        {
                            // if it's a choice list, check the choice names
                            const dae_eleminfo* celem = ldef->elems;
                            const dae_eleminfo* celemend=celem+ldef->numelems;
                            while(celem != celemend)
                            {
                                if(!strcmp(el, celem->name))
                                {
                                    size_t n = dae_add(&list->dae_base, NULL);
                                    const dae_structinfo* cdef;
                                    unsigned char* cbuf;
                                    dae_typebase* cval;
                                    cdef = dae_get_struct_def(celem->dae_type);
                                    cbuf = (unsigned char*) list->values;
                                    cbuf += n * ldef->size;
                                    cval = (dae_typebase*) cbuf;
                                    cval->type = celem->dae_type;
                                    child->buf = cbuf;
                                    child->def = cdef;
                                    break;
                                }
                                ++celem;
                            }
                        }
                        else if(!strcmp(el, elem->name))
                        {
                            size_t n = dae_add(&list->dae_base, NULL);
                            unsigned char* lval=(unsigned char*)list->values;
                            child->buf = lval + (n * ldef->size);
                            child->def = dae_get_struct_def(elem->dae_type);
                        }
                    }
                    break;
                case dae_RESTRICT_OPTIONAL:
                    // create and construct new optional item
                    if(!strcmp(el, elem->name))
                    {
                        dae_typebase** pelval = (dae_typebase**) dbuf;
                        if(*pelval == NULL)
                        {
                            const dae_structinfo* eldef;
                            void* p;
                            eldef = dae_get_struct_def(elem->dae_type);
                            p = realloc(NULL, eldef->size);
                            *pelval = (dae_typebase*) p;
                            dae_construct(elem->dae_type, *pelval);
                            child->buf = (unsigned char*) p;
                            child->def = dae_get_struct_def(elem->dae_type);
                        }
                    }
                    break;
                case dae_RESTRICT_REQUIRED:
                    // use required member
                    if(!strcmp(el, elem->name))
                    {
                        child->buf = dbuf;
                        child->def = dae_get_struct_def(elem->dae_type);
                    }
                    break;
                }
                ++elem;
            }
        }
    }
    else if(!strcmp(el, "COLLADA"))
    {
        child->def = dae_get_struct_def(dae_ID_COLLADA);
        child->buf = pi->out;
    }
    if(child->def != NULL && child->buf != NULL)
    {
        dae_parse_attr(child->def, attr, child->buf);
    }
}

//****************************************************************************
void dae_xml_chardata(
    dae_parser p,
    const char *s,
    int len)
{
    dae_parserinternal* pi = (dae_parserinternal*) p;
    size_t ncap = (pi->chardatasize + len + 1024) & (~1023);
    char* cd = pi->chardata;
    if(ncap > pi->chardatacapacity)
    {
        cd = (char*) realloc(cd, sizeof(*cd)*ncap);
        pi->chardata = cd;
        pi->chardatacapacity = ncap;
    }
    memcpy(cd + pi->chardatasize, s, len);
    pi->chardatasize += len;
    cd[pi->chardatasize] = '\0';
}

//****************************************************************************
void dae_xml_endelement(
    dae_parser p,
    const char *el)
{
    dae_parserinternal* pi = (dae_parserinternal*) p;
    dae_parseelement* e = pi->stack + (pi->stacksize-1);
    const dae_structinfo* def = e->def;
    char* cd = pi->chardata;
    size_t cdsize = 0;
    if(e->def != NULL && e->def->datatype != dae_DATA_NONE)
    {
        cdsize = pi->chardatasize;
        while(cdsize > 0 && isspace(*cd))
        {
            ++cd;
            --cdsize;
        }
        while(cdsize > 0 && isspace(cd[cdsize-1]))
        {
            --cdsize;
        }
    }
    if(cdsize > 0)
    {
        unsigned char* dbuf = e->buf + def->dataoffset;
        if(def->maxdata != 1 && def->mindata != def->maxdata)
        {
            // data list
            dae_datalist* list = (dae_datalist*) dbuf;
            switch(def->datatype)
            {
            case dae_DATA_NONE:
                assert(0);
                break;
            case dae_DATA_BOOL:
                {
                    size_t n = dae_count_booldata(cd);
                    int* d = (int*) realloc(list->values, sizeof(*d)*n);
                    dae_parse_booldata(cd, d, n);
                    list->values = d;
                    list->size = n;
                }
                break;
            case dae_DATA_INT:
                {
                    size_t n = dae_count_intdata(cd);
                    int* d = (int*) realloc(list->values, sizeof(*d)*n);
                    dae_parse_intdata(cd, d, n);
                    list->values = d;
                    list->size = n;
                }
                break;
            case dae_DATA_FLOAT:
                {
                    size_t n = dae_count_floatdata(cd);
                    float* d = (float*) realloc(list->values, sizeof(*d)*n);
                    dae_parse_floatdata(cd, d, n);
                    list->values = d;
                    list->size = n;
                }
                break;
            case dae_DATA_HEX:
                // TODO:
                assert(0);
                break;
            case dae_DATA_STRING:
                {
                    size_t n = dae_count_stringdata(cd);
                    char** d = (char**) realloc(list->values, sizeof(*d)*n);
                    dae_parse_stringdata(cd, d, n);
                    list->values = d;
                    list->size = n;
                }
                break;
            case dae_DATA_UINT:
                {
                    size_t n = dae_count_intdata(cd);
                    int* d = (int*) realloc(list->values, sizeof(*d)*n);
                    dae_parse_intdata(cd, d, n);
                    list->values = d;
                    list->size = n;
                }
                break;
            case dae_DATA_XML:
                assert(0);
                break;
            }
        }
        else if(def->datatype==dae_DATA_STRING || def->datatype==dae_DATA_XML)
        {
            // single string data
            char** pstr = (char**) dbuf;
            *pstr = dae_strdup(cd);
        }
        else
        {
            // single data or fixed data size
            switch(def->datatype)
            {
            case dae_DATA_NONE:
                assert(0);
                break;
            case dae_DATA_BOOL:
                dae_parse_booldata(cd, (int*) dbuf, def->maxdata);
                break;
            case dae_DATA_INT:
                dae_parse_intdata(cd, (int*) dbuf, def->maxdata);
                break;
            case dae_DATA_FLOAT:
                dae_parse_floatdata(cd, (float*) dbuf, def->maxdata);
                break;
            case dae_DATA_HEX:
                assert(0);
                break;
            case dae_DATA_STRING:
                assert(0);
                break;
            case dae_DATA_UINT:
                dae_parse_intdata(cd, (int*) dbuf, def->maxdata);
                break;
            case dae_DATA_XML:
                assert(0);
                break;
            }
        }
    }
    pi->chardatasize = 0;
    --pi->stacksize;
}

