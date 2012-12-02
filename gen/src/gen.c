#include "gen.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

//****************************************************************************
static void gen_str_alphanumeric(
    char* dst,
    const char* src,
    size_t n)
{
    char* end = dst + n - 1;
    while(dst != end && *src != '\0')
    {
        char ch = *src;
        *dst = (isalnum(ch)) ? ch : '_';
        ++src;
        ++dst;
    }
    *dst = '\0';
}

//****************************************************************************
static void gen_str_upper(
    char* dst,
    const char* src,
    size_t n)
{
    char* end = dst + n - 1;
    while(dst != end && *src != '\0')
    {
        *dst = toupper(*src);
        ++src;
        ++dst;
    }
    assert(dst != end);
    *dst = '\0';
}

//****************************************************************************
static void gen_cname_id(
    char* dst,
    const char* src,
    size_t n)
{
    if(n > 7)
    {
        const char* colon = strchr(src, ':');
        if(colon != NULL)
        {
            // strip out namespaces
            src = colon + 1;
        }
        strcpy(dst, "dae_ID_");
        gen_str_alphanumeric(dst + 7, src, n - 7);
        gen_str_upper(dst + 7, dst + 7, n - 7);
    }
    else if(n > 0)
    {
        *dst = '\0';
    }
}

//****************************************************************************
static void gen_cname_attrib(
    char* dst,
    const char* src,
    size_t n)
{
    if(n > 3)
    {
        strcpy(dst, "at_");
        gen_str_alphanumeric(dst + 3, src, n - 3);
    }
    else if(n > 0)
    {
        *dst = '\0';
    }
}

//****************************************************************************
static void gen_cname_elem(
    char* dst,
    const char* src,
    size_t n)
{
    if(n > 3)
    {
        strcpy(dst, "el_");
        gen_str_alphanumeric(dst + 3, src, n - 3);
    }
    else if(n > 0)
    {
        *dst = '\0';
    }
}

//****************************************************************************
static const char* gen_cname_nativeid(
    xsd_schema* schema,
    const char* src)
{
    const char* nativeid = NULL;
    while(src != NULL)
    {
        xsd_type* t;
        if(!strcmp(src, xsd_DATA_BOOL))
        {
            nativeid = "dae_NATIVE_BOOL32";
        }
        else if(!strcmp(src, xsd_DATA_HEX))
        {
            nativeid = "dae_NATIVE_HEX8";
        }
        else if(!strcmp(src, xsd_DATA_FLOAT))
        {
            nativeid = "dae_NATIVE_FLOAT";
        }
        else if(!strcmp(src, xsd_DATA_INT8))
        {
            nativeid = "dae_NATIVE_INT8";
        }
        else if(!strcmp(src, xsd_DATA_INT16))
        {
            nativeid = "dae_NATIVE_INT16";
        }
        else if(!strcmp(src, xsd_DATA_STRING))
        {
            nativeid = "dae_NATIVE_STRING";
        }
        else if(!strcmp(src, xsd_DATA_INT32))
        {
            nativeid = "dae_NATIVE_INT32";
        }
        else if(!strcmp(src, xsd_DATA_UINT8))
        {
            nativeid = "dae_NATIVE_UINT8";
        }
        else if(!strcmp(src, xsd_DATA_UINT16))
        {
            nativeid = "dae_NATIVE_UINT16";
        }
        else if(!strcmp(src, xsd_DATA_UINT32))
        {
            nativeid = "dae_NATIVE_UINT32";
        }
        if(nativeid != NULL)
        {
            break;
        }
        t = xsd_find_type(schema, src);
        src = t->base;
    }
    return nativeid;
}

static const char* gen_cname_native(
    const char* src)
{
    const char* cname = NULL;
    if(!strcmp(src, xsd_DATA_BOOL))
    {
        cname = "int";
    }
    else if(!strcmp(src, xsd_DATA_HEX))
    {
        cname = "unsigned char";
    }
    else if(!strcmp(src, xsd_DATA_FLOAT))
    {
        cname = "float";
    }
    else if(!strcmp(src, xsd_DATA_INT8))
    {
        cname = "char";
    }
    else if(!strcmp(src, xsd_DATA_INT16))
    {
        cname = "short";
    }
    else if(!strcmp(src, xsd_DATA_STRING))
    {
        cname = "char*";
    }
    else if(!strcmp(src, xsd_DATA_INT32))
    {
        cname = "int";
    }
    else if(!strcmp(src, xsd_DATA_UINT8))
    {
        cname = "unsigned char";
    }
    else if(!strcmp(src, xsd_DATA_UINT16))
    {
        cname = "unsigned short";
    }
    else if(!strcmp(src, xsd_DATA_UINT32))
    {
        cname = "unsigned int";
    }
    return cname;
}

