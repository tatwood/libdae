/**
 * @brief     libdae source
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#include <dae/dae.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
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
static const dae_schemainfo* dae_construct_schema()
{
    static dae_schemainfo schema;
    static dae_structinfo structs[219];
    {
        dae_structinfo* def = structs + dae_ID_TRANSFORMATION_ELEMENT;
        static dae_eleminfo elem[6];
        elem[0].name = "lookat";
        elem[0].dae_type = dae_ID_LOOKAT;
        elem[0].restrict = dae_RESTRICT_CHOICE;
        elem[0].offset = offsetof(dae_transformation_element,lookat);
        elem[1].name = "matrix";
        elem[1].dae_type = dae_ID_MATRIX;
        elem[1].restrict = dae_RESTRICT_CHOICE;
        elem[1].offset = offsetof(dae_transformation_element,matrix);
        elem[2].name = "rotate";
        elem[2].dae_type = dae_ID_ROTATE;
        elem[2].restrict = dae_RESTRICT_CHOICE;
        elem[2].offset = offsetof(dae_transformation_element,rotate);
        elem[3].name = "scale";
        elem[3].dae_type = dae_ID_SCALE;
        elem[3].restrict = dae_RESTRICT_CHOICE;
        elem[3].offset = offsetof(dae_transformation_element,scale);
        elem[4].name = "skew";
        elem[4].dae_type = dae_ID_SKEW;
        elem[4].restrict = dae_RESTRICT_CHOICE;
        elem[4].offset = offsetof(dae_transformation_element,skew);
        elem[5].name = "translate";
        elem[5].dae_type = dae_ID_TRANSLATE;
        elem[5].restrict = dae_RESTRICT_CHOICE;
        elem[5].offset = offsetof(dae_transformation_element,translate);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 1;
        def->numattrs = 0;
        def->numelems = 6;
        def->size = sizeof(dae_transformation_element);
    }
    {
        dae_structinfo* def = structs + dae_ID_LOOKAT;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_lookat,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_lookat,data);
        def->mindata = 9;
        def->maxdata = 9;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_lookat);
    }
    {
        dae_structinfo* def = structs + dae_ID_MATRIX;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_matrix,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_matrix,data);
        def->mindata = 16;
        def->maxdata = 16;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_matrix);
    }
    {
        dae_structinfo* def = structs + dae_ID_ROTATE;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_rotate,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_rotate,data);
        def->mindata = 4;
        def->maxdata = 4;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_rotate);
    }
    {
        dae_structinfo* def = structs + dae_ID_SCALE;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_scale,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_scale,data);
        def->mindata = 3;
        def->maxdata = 3;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_scale);
    }
    {
        dae_structinfo* def = structs + dae_ID_SKEW;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_skew,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_skew,data);
        def->mindata = 7;
        def->maxdata = 7;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_skew);
    }
    {
        dae_structinfo* def = structs + dae_ID_TRANSLATE;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_translate,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_translate,data);
        def->mindata = 3;
        def->maxdata = 3;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_translate);
    }
    {
        dae_structinfo* def = structs + dae_ID_ACCESSOR;
        static dae_attrinfo attr[4];
        static dae_eleminfo elem[1];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_accessor,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "offset";
        attr[1].offset = offsetof(dae_accessor,offset);
        attr[1].datatype = dae_DATA_UINT;
        attr[1].defaultval.uint_val = 0;
        attr[2].name = "source";
        attr[2].offset = offsetof(dae_accessor,source);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        attr[3].name = "stride";
        attr[3].offset = offsetof(dae_accessor,stride);
        attr[3].datatype = dae_DATA_UINT;
        attr[3].defaultval.uint_val = 0;
        elem[0].name = "param";
        elem[0].dae_type = dae_ID_PARAM;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_accessor,param);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 4;
        def->numelems = 1;
        def->size = sizeof(dae_accessor);
    }
    {
        dae_structinfo* def = structs + dae_ID_PARAM;
        static dae_attrinfo attr[4];
        attr[0].name = "name";
        attr[0].offset = offsetof(dae_param,name);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "sid";
        attr[1].offset = offsetof(dae_param,sid);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "type";
        attr[2].offset = offsetof(dae_param,type);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        attr[3].name = "semantic";
        attr[3].offset = offsetof(dae_param,semantic);
        attr[3].datatype = dae_DATA_STRING;
        attr[3].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 4;
        def->numelems = 0;
        def->size = sizeof(dae_param);
    }
    {
        dae_structinfo* def = structs + dae_ID_AMBIENT;
        static dae_eleminfo elem[1];
        elem[0].name = "color";
        elem[0].dae_type = dae_ID_TARGETABLE_FLOAT3;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_ambient,color);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 1;
        def->size = sizeof(dae_ambient);
    }
    {
        dae_structinfo* def = structs + dae_ID_TARGETABLE_FLOAT3;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_targetable_float3,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_targetable_float3,data);
        def->mindata = 3;
        def->maxdata = 3;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_targetable_float3);
    }
    {
        dae_structinfo* def = structs + dae_ID_ANIMATION;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[6];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_animation,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_animation,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_animation,asset);
        elem[1].name = "animation";
        elem[1].dae_type = dae_ID_ANIMATION;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_animation,animation);
        elem[2].name = "source";
        elem[2].dae_type = dae_ID_SOURCE;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_animation,source);
        elem[3].name = "sampler";
        elem[3].dae_type = dae_ID_SAMPLER;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_animation,sampler);
        elem[4].name = "channel";
        elem[4].dae_type = dae_ID_CHANNEL;
        elem[4].restrict = dae_RESTRICT_LIST;
        elem[4].offset = offsetof(dae_animation,channel);
        elem[5].name = "extra";
        elem[5].dae_type = dae_ID_EXTRA;
        elem[5].restrict = dae_RESTRICT_LIST;
        elem[5].offset = offsetof(dae_animation,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 6;
        def->size = sizeof(dae_animation);
    }
    {
        dae_structinfo* def = structs + dae_ID_ASSET;
        static dae_eleminfo elem[11];
        elem[0].name = "contributor";
        elem[0].dae_type = dae_ID_CONTRIBUTOR;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_asset,contributor);
        elem[1].name = "coverage";
        elem[1].dae_type = dae_ID_ASSET_COVERAGE;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_asset,coverage);
        elem[2].name = "created";
        elem[2].dae_type = dae_ID_STRING;
        elem[2].restrict = dae_RESTRICT_REQUIRED;
        elem[2].offset = offsetof(dae_asset,created);
        elem[3].name = "keywords";
        elem[3].dae_type = dae_ID_STRING;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_asset,keywords);
        elem[4].name = "modified";
        elem[4].dae_type = dae_ID_STRING;
        elem[4].restrict = dae_RESTRICT_REQUIRED;
        elem[4].offset = offsetof(dae_asset,modified);
        elem[5].name = "revision";
        elem[5].dae_type = dae_ID_STRING;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_asset,revision);
        elem[6].name = "subject";
        elem[6].dae_type = dae_ID_STRING;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_asset,subject);
        elem[7].name = "title";
        elem[7].dae_type = dae_ID_STRING;
        elem[7].restrict = dae_RESTRICT_OPTIONAL;
        elem[7].offset = offsetof(dae_asset,title);
        elem[8].name = "unit";
        elem[8].dae_type = dae_ID_ASSET_UNIT;
        elem[8].restrict = dae_RESTRICT_OPTIONAL;
        elem[8].offset = offsetof(dae_asset,unit);
        elem[9].name = "up_axis";
        elem[9].dae_type = dae_ID_STRING;
        elem[9].restrict = dae_RESTRICT_OPTIONAL;
        elem[9].offset = offsetof(dae_asset,up_axis);
        elem[10].name = "extra";
        elem[10].dae_type = dae_ID_EXTRA;
        elem[10].restrict = dae_RESTRICT_LIST;
        elem[10].offset = offsetof(dae_asset,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 11;
        def->size = sizeof(dae_asset);
    }
    {
        dae_structinfo* def = structs + dae_ID_SOURCE;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[9];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_source,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_source,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "bool_array";
        elem[0].dae_type = dae_ID_BOOL_ARRAY;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_source,bool_array);
        elem[1].name = "float_array";
        elem[1].dae_type = dae_ID_FLOAT_ARRAY;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_source,float_array);
        elem[2].name = "IDREF_array";
        elem[2].dae_type = dae_ID_IDREF_ARRAY;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_source,IDREF_array);
        elem[3].name = "int_array";
        elem[3].dae_type = dae_ID_INT_ARRAY;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_source,int_array);
        elem[4].name = "Name_array";
        elem[4].dae_type = dae_ID_NAME_ARRAY;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_source,Name_array);
        elem[5].name = "SIDREF_array";
        elem[5].dae_type = dae_ID_SIDREF_ARRAY;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_source,SIDREF_array);
        elem[6].name = "token_array";
        elem[6].dae_type = dae_ID_TOKEN_ARRAY;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_source,token_array);
        elem[7].name = "technique_common";
        elem[7].dae_type = dae_ID_SOURCE_TECHNIQUE_COMMON;
        elem[7].restrict = dae_RESTRICT_OPTIONAL;
        elem[7].offset = offsetof(dae_source,technique_common);
        elem[8].name = "technique";
        elem[8].dae_type = dae_ID_TECHNIQUE;
        elem[8].restrict = dae_RESTRICT_LIST;
        elem[8].offset = offsetof(dae_source,technique);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 9;
        def->size = sizeof(dae_source);
    }
    {
        dae_structinfo* def = structs + dae_ID_SAMPLER;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[1];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_sampler,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "pre_behavior";
        attr[1].offset = offsetof(dae_sampler,pre_behavior);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "post_behavior";
        attr[2].offset = offsetof(dae_sampler,post_behavior);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_sampler,input);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 1;
        def->size = sizeof(dae_sampler);
    }
    {
        dae_structinfo* def = structs + dae_ID_CHANNEL;
        static dae_attrinfo attr[2];
        attr[0].name = "source";
        attr[0].offset = offsetof(dae_channel,source);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "target";
        attr[1].offset = offsetof(dae_channel,target);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 0;
        def->size = sizeof(dae_channel);
    }
    {
        dae_structinfo* def = structs + dae_ID_EXTRA;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[2];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_extra,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_extra,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "type";
        attr[2].offset = offsetof(dae_extra,type);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_extra,asset);
        elem[1].name = "technique";
        elem[1].dae_type = dae_ID_TECHNIQUE;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_extra,technique);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 2;
        def->size = sizeof(dae_extra);
    }
    {
        dae_structinfo* def = structs + dae_ID_ANIMATION_CLIP;
        static dae_attrinfo attr[4];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_animation_clip,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "start";
        attr[1].offset = offsetof(dae_animation_clip,start);
        attr[1].datatype = dae_DATA_FLOAT;
        attr[1].defaultval.float_val = 0.000000;
        attr[1].datatype = dae_DATA_INT;
        attr[1].defaultval.int_val = 0;
        attr[2].name = "end";
        attr[2].offset = offsetof(dae_animation_clip,end);
        attr[2].datatype = dae_DATA_FLOAT;
        attr[2].defaultval.float_val = 0.000000;
        attr[2].datatype = dae_DATA_INT;
        attr[2].defaultval.int_val = 0;
        attr[3].name = "name";
        attr[3].offset = offsetof(dae_animation_clip,name);
        attr[3].datatype = dae_DATA_STRING;
        attr[3].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_animation_clip,asset);
        elem[1].name = "instance_animation";
        elem[1].dae_type = dae_ID_INSTANCE_ANIMATION;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_animation_clip,instance_animation);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_animation_clip,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 4;
        def->numelems = 3;
        def->size = sizeof(dae_animation_clip);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_ANIMATION;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_animation,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_animation,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_animation,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "extra";
        elem[0].dae_type = dae_ID_EXTRA;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_instance_animation,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 1;
        def->size = sizeof(dae_instance_animation);
    }
    {
        dae_structinfo* def = structs + dae_ID_ANNOTATE;
        static dae_eleminfo elem[16];
        elem[0].name = "bool";
        elem[0].dae_type = dae_ID_BOOL;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_annotate,bool_);
        elem[1].name = "bool2";
        elem[1].dae_type = dae_ID_BOOL2;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_annotate,bool2);
        elem[2].name = "bool3";
        elem[2].dae_type = dae_ID_BOOL3;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_annotate,bool3);
        elem[3].name = "bool4";
        elem[3].dae_type = dae_ID_BOOL4;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_annotate,bool4);
        elem[4].name = "int";
        elem[4].dae_type = dae_ID_INT;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_annotate,int_);
        elem[5].name = "int2";
        elem[5].dae_type = dae_ID_INT2;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_annotate,int2);
        elem[6].name = "int3";
        elem[6].dae_type = dae_ID_INT3;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_annotate,int3);
        elem[7].name = "int4";
        elem[7].dae_type = dae_ID_INT4;
        elem[7].restrict = dae_RESTRICT_OPTIONAL;
        elem[7].offset = offsetof(dae_annotate,int4);
        elem[8].name = "float";
        elem[8].dae_type = dae_ID_FLOAT;
        elem[8].restrict = dae_RESTRICT_OPTIONAL;
        elem[8].offset = offsetof(dae_annotate,float_);
        elem[9].name = "float2";
        elem[9].dae_type = dae_ID_FLOAT2;
        elem[9].restrict = dae_RESTRICT_OPTIONAL;
        elem[9].offset = offsetof(dae_annotate,float2);
        elem[10].name = "float3";
        elem[10].dae_type = dae_ID_FLOAT3;
        elem[10].restrict = dae_RESTRICT_OPTIONAL;
        elem[10].offset = offsetof(dae_annotate,float3);
        elem[11].name = "float4";
        elem[11].dae_type = dae_ID_FLOAT4;
        elem[11].restrict = dae_RESTRICT_OPTIONAL;
        elem[11].offset = offsetof(dae_annotate,float4);
        elem[12].name = "float2x2";
        elem[12].dae_type = dae_ID_FLOAT2X2;
        elem[12].restrict = dae_RESTRICT_OPTIONAL;
        elem[12].offset = offsetof(dae_annotate,float2x2);
        elem[13].name = "float3x3";
        elem[13].dae_type = dae_ID_FLOAT3X3;
        elem[13].restrict = dae_RESTRICT_OPTIONAL;
        elem[13].offset = offsetof(dae_annotate,float3x3);
        elem[14].name = "float4x4";
        elem[14].dae_type = dae_ID_FLOAT4X4;
        elem[14].restrict = dae_RESTRICT_OPTIONAL;
        elem[14].offset = offsetof(dae_annotate,float4x4);
        elem[15].name = "string";
        elem[15].dae_type = dae_ID_STRING;
        elem[15].restrict = dae_RESTRICT_OPTIONAL;
        elem[15].offset = offsetof(dae_annotate,string);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 16;
        def->size = sizeof(dae_annotate);
    }
    {
        dae_structinfo* def = structs + dae_ID_BOOL;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_BOOL;
        def->dataoffset = offsetof(dae_bool,data);
        def->mindata = 1;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_bool);
    }
    {
        dae_structinfo* def = structs + dae_ID_BOOL2;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_BOOL;
        def->dataoffset = offsetof(dae_bool2,data);
        def->mindata = 2;
        def->maxdata = 2;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_bool2);
    }
    {
        dae_structinfo* def = structs + dae_ID_BOOL3;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_BOOL;
        def->dataoffset = offsetof(dae_bool3,data);
        def->mindata = 3;
        def->maxdata = 3;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_bool3);
    }
    {
        dae_structinfo* def = structs + dae_ID_BOOL4;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_BOOL;
        def->dataoffset = offsetof(dae_bool4,data);
        def->mindata = 4;
        def->maxdata = 4;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_bool4);
    }
    {
        dae_structinfo* def = structs + dae_ID_INT;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_INT;
        def->dataoffset = offsetof(dae_int,data);
        def->mindata = 1;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_int);
    }
    {
        dae_structinfo* def = structs + dae_ID_INT2;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_INT;
        def->dataoffset = offsetof(dae_int2,data);
        def->mindata = 2;
        def->maxdata = 2;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_int2);
    }
    {
        dae_structinfo* def = structs + dae_ID_INT3;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_INT;
        def->dataoffset = offsetof(dae_int3,data);
        def->mindata = 3;
        def->maxdata = 3;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_int3);
    }
    {
        dae_structinfo* def = structs + dae_ID_INT4;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_INT;
        def->dataoffset = offsetof(dae_int4,data);
        def->mindata = 4;
        def->maxdata = 4;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_int4);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_float,data);
        def->mindata = 1;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_float);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT2;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_float2,data);
        def->mindata = 2;
        def->maxdata = 2;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_float2);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT3;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_float3,data);
        def->mindata = 3;
        def->maxdata = 3;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_float3);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT4;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_float4,data);
        def->mindata = 4;
        def->maxdata = 4;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_float4);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT2X2;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_float2x2,data);
        def->mindata = 4;
        def->maxdata = 4;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_float2x2);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT3X3;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_float3x3,data);
        def->mindata = 9;
        def->maxdata = 9;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_float3x3);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT4X4;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_float4x4,data);
        def->mindata = 16;
        def->maxdata = 16;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_float4x4);
    }
    {
        dae_structinfo* def = structs + dae_ID_STRING;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_STRING;
        def->dataoffset = offsetof(dae_string,data);
        def->mindata = 0;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_string);
    }
    {
        dae_structinfo* def = structs + dae_ID_CONTRIBUTOR;
        static dae_eleminfo elem[7];
        elem[0].name = "author";
        elem[0].dae_type = dae_ID_STRING;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_contributor,author);
        elem[1].name = "authoring_email";
        elem[1].dae_type = dae_ID_STRING;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_contributor,authoring_email);
        elem[2].name = "authoring_website";
        elem[2].dae_type = dae_ID_STRING;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_contributor,authoring_website);
        elem[3].name = "authoring_tool";
        elem[3].dae_type = dae_ID_STRING;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_contributor,authoring_tool);
        elem[4].name = "comments";
        elem[4].dae_type = dae_ID_STRING;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_contributor,comments);
        elem[5].name = "copyright";
        elem[5].dae_type = dae_ID_STRING;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_contributor,copyright);
        elem[6].name = "source_data";
        elem[6].dae_type = dae_ID_STRING;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_contributor,source_data);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 7;
        def->size = sizeof(dae_contributor);
    }
    {
        dae_structinfo* def = structs + dae_ID_ASSET_COVERAGE;
        static dae_eleminfo elem[1];
        elem[0].name = "geographic_location";
        elem[0].dae_type = dae_ID_GEOGRAPHIC_LOCATION;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_asset_coverage,geographic_location);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 1;
        def->size = sizeof(dae_asset_coverage);
    }
    {
        dae_structinfo* def = structs + dae_ID_ASSET_UNIT;
        static dae_attrinfo attr[2];
        attr[0].name = "name";
        attr[0].offset = offsetof(dae_asset_unit,name);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "meter";
        attr[1].offset = offsetof(dae_asset_unit,meter);
        attr[1].datatype = dae_DATA_FLOAT;
        attr[1].defaultval.float_val = 0.000000;
        attr[1].datatype = dae_DATA_INT;
        attr[1].defaultval.int_val = 0;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 0;
        def->size = sizeof(dae_asset_unit);
    }
    {
        dae_structinfo* def = structs + dae_ID_GEOGRAPHIC_LOCATION;
        static dae_eleminfo elem[3];
        elem[0].name = "longitude";
        elem[0].dae_type = dae_ID_FLOAT;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_geographic_location,longitude);
        elem[1].name = "latitude";
        elem[1].dae_type = dae_ID_FLOAT;
        elem[1].restrict = dae_RESTRICT_REQUIRED;
        elem[1].offset = offsetof(dae_geographic_location,latitude);
        elem[2].name = "altitude";
        elem[2].dae_type = dae_ID_GEOGRAPHIC_LOCATION_ALTITUDE;
        elem[2].restrict = dae_RESTRICT_REQUIRED;
        elem[2].offset = offsetof(dae_geographic_location,altitude);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 3;
        def->size = sizeof(dae_geographic_location);
    }
    {
        dae_structinfo* def = structs + dae_ID_BLINN;
        static dae_eleminfo elem[10];
        elem[0].name = "emission";
        elem[0].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_blinn,emission);
        elem[1].name = "ambient";
        elem[1].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_blinn,ambient);
        elem[2].name = "diffuse";
        elem[2].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_blinn,diffuse);
        elem[3].name = "specular";
        elem[3].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_blinn,specular);
        elem[4].name = "shininess";
        elem[4].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_blinn,shininess);
        elem[5].name = "reflective";
        elem[5].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_blinn,reflective);
        elem[6].name = "reflectivity";
        elem[6].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_blinn,reflectivity);
        elem[7].name = "transparent";
        elem[7].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[7].restrict = dae_RESTRICT_OPTIONAL;
        elem[7].offset = offsetof(dae_blinn,transparent);
        elem[8].name = "transparency";
        elem[8].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[8].restrict = dae_RESTRICT_OPTIONAL;
        elem[8].offset = offsetof(dae_blinn,transparency);
        elem[9].name = "index_of_refraction";
        elem[9].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[9].restrict = dae_RESTRICT_OPTIONAL;
        elem[9].offset = offsetof(dae_blinn,index_of_refraction);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 10;
        def->size = sizeof(dae_blinn);
    }
    {
        dae_structinfo* def = structs + dae_ID_COLOR_OR_TEXTURE;
        static dae_attrinfo attr[1];
        static dae_eleminfo elem[3];
        attr[0].name = "opaque";
        attr[0].offset = offsetof(dae_color_or_texture,opaque);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        elem[0].name = "color";
        elem[0].dae_type = dae_ID_FLOAT4;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_color_or_texture,color);
        elem[1].name = "param";
        elem[1].dae_type = dae_ID_PARAM_REF;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_color_or_texture,param);
        elem[2].name = "texture";
        elem[2].dae_type = dae_ID_FX_TEXTURE;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_color_or_texture,texture);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 3;
        def->size = sizeof(dae_color_or_texture);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT_OR_PARAM;
        static dae_eleminfo elem[2];
        elem[0].name = "float";
        elem[0].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_float_or_param,float_);
        elem[1].name = "param";
        elem[1].dae_type = dae_ID_PARAM_REF;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_float_or_param,param);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 2;
        def->size = sizeof(dae_float_or_param);
    }
    {
        dae_structinfo* def = structs + dae_ID_BOOL_ARRAY;
        static dae_attrinfo attr[3];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_bool_array,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "id";
        attr[1].offset = offsetof(dae_bool_array,id);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "name";
        attr[2].offset = offsetof(dae_bool_array,name);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_BOOL;
        def->dataoffset = offsetof(dae_bool_array,data);
        def->mindata = 0;
        def->maxdata = -1;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 0;
        def->size = sizeof(dae_bool_array);
    }
    {
        dae_structinfo* def = structs + dae_ID_CAMERA;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[4];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_camera,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_camera,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_camera,asset);
        elem[1].name = "optics";
        elem[1].dae_type = dae_ID_OPTICS;
        elem[1].restrict = dae_RESTRICT_REQUIRED;
        elem[1].offset = offsetof(dae_camera,optics);
        elem[2].name = "imager";
        elem[2].dae_type = dae_ID_IMAGER;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_camera,imager);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_camera,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 4;
        def->size = sizeof(dae_camera);
    }
    {
        dae_structinfo* def = structs + dae_ID_OPTICS;
        static dae_eleminfo elem[3];
        elem[0].name = "technique_common";
        elem[0].dae_type = dae_ID_OPTICS_TECHNIQUE_COMMON;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_optics,technique_common);
        elem[1].name = "technique";
        elem[1].dae_type = dae_ID_TECHNIQUE;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_optics,technique);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_optics,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 3;
        def->size = sizeof(dae_optics);
    }
    {
        dae_structinfo* def = structs + dae_ID_IMAGER;
        static dae_eleminfo elem[2];
        elem[0].name = "technique";
        elem[0].dae_type = dae_ID_TECHNIQUE;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_imager,technique);
        elem[1].name = "extra";
        elem[1].dae_type = dae_ID_EXTRA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_imager,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 2;
        def->size = sizeof(dae_imager);
    }
    {
        dae_structinfo* def = structs + dae_ID_COLLADA;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[22];
        attr[0].name = "version";
        attr[0].offset = offsetof(dae_COLLADA,version);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "xmlns";
        attr[1].offset = offsetof(dae_COLLADA,xmlns);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "base";
        attr[2].offset = offsetof(dae_COLLADA,base);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_COLLADA,asset);
        elem[1].name = "library_animation_clips";
        elem[1].dae_type = dae_ID_LIBRARY_ANIMATION_CLIPS;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_COLLADA,library_animation_clips);
        elem[2].name = "library_animations";
        elem[2].dae_type = dae_ID_LIBRARY_ANIMATIONS;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_COLLADA,library_animations);
        elem[3].name = "library_articulated_systems";
        elem[3].dae_type = dae_ID_LIBRARY_ARTICULATED_SYSTEMS;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_COLLADA,library_articulated_systems);
        elem[4].name = "library_cameras";
        elem[4].dae_type = dae_ID_LIBRARY_CAMERAS;
        elem[4].restrict = dae_RESTRICT_LIST;
        elem[4].offset = offsetof(dae_COLLADA,library_cameras);
        elem[5].name = "library_controllers";
        elem[5].dae_type = dae_ID_LIBRARY_CONTROLLERS;
        elem[5].restrict = dae_RESTRICT_LIST;
        elem[5].offset = offsetof(dae_COLLADA,library_controllers);
        elem[6].name = "library_effects";
        elem[6].dae_type = dae_ID_LIBRARY_EFFECTS;
        elem[6].restrict = dae_RESTRICT_LIST;
        elem[6].offset = offsetof(dae_COLLADA,library_effects);
        elem[7].name = "library_force_fields";
        elem[7].dae_type = dae_ID_LIBRARY_FORCE_FIELDS;
        elem[7].restrict = dae_RESTRICT_LIST;
        elem[7].offset = offsetof(dae_COLLADA,library_force_fields);
        elem[8].name = "library_geometries";
        elem[8].dae_type = dae_ID_LIBRARY_GEOMETRIES;
        elem[8].restrict = dae_RESTRICT_LIST;
        elem[8].offset = offsetof(dae_COLLADA,library_geometries);
        elem[9].name = "library_images";
        elem[9].dae_type = dae_ID_LIBRARY_IMAGES;
        elem[9].restrict = dae_RESTRICT_LIST;
        elem[9].offset = offsetof(dae_COLLADA,library_images);
        elem[10].name = "library_joints";
        elem[10].dae_type = dae_ID_LIBRARY_JOINTS;
        elem[10].restrict = dae_RESTRICT_LIST;
        elem[10].offset = offsetof(dae_COLLADA,library_joints);
        elem[11].name = "library_kinematic_models";
        elem[11].dae_type = dae_ID_LIBRARY_KINEMATIC_MODELS;
        elem[11].restrict = dae_RESTRICT_LIST;
        elem[11].offset = offsetof(dae_COLLADA,library_kinematic_models);
        elem[12].name = "library_kinematic_scenes";
        elem[12].dae_type = dae_ID_LIBRARY_KINEMATIC_SCENES;
        elem[12].restrict = dae_RESTRICT_LIST;
        elem[12].offset = offsetof(dae_COLLADA,library_kinematic_scenes);
        elem[13].name = "library_lights";
        elem[13].dae_type = dae_ID_LIBRARY_LIGHTS;
        elem[13].restrict = dae_RESTRICT_LIST;
        elem[13].offset = offsetof(dae_COLLADA,library_lights);
        elem[14].name = "library_materials";
        elem[14].dae_type = dae_ID_LIBRARY_MATERIALS;
        elem[14].restrict = dae_RESTRICT_LIST;
        elem[14].offset = offsetof(dae_COLLADA,library_materials);
        elem[15].name = "library_nodes";
        elem[15].dae_type = dae_ID_LIBRARY_NODES;
        elem[15].restrict = dae_RESTRICT_LIST;
        elem[15].offset = offsetof(dae_COLLADA,library_nodes);
        elem[16].name = "library_physics_materials";
        elem[16].dae_type = dae_ID_LIBRARY_PHYSICS_MATERIALS;
        elem[16].restrict = dae_RESTRICT_LIST;
        elem[16].offset = offsetof(dae_COLLADA,library_physics_materials);
        elem[17].name = "library_physics_models";
        elem[17].dae_type = dae_ID_LIBRARY_PHYSICS_MODELS;
        elem[17].restrict = dae_RESTRICT_LIST;
        elem[17].offset = offsetof(dae_COLLADA,library_physics_models);
        elem[18].name = "library_physics_scenes";
        elem[18].dae_type = dae_ID_LIBRARY_PHYSICS_SCENES;
        elem[18].restrict = dae_RESTRICT_LIST;
        elem[18].offset = offsetof(dae_COLLADA,library_physics_scenes);
        elem[19].name = "library_visual_scenes";
        elem[19].dae_type = dae_ID_LIBRARY_VISUAL_SCENES;
        elem[19].restrict = dae_RESTRICT_LIST;
        elem[19].offset = offsetof(dae_COLLADA,library_visual_scenes);
        elem[20].name = "scene";
        elem[20].dae_type = dae_ID_SCENE;
        elem[20].restrict = dae_RESTRICT_OPTIONAL;
        elem[20].offset = offsetof(dae_COLLADA,scene);
        elem[21].name = "extra";
        elem[21].dae_type = dae_ID_EXTRA;
        elem[21].restrict = dae_RESTRICT_LIST;
        elem[21].offset = offsetof(dae_COLLADA,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 22;
        def->size = sizeof(dae_COLLADA);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_ANIMATION_CLIPS;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_animation_clips,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_animation_clips,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_animation_clips,asset);
        elem[1].name = "animation_clip";
        elem[1].dae_type = dae_ID_ANIMATION_CLIP;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_animation_clips,animation_clip);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_animation_clips,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_animation_clips);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_ANIMATIONS;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_animations,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_animations,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_animations,asset);
        elem[1].name = "animation";
        elem[1].dae_type = dae_ID_ANIMATION;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_animations,animation);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_animations,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_animations);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_ARTICULATED_SYSTEMS;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_library_articulated_systems);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_CAMERAS;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_cameras,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_cameras,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_cameras,asset);
        elem[1].name = "camera";
        elem[1].dae_type = dae_ID_CAMERA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_cameras,camera);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_cameras,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_cameras);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_CONTROLLERS;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_controllers,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_controllers,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_controllers,asset);
        elem[1].name = "controller";
        elem[1].dae_type = dae_ID_CONTROLLER;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_controllers,controller);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_controllers,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_controllers);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_EFFECTS;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_effects,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_effects,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_effects,asset);
        elem[1].name = "effect";
        elem[1].dae_type = dae_ID_EFFECT;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_effects,effect);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_effects,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_effects);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_FORCE_FIELDS;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_library_force_fields);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_GEOMETRIES;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_geometries,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_geometries,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_geometries,asset);
        elem[1].name = "geometry";
        elem[1].dae_type = dae_ID_GEOMETRY;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_geometries,geometry);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_geometries,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_geometries);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_IMAGES;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_images,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_images,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_images,asset);
        elem[1].name = "image";
        elem[1].dae_type = dae_ID_IMAGE;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_images,image);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_images,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_images);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_JOINTS;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_library_joints);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_KINEMATIC_MODELS;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_library_kinematic_models);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_KINEMATIC_SCENES;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_library_kinematic_scenes);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_LIGHTS;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_lights,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_lights,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_lights,asset);
        elem[1].name = "light";
        elem[1].dae_type = dae_ID_LIGHT;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_lights,light);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_lights,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_lights);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_MATERIALS;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_materials,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_materials,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_materials,asset);
        elem[1].name = "material";
        elem[1].dae_type = dae_ID_MATERIAL;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_materials,material);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_materials,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_materials);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_NODES;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_nodes,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_nodes,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_nodes,asset);
        elem[1].name = "node";
        elem[1].dae_type = dae_ID_NODE;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_nodes,node);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_nodes,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_nodes);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_PHYSICS_MATERIALS;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_library_physics_materials);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_PHYSICS_MODELS;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_library_physics_models);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_PHYSICS_SCENES;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_library_physics_scenes);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_VISUAL_SCENES;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_visual_scenes,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_visual_scenes,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_visual_scenes,asset);
        elem[1].name = "visual_scene";
        elem[1].dae_type = dae_ID_VISUAL_SCENE;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_visual_scenes,visual_scene);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_visual_scenes,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_visual_scenes);
    }
    {
        dae_structinfo* def = structs + dae_ID_SCENE;
        static dae_eleminfo elem[4];
        elem[0].name = "instance_physics_scene";
        elem[0].dae_type = dae_ID_INSTANCE_PHYSICS_SCENE;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_scene,instance_physics_scene);
        elem[1].name = "instance_visual_scene";
        elem[1].dae_type = dae_ID_INSTANCE_VISUAL_SCENE;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_scene,instance_visual_scene);
        elem[2].name = "instance_kinematics_scene";
        elem[2].dae_type = dae_ID_INSTANCE_KINEMATICS_SCENE;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_scene,instance_kinematics_scene);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_scene,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 4;
        def->size = sizeof(dae_scene);
    }
    {
        dae_structinfo* def = structs + dae_ID_PARAM_REF;
        static dae_attrinfo attr[1];
        attr[0].name = "ref";
        attr[0].offset = offsetof(dae_param_ref,ref);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_param_ref);
    }
    {
        dae_structinfo* def = structs + dae_ID_FX_TEXTURE;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[1];
        attr[0].name = "texture";
        attr[0].offset = offsetof(dae_fx_texture,texture);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "texcoord";
        attr[1].offset = offsetof(dae_fx_texture,texcoord);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "extra";
        elem[0].dae_type = dae_ID_EXTRA;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_fx_texture,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 1;
        def->size = sizeof(dae_fx_texture);
    }
    {
        dae_structinfo* def = structs + dae_ID_CONSTANT;
        static dae_eleminfo elem[6];
        elem[0].name = "emission";
        elem[0].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_constant,emission);
        elem[1].name = "reflective";
        elem[1].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_constant,reflective);
        elem[2].name = "reflectivity";
        elem[2].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_constant,reflectivity);
        elem[3].name = "transparent";
        elem[3].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_constant,transparent);
        elem[4].name = "transparency";
        elem[4].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_constant,transparency);
        elem[5].name = "index_of_refraction";
        elem[5].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_constant,index_of_refraction);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 6;
        def->size = sizeof(dae_constant);
    }
    {
        dae_structinfo* def = structs + dae_ID_CONTROLLER;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[4];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_controller,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_controller,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_controller,asset);
        elem[1].name = "skin";
        elem[1].dae_type = dae_ID_SKIN;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_controller,skin);
        elem[2].name = "morph";
        elem[2].dae_type = dae_ID_MORPH;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_controller,morph);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_controller,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 4;
        def->size = sizeof(dae_controller);
    }
    {
        dae_structinfo* def = structs + dae_ID_SKIN;
        static dae_attrinfo attr[1];
        static dae_eleminfo elem[5];
        attr[0].name = "source";
        attr[0].offset = offsetof(dae_skin,source);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        elem[0].name = "bind_shape_matrix";
        elem[0].dae_type = dae_ID_FLOAT4X4;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_skin,bind_shape_matrix);
        elem[1].name = "source";
        elem[1].dae_type = dae_ID_SOURCE;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_skin,source_);
        elem[2].name = "joints";
        elem[2].dae_type = dae_ID_JOINTS;
        elem[2].restrict = dae_RESTRICT_REQUIRED;
        elem[2].offset = offsetof(dae_skin,joints);
        elem[3].name = "vertex_weights";
        elem[3].dae_type = dae_ID_VERTEX_WEIGHTS;
        elem[3].restrict = dae_RESTRICT_REQUIRED;
        elem[3].offset = offsetof(dae_skin,vertex_weights);
        elem[4].name = "extra";
        elem[4].dae_type = dae_ID_EXTRA;
        elem[4].restrict = dae_RESTRICT_LIST;
        elem[4].offset = offsetof(dae_skin,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 5;
        def->size = sizeof(dae_skin);
    }
    {
        dae_structinfo* def = structs + dae_ID_MORPH;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_morph,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "method";
        attr[1].offset = offsetof(dae_morph,method);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "source";
        elem[0].dae_type = dae_ID_SOURCE;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_morph,source);
        elem[1].name = "targets";
        elem[1].dae_type = dae_ID_TARGETS;
        elem[1].restrict = dae_RESTRICT_REQUIRED;
        elem[1].offset = offsetof(dae_morph,targets);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_morph,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_morph);
    }
    {
        dae_structinfo* def = structs + dae_ID_CONTROL_VERTICES;
        static dae_eleminfo elem[2];
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_UNSHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_control_vertices,input);
        elem[1].name = "extra";
        elem[1].dae_type = dae_ID_EXTRA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_control_vertices,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 2;
        def->size = sizeof(dae_control_vertices);
    }
    {
        dae_structinfo* def = structs + dae_ID_INPUT_UNSHARED;
        static dae_attrinfo attr[2];
        attr[0].name = "semantic";
        attr[0].offset = offsetof(dae_input_unshared,semantic);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "source";
        attr[1].offset = offsetof(dae_input_unshared,source);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 0;
        def->size = sizeof(dae_input_unshared);
    }
    {
        dae_structinfo* def = structs + dae_ID_DIRECTIONAL;
        static dae_eleminfo elem[1];
        elem[0].name = "color";
        elem[0].dae_type = dae_ID_TARGETABLE_FLOAT3;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_directional,color);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 1;
        def->size = sizeof(dae_directional);
    }
    {
        dae_structinfo* def = structs + dae_ID_EFFECT;
        static dae_eleminfo elem[5];
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_effect,asset);
        elem[1].name = "annotate";
        elem[1].dae_type = dae_ID_ANNOTATE;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_effect,annotate);
        elem[2].name = "newparam";
        elem[2].dae_type = dae_ID_NEWPARAM;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_effect,newparam);
        elem[3].name = "profile_COMMON";
        elem[3].dae_type = dae_ID_PROFILE_COMMON;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_effect,profile_COMMON);
        elem[4].name = "extra";
        elem[4].dae_type = dae_ID_EXTRA;
        elem[4].restrict = dae_RESTRICT_LIST;
        elem[4].offset = offsetof(dae_effect,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 5;
        def->size = sizeof(dae_effect);
    }
    {
        dae_structinfo* def = structs + dae_ID_NEWPARAM;
        static dae_attrinfo attr[1];
        static dae_eleminfo elem[8];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_newparam,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        elem[0].name = "annotate";
        elem[0].dae_type = dae_ID_ANNOTATE;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_newparam,annotate);
        elem[1].name = "semantic";
        elem[1].dae_type = dae_ID_STRING;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_newparam,semantic);
        elem[2].name = "modifier";
        elem[2].dae_type = dae_ID_STRING;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_newparam,modifier);
        elem[3].name = "float";
        elem[3].dae_type = dae_ID_FLOAT;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_newparam,float_);
        elem[4].name = "float2";
        elem[4].dae_type = dae_ID_FLOAT2;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_newparam,float2);
        elem[5].name = "float3";
        elem[5].dae_type = dae_ID_FLOAT3;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_newparam,float3);
        elem[6].name = "float4";
        elem[6].dae_type = dae_ID_FLOAT4;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_newparam,float4);
        elem[7].name = "sampler2D";
        elem[7].dae_type = dae_ID_FX_SAMPLER_COMMON;
        elem[7].restrict = dae_RESTRICT_OPTIONAL;
        elem[7].offset = offsetof(dae_newparam,sampler2D);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 8;
        def->size = sizeof(dae_newparam);
    }
    {
        dae_structinfo* def = structs + dae_ID_PROFILE_COMMON;
        static dae_eleminfo elem[4];
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_profile_COMMON,asset);
        elem[1].name = "newparam";
        elem[1].dae_type = dae_ID_NEWPARAM;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_profile_COMMON,newparam);
        elem[2].name = "technique";
        elem[2].dae_type = dae_ID_FX_TECHNIQUE;
        elem[2].restrict = dae_RESTRICT_REQUIRED;
        elem[2].offset = offsetof(dae_profile_COMMON,technique);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_profile_COMMON,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 4;
        def->size = sizeof(dae_profile_COMMON);
    }
    {
        dae_structinfo* def = structs + dae_ID_EVALUATE_SCENE;
        static dae_attrinfo attr[4];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_evaluate_scene,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_evaluate_scene,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "sid";
        attr[2].offset = offsetof(dae_evaluate_scene,sid);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        attr[3].name = "enable";
        attr[3].offset = offsetof(dae_evaluate_scene,enable);
        attr[3].datatype = dae_DATA_BOOL;
        attr[3].defaultval.bool_val = 0;
        attr[3].datatype = dae_DATA_FLOAT;
        attr[3].defaultval.float_val = 0.000000;
        attr[3].datatype = dae_DATA_INT;
        attr[3].defaultval.int_val = 0;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_evaluate_scene,asset);
        elem[1].name = "render";
        elem[1].dae_type = dae_ID_RENDER;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_evaluate_scene,render);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_evaluate_scene,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 4;
        def->numelems = 3;
        def->size = sizeof(dae_evaluate_scene);
    }
    {
        dae_structinfo* def = structs + dae_ID_RENDER;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_render);
    }
    {
        dae_structinfo* def = structs + dae_ID_TECHNIQUE;
        static dae_attrinfo attr[2];
        attr[0].name = "profile";
        attr[0].offset = offsetof(dae_technique,profile);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "xmlns";
        attr[1].offset = offsetof(dae_technique,xmlns);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 0;
        def->size = sizeof(dae_technique);
    }
    {
        dae_structinfo* def = structs + dae_ID_FLOAT_ARRAY;
        static dae_attrinfo attr[5];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_float_array,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "id";
        attr[1].offset = offsetof(dae_float_array,id);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "name";
        attr[2].offset = offsetof(dae_float_array,name);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        attr[3].name = "digits";
        attr[3].offset = offsetof(dae_float_array,digits);
        attr[3].datatype = dae_DATA_INT;
        attr[3].defaultval.int_val = 0;
        attr[4].name = "magnitude";
        attr[4].offset = offsetof(dae_float_array,magnitude);
        attr[4].datatype = dae_DATA_INT;
        attr[4].defaultval.int_val = 0;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_float_array,data);
        def->mindata = 0;
        def->maxdata = -1;
        def->isunion = 0;
        def->numattrs = 5;
        def->numelems = 0;
        def->size = sizeof(dae_float_array);
    }
    {
        dae_structinfo* def = structs + dae_ID_TARGETABLE_FLOAT;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_targetable_float,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_targetable_float,data);
        def->mindata = 1;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_targetable_float);
    }
    {
        dae_structinfo* def = structs + dae_ID_FORMULA;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_formula,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_formula,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "sid";
        attr[2].offset = offsetof(dae_formula,sid);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "target";
        elem[0].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_formula,target);
        elem[1].name = "technique_common";
        elem[1].dae_type = dae_ID_FORMULA_TECHNIQUE_COMMON;
        elem[1].restrict = dae_RESTRICT_REQUIRED;
        elem[1].offset = offsetof(dae_formula,technique_common);
        elem[2].name = "technique";
        elem[2].dae_type = dae_ID_TECHNIQUE;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_formula,technique);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 3;
        def->size = sizeof(dae_formula);
    }
    {
        dae_structinfo* def = structs + dae_ID_FORMULA_TECHNIQUE_COMMON;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_STRING;
        def->dataoffset = offsetof(dae_formula_technique_common,data);
        def->mindata = 0;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_formula_technique_common);
    }
    {
        dae_structinfo* def = structs + dae_ID_FX_SAMPLER_COMMON;
        static dae_eleminfo elem[13];
        elem[0].name = "instance_image";
        elem[0].dae_type = dae_ID_INSTANCE_IMAGE;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_fx_sampler_common,instance_image);
        elem[1].name = "wrap_s";
        elem[1].dae_type = dae_ID_STRING;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_fx_sampler_common,wrap_s);
        elem[2].name = "wrap_t";
        elem[2].dae_type = dae_ID_STRING;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_fx_sampler_common,wrap_t);
        elem[3].name = "wrap_p";
        elem[3].dae_type = dae_ID_STRING;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_fx_sampler_common,wrap_p);
        elem[4].name = "minfilter";
        elem[4].dae_type = dae_ID_STRING;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_fx_sampler_common,minfilter);
        elem[5].name = "magfilter";
        elem[5].dae_type = dae_ID_STRING;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_fx_sampler_common,magfilter);
        elem[6].name = "mipfilter";
        elem[6].dae_type = dae_ID_STRING;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_fx_sampler_common,mipfilter);
        elem[7].name = "border_color";
        elem[7].dae_type = dae_ID_FLOAT4;
        elem[7].restrict = dae_RESTRICT_OPTIONAL;
        elem[7].offset = offsetof(dae_fx_sampler_common,border_color);
        elem[8].name = "mip_max_level";
        elem[8].dae_type = dae_ID_UINT;
        elem[8].restrict = dae_RESTRICT_OPTIONAL;
        elem[8].offset = offsetof(dae_fx_sampler_common,mip_max_level);
        elem[9].name = "mip_min_level";
        elem[9].dae_type = dae_ID_UINT;
        elem[9].restrict = dae_RESTRICT_OPTIONAL;
        elem[9].offset = offsetof(dae_fx_sampler_common,mip_min_level);
        elem[10].name = "mip_bias";
        elem[10].dae_type = dae_ID_FLOAT;
        elem[10].restrict = dae_RESTRICT_OPTIONAL;
        elem[10].offset = offsetof(dae_fx_sampler_common,mip_bias);
        elem[11].name = "max_anisotropy";
        elem[11].dae_type = dae_ID_UINT;
        elem[11].restrict = dae_RESTRICT_OPTIONAL;
        elem[11].offset = offsetof(dae_fx_sampler_common,max_anisotropy);
        elem[12].name = "extra";
        elem[12].dae_type = dae_ID_EXTRA;
        elem[12].restrict = dae_RESTRICT_LIST;
        elem[12].offset = offsetof(dae_fx_sampler_common,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 13;
        def->size = sizeof(dae_fx_sampler_common);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_IMAGE;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_instance_image);
    }
    {
        dae_structinfo* def = structs + dae_ID_UINT;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_UINT;
        def->dataoffset = offsetof(dae_uint,data);
        def->mindata = 0;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_uint);
    }
    {
        dae_structinfo* def = structs + dae_ID_FX_TECHNIQUE;
        static dae_eleminfo elem[5];
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_fx_technique,asset);
        elem[1].name = "constant";
        elem[1].dae_type = dae_ID_CONSTANT;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_fx_technique,constant);
        elem[2].name = "lambert";
        elem[2].dae_type = dae_ID_LAMBERT;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_fx_technique,lambert);
        elem[3].name = "phong";
        elem[3].dae_type = dae_ID_PHONG;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_fx_technique,phong);
        elem[4].name = "blinn";
        elem[4].dae_type = dae_ID_BLINN;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_fx_technique,blinn);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 5;
        def->size = sizeof(dae_fx_technique);
    }
    {
        dae_structinfo* def = structs + dae_ID_LAMBERT;
        static dae_eleminfo elem[8];
        elem[0].name = "emission";
        elem[0].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_lambert,emission);
        elem[1].name = "ambient";
        elem[1].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_lambert,ambient);
        elem[2].name = "diffuse";
        elem[2].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_lambert,diffuse);
        elem[3].name = "reflective";
        elem[3].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_lambert,reflective);
        elem[4].name = "reflectivity";
        elem[4].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_lambert,reflectivity);
        elem[5].name = "transparent";
        elem[5].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_lambert,transparent);
        elem[6].name = "transparency";
        elem[6].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_lambert,transparency);
        elem[7].name = "index_of_refraction";
        elem[7].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[7].restrict = dae_RESTRICT_OPTIONAL;
        elem[7].offset = offsetof(dae_lambert,index_of_refraction);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 8;
        def->size = sizeof(dae_lambert);
    }
    {
        dae_structinfo* def = structs + dae_ID_PHONG;
        static dae_eleminfo elem[10];
        elem[0].name = "emission";
        elem[0].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_phong,emission);
        elem[1].name = "ambient";
        elem[1].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_phong,ambient);
        elem[2].name = "diffuse";
        elem[2].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_phong,diffuse);
        elem[3].name = "specular";
        elem[3].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_phong,specular);
        elem[4].name = "shininess";
        elem[4].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_phong,shininess);
        elem[5].name = "reflective";
        elem[5].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_phong,reflective);
        elem[6].name = "reflectivity";
        elem[6].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[6].restrict = dae_RESTRICT_OPTIONAL;
        elem[6].offset = offsetof(dae_phong,reflectivity);
        elem[7].name = "transparent";
        elem[7].dae_type = dae_ID_COLOR_OR_TEXTURE;
        elem[7].restrict = dae_RESTRICT_OPTIONAL;
        elem[7].offset = offsetof(dae_phong,transparent);
        elem[8].name = "transparency";
        elem[8].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[8].restrict = dae_RESTRICT_OPTIONAL;
        elem[8].offset = offsetof(dae_phong,transparency);
        elem[9].name = "index_of_refraction";
        elem[9].dae_type = dae_ID_FLOAT_OR_PARAM;
        elem[9].restrict = dae_RESTRICT_OPTIONAL;
        elem[9].offset = offsetof(dae_phong,index_of_refraction);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 10;
        def->size = sizeof(dae_phong);
    }
    {
        dae_structinfo* def = structs + dae_ID_GEOGRAPHIC_LOCATION_ALTITUDE;
        static dae_attrinfo attr[1];
        attr[0].name = "mode";
        attr[0].offset = offsetof(dae_geographic_location_altitude,mode);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_geographic_location_altitude,data);
        def->mindata = 1;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_geographic_location_altitude);
    }
    {
        dae_structinfo* def = structs + dae_ID_GEOMETRY;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[6];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_geometry,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_geometry,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_geometry,asset);
        elem[1].name = "convex_mesh";
        elem[1].dae_type = dae_ID_CONVEX_MESH;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_geometry,convex_mesh);
        elem[2].name = "mesh";
        elem[2].dae_type = dae_ID_MESH;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_geometry,mesh);
        elem[3].name = "spline";
        elem[3].dae_type = dae_ID_SPLINE;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_geometry,spline);
        elem[4].name = "brep";
        elem[4].dae_type = dae_ID_BREP;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_geometry,brep);
        elem[5].name = "extra";
        elem[5].dae_type = dae_ID_EXTRA;
        elem[5].restrict = dae_RESTRICT_LIST;
        elem[5].offset = offsetof(dae_geometry,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 6;
        def->size = sizeof(dae_geometry);
    }
    {
        dae_structinfo* def = structs + dae_ID_CONVEX_MESH;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_convex_mesh);
    }
    {
        dae_structinfo* def = structs + dae_ID_MESH;
        static dae_eleminfo elem[10];
        elem[0].name = "source";
        elem[0].dae_type = dae_ID_SOURCE;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_mesh,source);
        elem[1].name = "vertices";
        elem[1].dae_type = dae_ID_VERTICES;
        elem[1].restrict = dae_RESTRICT_REQUIRED;
        elem[1].offset = offsetof(dae_mesh,vertices);
        elem[2].name = "lines";
        elem[2].dae_type = dae_ID_LINES;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_mesh,lines);
        elem[3].name = "linestrips";
        elem[3].dae_type = dae_ID_LINESTRIPS;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_mesh,linestrips);
        elem[4].name = "polygons";
        elem[4].dae_type = dae_ID_POLYGONS;
        elem[4].restrict = dae_RESTRICT_LIST;
        elem[4].offset = offsetof(dae_mesh,polygons);
        elem[5].name = "polylist";
        elem[5].dae_type = dae_ID_POLYLIST;
        elem[5].restrict = dae_RESTRICT_LIST;
        elem[5].offset = offsetof(dae_mesh,polylist);
        elem[6].name = "triangles";
        elem[6].dae_type = dae_ID_TRIANGLES;
        elem[6].restrict = dae_RESTRICT_LIST;
        elem[6].offset = offsetof(dae_mesh,triangles);
        elem[7].name = "trifans";
        elem[7].dae_type = dae_ID_TRIFANS;
        elem[7].restrict = dae_RESTRICT_LIST;
        elem[7].offset = offsetof(dae_mesh,trifans);
        elem[8].name = "tristrips";
        elem[8].dae_type = dae_ID_TRISTRIPS;
        elem[8].restrict = dae_RESTRICT_LIST;
        elem[8].offset = offsetof(dae_mesh,tristrips);
        elem[9].name = "extra";
        elem[9].dae_type = dae_ID_EXTRA;
        elem[9].restrict = dae_RESTRICT_LIST;
        elem[9].offset = offsetof(dae_mesh,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 10;
        def->size = sizeof(dae_mesh);
    }
    {
        dae_structinfo* def = structs + dae_ID_SPLINE;
        static dae_attrinfo attr[1];
        static dae_eleminfo elem[3];
        attr[0].name = "closed";
        attr[0].offset = offsetof(dae_spline,closed);
        attr[0].datatype = dae_DATA_BOOL;
        attr[0].defaultval.bool_val = 0;
        attr[0].datatype = dae_DATA_FLOAT;
        attr[0].defaultval.float_val = 0.000000;
        attr[0].datatype = dae_DATA_INT;
        attr[0].defaultval.int_val = 0;
        elem[0].name = "source";
        elem[0].dae_type = dae_ID_SOURCE;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_spline,source);
        elem[1].name = "control_vertices";
        elem[1].dae_type = dae_ID_CONTROL_VERTICES;
        elem[1].restrict = dae_RESTRICT_REQUIRED;
        elem[1].offset = offsetof(dae_spline,control_vertices);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_spline,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 3;
        def->size = sizeof(dae_spline);
    }
    {
        dae_structinfo* def = structs + dae_ID_BREP;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_brep);
    }
    {
        dae_structinfo* def = structs + dae_ID_IDREF_ARRAY;
        static dae_attrinfo attr[3];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_IDREF_array,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "id";
        attr[1].offset = offsetof(dae_IDREF_array,id);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "name";
        attr[2].offset = offsetof(dae_IDREF_array,name);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_STRING;
        def->dataoffset = offsetof(dae_IDREF_array,data);
        def->mindata = 0;
        def->maxdata = -1;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 0;
        def->size = sizeof(dae_IDREF_array);
    }
    {
        dae_structinfo* def = structs + dae_ID_IMAGE;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[7];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_image,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "sid";
        attr[1].offset = offsetof(dae_image,sid);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "name";
        attr[2].offset = offsetof(dae_image,name);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_image,asset);
        elem[1].name = "renderable";
        elem[1].dae_type = dae_ID_RENDERABLE;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_image,renderable);
        elem[2].name = "init_from";
        elem[2].dae_type = dae_ID_IMAGE_INIT_FROM;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_image,init_from);
        elem[3].name = "create_2d";
        elem[3].dae_type = dae_ID_CREATE_2D;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_image,create_2d);
        elem[4].name = "create_3d";
        elem[4].dae_type = dae_ID_CREATE_3D;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_image,create_3d);
        elem[5].name = "create_cube";
        elem[5].dae_type = dae_ID_CREATE_CUBE;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_image,create_cube);
        elem[6].name = "extra";
        elem[6].dae_type = dae_ID_EXTRA;
        elem[6].restrict = dae_RESTRICT_LIST;
        elem[6].offset = offsetof(dae_image,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 7;
        def->size = sizeof(dae_image);
    }
    {
        dae_structinfo* def = structs + dae_ID_RENDERABLE;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_renderable);
    }
    {
        dae_structinfo* def = structs + dae_ID_IMAGE_INIT_FROM;
        static dae_attrinfo attr[1];
        static dae_eleminfo elem[2];
        attr[0].name = "mips_gen_erate";
        attr[0].offset = offsetof(dae_image_init_from,mips_gen_erate);
        attr[0].datatype = dae_DATA_BOOL;
        attr[0].defaultval.bool_val = 0;
        attr[0].datatype = dae_DATA_FLOAT;
        attr[0].defaultval.float_val = 0.000000;
        attr[0].datatype = dae_DATA_INT;
        attr[0].defaultval.int_val = 0;
        elem[0].name = "ref";
        elem[0].dae_type = dae_ID_STRING;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_image_init_from,ref);
        elem[1].name = "hex";
        elem[1].dae_type = dae_ID_IMAGE_HEX;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_image_init_from,hex);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_STRING;
        def->dataoffset = offsetof(dae_image_init_from,data);
        def->mindata = 0;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 2;
        def->size = sizeof(dae_image_init_from);
    }
    {
        dae_structinfo* def = structs + dae_ID_CREATE_2D;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_create_2d);
    }
    {
        dae_structinfo* def = structs + dae_ID_CREATE_3D;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_create_3d);
    }
    {
        dae_structinfo* def = structs + dae_ID_CREATE_CUBE;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_create_cube);
    }
    {
        dae_structinfo* def = structs + dae_ID_IMAGE_RENDERABLE;
        static dae_attrinfo attr[1];
        attr[0].name = "share";
        attr[0].offset = offsetof(dae_image_renderable,share);
        attr[0].datatype = dae_DATA_BOOL;
        attr[0].defaultval.bool_val = 0;
        attr[0].datatype = dae_DATA_FLOAT;
        attr[0].defaultval.float_val = 0.000000;
        attr[0].datatype = dae_DATA_INT;
        attr[0].defaultval.int_val = 0;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_image_renderable);
    }
    {
        dae_structinfo* def = structs + dae_ID_IMAGE_HEX;
        static dae_attrinfo attr[1];
        attr[0].name = "format";
        attr[0].offset = offsetof(dae_image_hex,format);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_STRING;
        def->dataoffset = offsetof(dae_image_hex,data);
        def->mindata = 0;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_image_hex);
    }
    {
        dae_structinfo* def = structs + dae_ID_INPUT_SHARED;
        static dae_attrinfo attr[4];
        attr[0].name = "offset";
        attr[0].offset = offsetof(dae_input_shared,offset);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "semantic";
        attr[1].offset = offsetof(dae_input_shared,semantic);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "source";
        attr[2].offset = offsetof(dae_input_shared,source);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        attr[3].name = "set";
        attr[3].offset = offsetof(dae_input_shared,set);
        attr[3].datatype = dae_DATA_UINT;
        attr[3].defaultval.uint_val = 0;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 4;
        def->numelems = 0;
        def->size = sizeof(dae_input_shared);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_CAMERA;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_camera,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_camera,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_camera,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "extra";
        elem[0].dae_type = dae_ID_EXTRA;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_instance_camera,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 1;
        def->size = sizeof(dae_instance_camera);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_CONTROLLER;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[3];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_controller,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_controller,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_controller,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "skeleton";
        elem[0].dae_type = dae_ID_SKELETON;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_instance_controller,skeleton);
        elem[1].name = "bind_material";
        elem[1].dae_type = dae_ID_BIND_MATERIAL;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_instance_controller,bind_material);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_instance_controller,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 3;
        def->size = sizeof(dae_instance_controller);
    }
    {
        dae_structinfo* def = structs + dae_ID_SKELETON;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_STRING;
        def->dataoffset = offsetof(dae_skeleton,data);
        def->mindata = 0;
        def->maxdata = 1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_skeleton);
    }
    {
        dae_structinfo* def = structs + dae_ID_BIND_MATERIAL;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_bind_material);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_EFFECT;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[3];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_effect,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_effect,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_effect,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "technique_hint";
        elem[0].dae_type = dae_ID_TECHNIQUE_HINT;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_instance_effect,technique_hint);
        elem[1].name = "setparam";
        elem[1].dae_type = dae_ID_SETPARAM;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_instance_effect,setparam);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_instance_effect,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 3;
        def->size = sizeof(dae_instance_effect);
    }
    {
        dae_structinfo* def = structs + dae_ID_TECHNIQUE_HINT;
        static dae_attrinfo attr[3];
        attr[0].name = "platform";
        attr[0].offset = offsetof(dae_technique_hint,platform);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "ref";
        attr[1].offset = offsetof(dae_technique_hint,ref);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "profile";
        attr[2].offset = offsetof(dae_technique_hint,profile);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 0;
        def->size = sizeof(dae_technique_hint);
    }
    {
        dae_structinfo* def = structs + dae_ID_SETPARAM;
        static dae_attrinfo attr[1];
        attr[0].name = "ref";
        attr[0].offset = offsetof(dae_setparam,ref);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_setparam);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_FORMULA;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[2];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_formula,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_formula,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_formula,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "setparam";
        elem[0].dae_type = dae_ID_SETPARAM;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_instance_formula,setparam);
        elem[1].name = "extra";
        elem[1].dae_type = dae_ID_EXTRA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_instance_formula,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 2;
        def->size = sizeof(dae_instance_formula);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_GEOMETRY;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[2];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_geometry,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_geometry,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_geometry,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "bind_material";
        elem[0].dae_type = dae_ID_BIND_MATERIAL;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_instance_geometry,bind_material);
        elem[1].name = "extra";
        elem[1].dae_type = dae_ID_EXTRA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_instance_geometry,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 2;
        def->size = sizeof(dae_instance_geometry);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_LIGHT;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_light,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_light,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_light,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "extra";
        elem[0].dae_type = dae_ID_EXTRA;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_instance_light,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 1;
        def->size = sizeof(dae_instance_light);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_NODE;
        static dae_attrinfo attr[4];
        static dae_eleminfo elem[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_node,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_node,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_node,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        attr[3].name = "proxy";
        attr[3].offset = offsetof(dae_instance_node,proxy);
        attr[3].datatype = dae_DATA_STRING;
        attr[3].defaultval.string_val = NULL;
        elem[0].name = "extra";
        elem[0].dae_type = dae_ID_EXTRA;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_instance_node,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 4;
        def->numelems = 1;
        def->size = sizeof(dae_instance_node);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_VISUAL_SCENE;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_instance_visual_scene,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_instance_visual_scene,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "url";
        attr[2].offset = offsetof(dae_instance_visual_scene,url);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "extra";
        elem[0].dae_type = dae_ID_EXTRA;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_instance_visual_scene,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 1;
        def->size = sizeof(dae_instance_visual_scene);
    }
    {
        dae_structinfo* def = structs + dae_ID_INT_ARRAY;
        static dae_attrinfo attr[5];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_int_array,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "id";
        attr[1].offset = offsetof(dae_int_array,id);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "name";
        attr[2].offset = offsetof(dae_int_array,name);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        attr[3].name = "mininclusive";
        attr[3].offset = offsetof(dae_int_array,mininclusive);
        attr[3].datatype = dae_DATA_INT;
        attr[3].defaultval.int_val = 0;
        attr[4].name = "maxinclusive";
        attr[4].offset = offsetof(dae_int_array,maxinclusive);
        attr[4].datatype = dae_DATA_INT;
        attr[4].defaultval.int_val = 0;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_INT;
        def->dataoffset = offsetof(dae_int_array,data);
        def->mindata = 0;
        def->maxdata = -1;
        def->isunion = 0;
        def->numattrs = 5;
        def->numelems = 0;
        def->size = sizeof(dae_int_array);
    }
    {
        dae_structinfo* def = structs + dae_ID_JOINTS;
        static dae_eleminfo elem[2];
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_UNSHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_joints,input);
        elem[1].name = "extra";
        elem[1].dae_type = dae_ID_EXTRA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_joints,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 2;
        def->size = sizeof(dae_joints);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIBRARY_FORMULAS;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_library_formulas,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_library_formulas,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_library_formulas,asset);
        elem[1].name = "formula";
        elem[1].dae_type = dae_ID_FORMULA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_library_formulas,formula);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_library_formulas,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_library_formulas);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIGHT;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[4];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_light,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_light,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_light,asset);
        elem[1].name = "technique_common";
        elem[1].dae_type = dae_ID_LIGHT_TECHNIQUE_COMMON;
        elem[1].restrict = dae_RESTRICT_REQUIRED;
        elem[1].offset = offsetof(dae_light,technique_common);
        elem[2].name = "technique";
        elem[2].dae_type = dae_ID_TECHNIQUE;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_light,technique);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_light,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 4;
        def->size = sizeof(dae_light);
    }
    {
        dae_structinfo* def = structs + dae_ID_MATERIAL;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[3];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_material,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_material,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_material,asset);
        elem[1].name = "instance_effect";
        elem[1].dae_type = dae_ID_INSTANCE_EFFECT;
        elem[1].restrict = dae_RESTRICT_REQUIRED;
        elem[1].offset = offsetof(dae_material,instance_effect);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_material,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 3;
        def->size = sizeof(dae_material);
    }
    {
        dae_structinfo* def = structs + dae_ID_NODE;
        static dae_attrinfo attr[5];
        static dae_eleminfo elem[9];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_node,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_node,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "sid";
        attr[2].offset = offsetof(dae_node,sid);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        attr[3].name = "type";
        attr[3].offset = offsetof(dae_node,type);
        attr[3].datatype = dae_DATA_STRING;
        attr[3].defaultval.string_val = NULL;
        attr[4].name = "layer";
        attr[4].offset = offsetof(dae_node,layer);
        attr[4].datatype = dae_DATA_STRING;
        attr[4].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_node,asset);
        elem[1].name = "transformation_element";
        elem[1].dae_type = dae_ID_TRANSFORMATION_ELEMENT;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_node,transformation_element);
        elem[2].name = "instance_camera";
        elem[2].dae_type = dae_ID_INSTANCE_CAMERA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_node,instance_camera);
        elem[3].name = "instance_controller";
        elem[3].dae_type = dae_ID_INSTANCE_CONTROLLER;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_node,instance_controller);
        elem[4].name = "instance_geometry";
        elem[4].dae_type = dae_ID_INSTANCE_GEOMETRY;
        elem[4].restrict = dae_RESTRICT_LIST;
        elem[4].offset = offsetof(dae_node,instance_geometry);
        elem[5].name = "instance_light";
        elem[5].dae_type = dae_ID_INSTANCE_LIGHT;
        elem[5].restrict = dae_RESTRICT_LIST;
        elem[5].offset = offsetof(dae_node,instance_light);
        elem[6].name = "instance_node";
        elem[6].dae_type = dae_ID_INSTANCE_NODE;
        elem[6].restrict = dae_RESTRICT_LIST;
        elem[6].offset = offsetof(dae_node,instance_node);
        elem[7].name = "node";
        elem[7].dae_type = dae_ID_NODE;
        elem[7].restrict = dae_RESTRICT_LIST;
        elem[7].offset = offsetof(dae_node,node);
        elem[8].name = "extra";
        elem[8].dae_type = dae_ID_EXTRA;
        elem[8].restrict = dae_RESTRICT_LIST;
        elem[8].offset = offsetof(dae_node,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 5;
        def->numelems = 9;
        def->size = sizeof(dae_node);
    }
    {
        dae_structinfo* def = structs + dae_ID_VISUAL_SCENE;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[4];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_visual_scene,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_visual_scene,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "asset";
        elem[0].dae_type = dae_ID_ASSET;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_visual_scene,asset);
        elem[1].name = "node";
        elem[1].dae_type = dae_ID_NODE;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_visual_scene,node);
        elem[2].name = "evaluate_scene";
        elem[2].dae_type = dae_ID_EVALUATE_SCENE;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_visual_scene,evaluate_scene);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_visual_scene,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 4;
        def->size = sizeof(dae_visual_scene);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIGHT_TECHNIQUE_COMMON;
        static dae_eleminfo elem[4];
        elem[0].name = "ambient";
        elem[0].dae_type = dae_ID_AMBIENT;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_light_technique_common,ambient);
        elem[1].name = "directional";
        elem[1].dae_type = dae_ID_DIRECTIONAL;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_light_technique_common,directional);
        elem[2].name = "point";
        elem[2].dae_type = dae_ID_POINT;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_light_technique_common,point);
        elem[3].name = "spot";
        elem[3].dae_type = dae_ID_SPOT;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_light_technique_common,spot);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 4;
        def->size = sizeof(dae_light_technique_common);
    }
    {
        dae_structinfo* def = structs + dae_ID_POINT;
        static dae_eleminfo elem[4];
        elem[0].name = "color";
        elem[0].dae_type = dae_ID_TARGETABLE_FLOAT3;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_point,color);
        elem[1].name = "constant_attenuation";
        elem[1].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_point,constant_attenuation);
        elem[2].name = "linear_attenuation";
        elem[2].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_point,linear_attenuation);
        elem[3].name = "quadratic_attenuation";
        elem[3].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_point,quadratic_attenuation);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 4;
        def->size = sizeof(dae_point);
    }
    {
        dae_structinfo* def = structs + dae_ID_SPOT;
        static dae_eleminfo elem[6];
        elem[0].name = "color";
        elem[0].dae_type = dae_ID_TARGETABLE_FLOAT3;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_spot,color);
        elem[1].name = "constant_attenuation";
        elem[1].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_spot,constant_attenuation);
        elem[2].name = "linear_attenuation";
        elem[2].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_spot,linear_attenuation);
        elem[3].name = "quadratic_attenuation";
        elem[3].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_spot,quadratic_attenuation);
        elem[4].name = "falloff_angle";
        elem[4].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[4].restrict = dae_RESTRICT_OPTIONAL;
        elem[4].offset = offsetof(dae_spot,falloff_angle);
        elem[5].name = "falloff_exponent";
        elem[5].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[5].restrict = dae_RESTRICT_OPTIONAL;
        elem[5].offset = offsetof(dae_spot,falloff_exponent);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 6;
        def->size = sizeof(dae_spot);
    }
    {
        dae_structinfo* def = structs + dae_ID_LINES;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[3];
        attr[0].name = "name";
        attr[0].offset = offsetof(dae_lines,name);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "count";
        attr[1].offset = offsetof(dae_lines,count);
        attr[1].datatype = dae_DATA_UINT;
        attr[1].defaultval.uint_val = 0;
        attr[2].name = "material";
        attr[2].offset = offsetof(dae_lines,material);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_lines,input);
        elem[1].name = "p";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_lines,p);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_lines,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 3;
        def->size = sizeof(dae_lines);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIST_OF_UINTS;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_UINT;
        def->dataoffset = offsetof(dae_list_of_uints,data);
        def->mindata = 0;
        def->maxdata = -1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_list_of_uints);
    }
    {
        dae_structinfo* def = structs + dae_ID_LINESTRIPS;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[3];
        attr[0].name = "name";
        attr[0].offset = offsetof(dae_linestrips,name);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "count";
        attr[1].offset = offsetof(dae_linestrips,count);
        attr[1].datatype = dae_DATA_UINT;
        attr[1].defaultval.uint_val = 0;
        attr[2].name = "material";
        attr[2].offset = offsetof(dae_linestrips,material);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_linestrips,input);
        elem[1].name = "p";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_linestrips,p);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_linestrips,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 3;
        def->size = sizeof(dae_linestrips);
    }
    {
        dae_structinfo* def = structs + dae_ID_LIST_OF_INTS;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_INT;
        def->dataoffset = offsetof(dae_list_of_ints,data);
        def->mindata = 0;
        def->maxdata = -1;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_list_of_ints);
    }
    {
        dae_structinfo* def = structs + dae_ID_VERTICES;
        static dae_attrinfo attr[2];
        static dae_eleminfo elem[2];
        attr[0].name = "id";
        attr[0].offset = offsetof(dae_vertices,id);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "name";
        attr[1].offset = offsetof(dae_vertices,name);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_UNSHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_vertices,input);
        elem[1].name = "extra";
        elem[1].dae_type = dae_ID_EXTRA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_vertices,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 2;
        def->numelems = 2;
        def->size = sizeof(dae_vertices);
    }
    {
        dae_structinfo* def = structs + dae_ID_POLYGONS;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[4];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_polygons,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "material";
        attr[1].offset = offsetof(dae_polygons,material);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "name";
        attr[2].offset = offsetof(dae_polygons,name);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_polygons,input);
        elem[1].name = "p";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_polygons,p);
        elem[2].name = "ph";
        elem[2].dae_type = dae_ID_PH;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_polygons,ph);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_polygons,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 4;
        def->size = sizeof(dae_polygons);
    }
    {
        dae_structinfo* def = structs + dae_ID_POLYLIST;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[4];
        attr[0].name = "name";
        attr[0].offset = offsetof(dae_polylist,name);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "count";
        attr[1].offset = offsetof(dae_polylist,count);
        attr[1].datatype = dae_DATA_UINT;
        attr[1].defaultval.uint_val = 0;
        attr[2].name = "material";
        attr[2].offset = offsetof(dae_polylist,material);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_polylist,input);
        elem[1].name = "vcount";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_polylist,vcount);
        elem[2].name = "p";
        elem[2].dae_type = dae_ID_LIST_OF_UINTS;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_polylist,p);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_polylist,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 4;
        def->size = sizeof(dae_polylist);
    }
    {
        dae_structinfo* def = structs + dae_ID_TRIANGLES;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[3];
        attr[0].name = "name";
        attr[0].offset = offsetof(dae_triangles,name);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "count";
        attr[1].offset = offsetof(dae_triangles,count);
        attr[1].datatype = dae_DATA_UINT;
        attr[1].defaultval.uint_val = 0;
        attr[2].name = "material";
        attr[2].offset = offsetof(dae_triangles,material);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_triangles,input);
        elem[1].name = "p";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_triangles,p);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_triangles,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 3;
        def->size = sizeof(dae_triangles);
    }
    {
        dae_structinfo* def = structs + dae_ID_TRIFANS;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[3];
        attr[0].name = "name";
        attr[0].offset = offsetof(dae_trifans,name);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "count";
        attr[1].offset = offsetof(dae_trifans,count);
        attr[1].datatype = dae_DATA_UINT;
        attr[1].defaultval.uint_val = 0;
        attr[2].name = "material";
        attr[2].offset = offsetof(dae_trifans,material);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_trifans,input);
        elem[1].name = "p";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_trifans,p);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_trifans,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 3;
        def->size = sizeof(dae_trifans);
    }
    {
        dae_structinfo* def = structs + dae_ID_TRISTRIPS;
        static dae_attrinfo attr[3];
        static dae_eleminfo elem[3];
        attr[0].name = "name";
        attr[0].offset = offsetof(dae_tristrips,name);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        attr[1].name = "count";
        attr[1].offset = offsetof(dae_tristrips,count);
        attr[1].datatype = dae_DATA_UINT;
        attr[1].defaultval.uint_val = 0;
        attr[2].name = "material";
        attr[2].offset = offsetof(dae_tristrips,material);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_tristrips,input);
        elem[1].name = "p";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_tristrips,p);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_tristrips,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 3;
        def->size = sizeof(dae_tristrips);
    }
    {
        dae_structinfo* def = structs + dae_ID_TARGETS;
        static dae_eleminfo elem[2];
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_UNSHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_targets,input);
        elem[1].name = "extra";
        elem[1].dae_type = dae_ID_EXTRA;
        elem[1].restrict = dae_RESTRICT_LIST;
        elem[1].offset = offsetof(dae_targets,extra);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 2;
        def->size = sizeof(dae_targets);
    }
    {
        dae_structinfo* def = structs + dae_ID_NAME_ARRAY;
        static dae_attrinfo attr[3];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_Name_array,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "id";
        attr[1].offset = offsetof(dae_Name_array,id);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "name";
        attr[2].offset = offsetof(dae_Name_array,name);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_STRING;
        def->dataoffset = offsetof(dae_Name_array,data);
        def->mindata = 0;
        def->maxdata = -1;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 0;
        def->size = sizeof(dae_Name_array);
    }
    {
        dae_structinfo* def = structs + dae_ID_OPTICS_TECHNIQUE_COMMON;
        static dae_eleminfo elem[2];
        elem[0].name = "orthographic";
        elem[0].dae_type = dae_ID_ORTHOGRAPHIC;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_optics_technique_common,orthographic);
        elem[1].name = "perspective";
        elem[1].dae_type = dae_ID_PERSPECTIVE;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_optics_technique_common,perspective);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 2;
        def->size = sizeof(dae_optics_technique_common);
    }
    {
        dae_structinfo* def = structs + dae_ID_ORTHOGRAPHIC;
        static dae_eleminfo elem[6];
        elem[0].name = "xmag";
        elem[0].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_orthographic,xmag);
        elem[1].name = "ymag";
        elem[1].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_orthographic,ymag);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_orthographic,extra);
        elem[3].name = "aspect_ratio";
        elem[3].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_orthographic,aspect_ratio);
        elem[4].name = "znear";
        elem[4].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[4].restrict = dae_RESTRICT_REQUIRED;
        elem[4].offset = offsetof(dae_orthographic,znear);
        elem[5].name = "zfar";
        elem[5].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[5].restrict = dae_RESTRICT_REQUIRED;
        elem[5].offset = offsetof(dae_orthographic,zfar);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 6;
        def->size = sizeof(dae_orthographic);
    }
    {
        dae_structinfo* def = structs + dae_ID_PERSPECTIVE;
        static dae_eleminfo elem[6];
        elem[0].name = "xfov";
        elem[0].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_perspective,xfov);
        elem[1].name = "yfov";
        elem[1].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_perspective,yfov);
        elem[2].name = "extra";
        elem[2].dae_type = dae_ID_EXTRA;
        elem[2].restrict = dae_RESTRICT_LIST;
        elem[2].offset = offsetof(dae_perspective,extra);
        elem[3].name = "aspect_ratio";
        elem[3].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[3].restrict = dae_RESTRICT_OPTIONAL;
        elem[3].offset = offsetof(dae_perspective,aspect_ratio);
        elem[4].name = "znear";
        elem[4].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[4].restrict = dae_RESTRICT_REQUIRED;
        elem[4].offset = offsetof(dae_perspective,znear);
        elem[5].name = "zfar";
        elem[5].dae_type = dae_ID_TARGETABLE_FLOAT;
        elem[5].restrict = dae_RESTRICT_REQUIRED;
        elem[5].offset = offsetof(dae_perspective,zfar);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 6;
        def->size = sizeof(dae_perspective);
    }
    {
        dae_structinfo* def = structs + dae_ID_PH;
        static dae_eleminfo elem[2];
        elem[0].name = "p";
        elem[0].dae_type = dae_ID_LIST_OF_UINTS;
        elem[0].restrict = dae_RESTRICT_OPTIONAL;
        elem[0].offset = offsetof(dae_ph,p);
        elem[1].name = "h";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_ph,h);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 2;
        def->size = sizeof(dae_ph);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_PHYSICS_SCENE;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_instance_physics_scene);
    }
    {
        dae_structinfo* def = structs + dae_ID_INSTANCE_KINEMATICS_SCENE;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_instance_kinematics_scene);
    }
    {
        dae_structinfo* def = structs + dae_ID_SIDREF_ARRAY;
        static dae_attrinfo attr[3];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_SIDREF_array,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        attr[1].name = "id";
        attr[1].offset = offsetof(dae_SIDREF_array,id);
        attr[1].datatype = dae_DATA_STRING;
        attr[1].defaultval.string_val = NULL;
        attr[2].name = "name";
        attr[2].offset = offsetof(dae_SIDREF_array,name);
        attr[2].datatype = dae_DATA_STRING;
        attr[2].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_STRING;
        def->dataoffset = offsetof(dae_SIDREF_array,data);
        def->mindata = 0;
        def->maxdata = -1;
        def->isunion = 0;
        def->numattrs = 3;
        def->numelems = 0;
        def->size = sizeof(dae_SIDREF_array);
    }
    {
        dae_structinfo* def = structs + dae_ID_VERTEX_WEIGHTS;
        static dae_attrinfo attr[1];
        static dae_eleminfo elem[4];
        attr[0].name = "count";
        attr[0].offset = offsetof(dae_vertex_weights,count);
        attr[0].datatype = dae_DATA_UINT;
        attr[0].defaultval.uint_val = 0;
        elem[0].name = "input";
        elem[0].dae_type = dae_ID_INPUT_SHARED;
        elem[0].restrict = dae_RESTRICT_LIST;
        elem[0].offset = offsetof(dae_vertex_weights,input);
        elem[1].name = "vcount";
        elem[1].dae_type = dae_ID_LIST_OF_UINTS;
        elem[1].restrict = dae_RESTRICT_OPTIONAL;
        elem[1].offset = offsetof(dae_vertex_weights,vcount);
        elem[2].name = "v";
        elem[2].dae_type = dae_ID_LIST_OF_INTS;
        elem[2].restrict = dae_RESTRICT_OPTIONAL;
        elem[2].offset = offsetof(dae_vertex_weights,v);
        elem[3].name = "extra";
        elem[3].dae_type = dae_ID_EXTRA;
        elem[3].restrict = dae_RESTRICT_LIST;
        elem[3].offset = offsetof(dae_vertex_weights,extra);
        def->attrs = attr;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 4;
        def->size = sizeof(dae_vertex_weights);
    }
    {
        dae_structinfo* def = structs + dae_ID_TOKEN_ARRAY;
        def->attrs = NULL;
        def->elems = NULL;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 0;
        def->size = sizeof(dae_token_array);
    }
    {
        dae_structinfo* def = structs + dae_ID_SOURCE_TECHNIQUE_COMMON;
        static dae_eleminfo elem[1];
        elem[0].name = "accessor";
        elem[0].dae_type = dae_ID_ACCESSOR;
        elem[0].restrict = dae_RESTRICT_REQUIRED;
        elem[0].offset = offsetof(dae_source_technique_common,accessor);
        def->attrs = NULL;
        def->elems = elem;
        def->datatype = dae_DATA_NONE;
        def->dataoffset = -1;
        def->mindata = 0;
        def->maxdata = 0;
        def->isunion = 0;
        def->numattrs = 0;
        def->numelems = 1;
        def->size = sizeof(dae_source_technique_common);
    }
    {
        dae_structinfo* def = structs + dae_ID_TARGETABLE_FLOAT4;
        static dae_attrinfo attr[1];
        attr[0].name = "sid";
        attr[0].offset = offsetof(dae_targetable_float4,sid);
        attr[0].datatype = dae_DATA_STRING;
        attr[0].defaultval.string_val = NULL;
        def->attrs = attr;
        def->elems = NULL;
        def->datatype = dae_DATA_FLOAT;
        def->dataoffset = offsetof(dae_targetable_float4,data);
        def->mindata = 4;
        def->maxdata = 4;
        def->isunion = 0;
        def->numattrs = 1;
        def->numelems = 0;
        def->size = sizeof(dae_targetable_float4);
    }
    schema.structs = structs;
    schema.numstructs = sizeof(structs)/sizeof(*structs);
    return &schema;
}


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
            if(!strcmp(itemval, attval))
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
            //TODO: case dae_DATA_HEX:
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