//****************************************************************************
static void gen_cname_type(
    char* dst,
    const char* src,
    size_t n)
{
    const char* native = gen_cname_native(src);
    if(native != NULL)
    {
        strncpy(dst, native, n);
        dst[n - 1] = '\0';
    }
    else
    {
        if(n > 4)
        {
            const char* colon = strchr(src, ':');
            if(colon != NULL)
            {
                // strip out namespaces
                src = colon + 1;
            }
            strcpy(dst, "dae_");
            gen_str_alphanumeric(dst + 4, src, n - 4);
        }
        else if(n > 0)
        {
            *dst = '\0';
        }
    }
}

//****************************************************************************
static void gen_print_structmember(
    FILE* fp,
    xsd_type* t,
    const char* name,
    int min,
    int max,
    int useptr)
{
    char tcn[128];
    gen_cname_type(tcn, t->name, sizeof(tcn) - 1);
    if(useptr)
    {
        strcat(tcn, "*");
    }
    if((max == 1 || max == xsd_UNSET))
    {
        fprintf(fp,"    %s %s;\n", tcn, name);
    }
    else if(max == xsd_UNBOUNDED)
    {
        // variable sized list
        fprintf(fp,"    struct\n");
        fprintf(fp,"    {\n");
        fprintf(fp,"        %s* values;\n", tcn);
        fprintf(fp,"        size_t size;\n");
        fprintf(fp,"    } %s;\n", name);
    }
    else
    {
        // fixed size array
        fprintf(fp,"    %s %s[%i];\n", tcn, name, max);
    }
}

//****************************************************************************
void gen_print_typeids(
    FILE* fp,
    xsd_schema* schema)
{
    xsd_type** titr = schema->types;
    xsd_type** tend = titr + schema->numtypes;
    fprintf(fp,"enum dae_obj_typeid_e\n");
    fprintf(fp,"{\n");
    while(titr != tend)
    {
        xsd_type* t = *titr;
        xsd_type** previtr = schema->types;
        char idcn[128];
        gen_cname_id(idcn, t->name, sizeof(idcn));
        // check for duplicates
        while(previtr != titr)
        {
            xsd_type* prev = *previtr;
            char prevcn[128];
            gen_cname_id(prevcn, prev->name, sizeof(prevcn));
            if(strcmp(prevcn, idcn) == 0)
            {
                break;
            }
            ++previtr;
        }
        if(previtr == titr)
        {
            fprintf(fp,"    %s,\n", idcn);
        }
        ++titr;
    }
    fprintf(fp,"    dae_ID_INVALID = -1\n");
    fprintf(fp,"};\n");
}

//****************************************************************************
void gen_print_typedefs(
    FILE* fp,
    xsd_schema* schema)
{
    xsd_type** titr = schema->types;
    xsd_type** tend = titr + schema->numtypes;
    while(titr != tend)
    {
        xsd_type* t = *titr;
        if(gen_cname_native(t->name) == NULL)
        {
            // only generate typedefs for non-native types
            char tcn[128];
            gen_cname_type(tcn, t->name, sizeof(tcn));
            if(t->baserelation == xsd_BASE_RENAME)
            {
                char basecn[128];
                gen_cname_type(basecn, t->base, sizeof(basecn));
                fprintf(fp, "typedef %s %s;\n", basecn, tcn);
            }
            else if(t->hascomplex)
            {
                // if the type has complex content, generate a struct for it
                if(t->complex.numattribs > 0 || !t->anycontent)
                {
                    fprintf(fp,"typedef struct %s_s %s;\n", tcn, tcn);
                }
                else
                {
                    // if any child content is allowed, and there are no
                    // attribs, then there is nothing to define in a struct
                    // define as void*
                    fprintf(fp,"typedef void* %s;\n", tcn);
                }
            }
            else if(t->hassimple)
            {
                // if the type is simple, generate a primitive typedef for it
                char cdata[128];
                int max;
                gen_cname_type(cdata, t->simple.itemtype, sizeof(cdata));
                max=(t->simple.maxoccurs!=xsd_UNSET) ? t->simple.maxoccurs:1;
                assert(!t->anycontent);
                if(max == 1)
                {
                    // single component
                    fprintf(fp,"typedef %s %s;\n", cdata, tcn);
                }
                else if(max == xsd_UNBOUNDED)
                {
                    // list, need a struct
                    fprintf(fp,"typedef struct %s_s %s;\n", tcn, tcn);
                }
                else
                {
                    // array
                    fprintf(fp,"typedef %s %s[%i];\n", cdata, tcn, max);
                }
            }
            else
            {
                // if the type has no content, define as void*
                fprintf(fp,"typedef void* %s;\n", tcn);
            }
        }
        ++titr;
    }
}

//****************************************************************************
static int gen_is_struct(
    xsd_type* t)
{
    int isstruct = 0;
    if(t->hascomplex)
    {
        isstruct = t->complex.numattribs > 0 || t->complex.numelements > 0;
    }
    else if(t->hassimple)
    {
        isstruct = t->simple.maxoccurs == xsd_UNBOUNDED;
    }
    return isstruct;
}

//****************************************************************************
void gen_print_structs(
    FILE* fp,
    xsd_schema* schema)
{
    xsd_type** titr = schema->types;
    xsd_type** tend = titr + schema->numtypes;
    while(titr != tend)
    {
        xsd_type* t = *titr;
        int needstruct = 0;
        if(t->baserelation == xsd_BASE_RENAME)
        {
            // special case, renames get typedefed as base type
            needstruct = 0;
        }
        else
        {
            needstruct = gen_is_struct(t);
        }
        if(needstruct)
        {
            xsd_attrib** atitr = t->complex.attribs;
            xsd_attrib** atend = atitr + t->complex.numattribs;
            xsd_element** elitr = t->complex.elements;
            xsd_element** elend = elitr + t->complex.numelements;
            char tcn[128];
            assert(!t->isbuiltin);
            gen_cname_type(tcn, t->name, sizeof(tcn));
            fprintf(fp,"struct %s_s\n", tcn);
            fprintf(fp,"{\n");
            while(atitr != atend)
            {
                xsd_attrib* at = *atitr;
                xsd_type* tat = xsd_find_type(schema,at->type);
                const char* item = tat->simple.itemtype;
                int min = (at->required) ? 1 : 0;
                int max = 1;
                char atcn[128];
                gen_cname_attrib(atcn, at->name, sizeof(atcn));
                gen_print_structmember(fp, tat, atcn, min, max, 1);
                if (!tat->hassimple || gen_cname_nativeid(schema,item)==NULL)
                {
                    fprintf(fp,"#error unrecogonized attribute type\n");
                }
                ++atitr;
            }
            while(elitr != elend)
            {
                xsd_element* el = *elitr;
                if(el->type != NULL)
                {
                    xsd_element** inrelitr = t->complex.elements;
                    int isduplicate = 0;
                    // make sure there are no other elements with the same name
                    // only export duplicated elements once to the struct def
                    while(inrelitr != elitr)
                    {
                        xsd_element* inrel = *inrelitr;
                        if(!strcmp(el->name,inrel->name))
                        {
                            isduplicate = 1;
                            break;
                        }
                        ++inrelitr;
                    }
                    if(!isduplicate)
                    {
                        xsd_type* tel = xsd_find_type(schema,el->type);
                        int min = el->minoccurs;
                        int max = el->maxoccurs;
                        char elcn[128];
                        gen_cname_elem(elcn, el->name, sizeof(elcn));
                        gen_print_structmember(fp, tel, elcn, min, max, 1);
                    }
                }
                ++elitr;
            }
            if(t->hassimple)
            {
                xsd_type* titem = xsd_find_type(schema, t->simple.itemtype);
                int min = t->simple.minoccurs;
                int max = t->simple.maxoccurs;
                gen_print_structmember(fp, titem, "data", min, max, 0);
            }
            fprintf(fp,"};\n");
            fprintf(fp,"\n");
        }
        ++titr;
    }
}

//****************************************************************************
void gen_print_schema(
    FILE* fp,
    xsd_schema* schema)
{
    xsd_type** titr;
    xsd_type** tend;
    fprintf(fp,"static void dae_build_schema(\n");
    fprintf(fp,"    dae_obj_typedef** types_out,\n");
    fprintf(fp,"    unsigned* numtypes_out)\n");
    fprintf(fp,"{\n");
    fprintf(fp,"    static dae_obj_typedef s_types[%i];\n",
        (int) schema->numtypes);
    fprintf(fp,"    dae_obj_typedef* t = s_types;\n");
    titr = schema->types;
    tend = titr + schema->numtypes;
    while(titr != tend)
    {
        xsd_type* t = *titr;
        char idcn[128];
        char tcn[128];
        gen_cname_id(idcn, t->name, sizeof(idcn));
        gen_cname_type(tcn, t->name, sizeof(tcn));
        fprintf(fp,"    t->name = \"%s\";\n", t->name);
        fprintf(fp,"    t->size = sizeof(%s);\n", tcn);
        fprintf(fp,"    t->flags= 0");
        if(t->anycontent)
        {
            fprintf(fp," | dae_XSD_ANY");
        }
        fprintf(fp,";\n");
        fprintf(fp,"    t->objtypeid = %s;\n", idcn);
        if(t->hassimple)
        {
            const char* dtcid;
            int min;
            int max;
            // exported data type should always be a native
            dtcid = gen_cname_nativeid(schema, t->simple.itemtype);
            min = (t->simple.minoccurs!=xsd_UNSET) ? t->simple.minoccurs : 1;
            max = (t->simple.maxoccurs!=xsd_UNSET) ? t->simple.maxoccurs : 1;
            if(gen_is_struct(t))
            {
                fprintf(fp,"    t->dataoffset=offsetof(%s,data);\n",tcn);
            }
            else
            {
                fprintf(fp,"    t->dataoffset=0;\n");
            }
            fprintf(fp,"    t->datatypeid=%s;\n", dtcid);
            fprintf(fp,"    t->datamin=%i;\n", min);
            fprintf(fp,"    t->datamax=%i;\n", max);
        }
        else
        {
            fprintf(fp,"    t->datatypeid=dae_ID_INVALID;\n");
        }
        if(t->hascomplex && t->complex.numattribs > 0)
        {
            xsd_attrib** atitr = t->complex.attribs;
            xsd_attrib** atend = atitr + t->complex.numattribs;
            int numats = t->complex.numattribs;
            fprintf(fp,"    {\n");
            fprintf(fp,"        static dae_obj_memberdef attribs[%i];\n", numats);
            fprintf(fp,"        dae_obj_memberdef* at = attribs;\n");
            while(atitr != atend)
            {
                xsd_attrib* at = *atitr;
                const char* deflt = at->deflt;
                char atcn[128];
                char attcid[128];
                int req;
                gen_cname_attrib(atcn, at->name, sizeof(atcn));
                gen_cname_id(attcid, at->type, sizeof(attcid));
                req = (at->required!=xsd_UNSET) ? at->required : 0;
                fprintf(fp,"        at->name=\"%s\";\n", at->name);
                fprintf(fp,"        at->offset=offsetof(%s,%s);\n",tcn,atcn);
                fprintf(fp,"        at->objtypeid=%s;\n", attcid);
                fprintf(fp,"        at->min=%i;\n", req);
                fprintf(fp,"        at->max=1;\n");
                if(deflt != NULL)
                {
                    fprintf(fp,"        at->deflt=\"%s\";\n",deflt);
                }
                fprintf(fp,"        ++at;\n");
                ++atitr;
            }
            fprintf(fp,"        t->attribs = attribs;\n");
            fprintf(fp,"        t->numattribs = %i;\n", numats);
            fprintf(fp,"    }\n");
        }
        if(t->hascomplex && t->complex.numelements > 0)
        {
            xsd_element** elitr = t->complex.elements;
            xsd_element** elend = elitr + t->complex.numelements;
            int numels = t->complex.numelements;
            fprintf(fp,"    {\n");
            fprintf(fp,"        static dae_obj_memberdef elems[%i];\n",numels);
            fprintf(fp,"        dae_obj_memberdef* el = elems;\n");
            while(elitr != elend)
            {
                xsd_element* el = *elitr;
                char elcn[128];
                char eltcid[128];
                int min;
                int max;
                gen_cname_elem(elcn, el->name, sizeof(elcn));
                gen_cname_id(eltcid, el->type, sizeof(eltcid));
                min = (el->minoccurs!=xsd_UNSET) ? el->minoccurs : 1;
                max = (el->maxoccurs!=xsd_UNSET) ? el->maxoccurs : 1;
                fprintf(fp,"        el->name=\"%s\";\n", el->name);
                fprintf(fp,"        el->offset=offsetof(%s,%s);\n",tcn,elcn);
                fprintf(fp,"        el->objtypeid=%s;\n", eltcid);
                fprintf(fp,"        el->min=%i;\n", min);
                fprintf(fp,"        el->max=%i;\n", max);
                fprintf(fp,"        el->seq=%i;\n", el->seq);
                fprintf(fp,"        ++el;\n");
                ++elitr;
            }

            fprintf(fp,"        t->elems = elems;\n");
            fprintf(fp,"        t->numelems = %i;\n", numels);
            fprintf(fp,"    }\n");
        }
        fprintf(fp,"    ++t;\n");
        ++titr;
    }
    fprintf(fp,"    *types_out = s_types;\n");
    fprintf(fp,"    *numtypes_out = sizeof(s_types)/sizeof(*s_types);\n");
    fprintf(fp,"}\n");
}
