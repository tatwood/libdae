/**
 * @brief     libdae utility source
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#include <daeu.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct daeu_xml_parser_s
{
    dae_COLLADA* root;
    dae_obj_ptr current;
    struct
    {
        char* str;
        size_t len;
        size_t cap;
    } chardata;
};

//****************************************************************************
static void daeu_cross3(
    const float* a,
    const float* b,
    float* out)
{
    out[0] = a[1]*b[2]-a[2]*b[1];
    out[1] = a[2]*b[0]-a[0]*b[2];
    out[2] = a[0]*b[1]-a[1]*b[0];
}

//****************************************************************************
static void daeu_normalize4(
    const float* a,
    float* out)
{
    float d = sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2] + a[3]*a[3]);
    if(d > 0.000001f)
    {
        d = 1.0f/d;
        out[0] = a[0]*d; out[1] = a[1]*d; out[2] = a[2]*d; out[3] = a[3]*d;
    }
}

//****************************************************************************
static void daeu_matrix_identity(
    float* out)
{
    out[ 0] = 1.0f; out[ 1] = 0.0f; out[ 2] = 0.0f; out[ 3] = 0.0f;
    out[ 4] = 0.0f; out[ 5] = 1.0f; out[ 6] = 0.0f; out[ 7] = 0.0f;
    out[ 8] = 0.0f; out[ 9] = 0.0f; out[10] = 1.0f; out[11] = 0.0f;
    out[12] = 0.0f; out[13] = 0.0f; out[14] = 0.0f; out[15] = 1.0f;
}


//****************************************************************************
static void daeu_matrix_multiply(
    const float* a,
    const float* b,
    float* out)
{
#define daeu_MATRIXMUL_COMP(r_, c_) \
    out[(r_)*4 + (c_)] = \
        a[(r_)*4+0]*b[0*4+(c_)] + \
        a[(r_)*4+1]*b[1*4+(c_)] + \
        a[(r_)*4+2]*b[2*4+(c_)] + \
        a[(r_)*4+3]*b[3*4+(c_)]
    daeu_MATRIXMUL_COMP(0,0);
    daeu_MATRIXMUL_COMP(0,1);
    daeu_MATRIXMUL_COMP(0,2);
    daeu_MATRIXMUL_COMP(0,3);
    daeu_MATRIXMUL_COMP(1,0);
    daeu_MATRIXMUL_COMP(1,1);
    daeu_MATRIXMUL_COMP(1,2);
    daeu_MATRIXMUL_COMP(1,3);
    daeu_MATRIXMUL_COMP(2,0);
    daeu_MATRIXMUL_COMP(2,1);
    daeu_MATRIXMUL_COMP(2,2);
    daeu_MATRIXMUL_COMP(2,3);
    daeu_MATRIXMUL_COMP(3,0);
    daeu_MATRIXMUL_COMP(3,1);
    daeu_MATRIXMUL_COMP(3,2);
    daeu_MATRIXMUL_COMP(3,3);
#undef daeu_MATRIXMUL_COMP
}

//****************************************************************************
void daeu_lookat_to_matrix(
    const dae_lookat_type* lookat,
    float* mtx_out)
{
    const float* vp  = lookat->data + 0;
    const float* vi  = lookat->data + 3;
    const float* vup = lookat->data + 6;
    float mtxrot[16];
    float mtxtrans[16];
    float* s = mtxrot + 0;
    float* u = mtxrot + 4;
    float* f = mtxrot + 8;
    // calculate rotation
    daeu_matrix_identity(mtxrot);
    f[0]=vp[0]-vi[0]; f[1]=vp[1]-vi[1]; f[2]=vp[2]-vi[2]; f[3]=0.0f;
    daeu_normalize4(f, f);
    daeu_cross3(vup, f, s);
    s[3] = 0.0f;
    daeu_normalize4(s, s);
    daeu_cross3(f, s, u);
    u[3] = 0.0f;
    daeu_normalize4(u, u);
    // calculate translation
    daeu_matrix_identity(mtxtrans);
    mtxtrans[3] = -vp[0]; mtxtrans[7] = -vp[1]; mtxtrans[11] = -vp[2];
    daeu_matrix_multiply(mtxrot, mtxtrans, mtx_out);
}

//****************************************************************************
void daeu_rotate_to_matrix(
    const dae_rotate_type* rotate,
    float* mtx_out)
{
    const float* axis = rotate->data;
    float rad = rotate->data[3] * (3.14159265f/180.0f);
    float c = cosf(rad);
    float s = sinf(rad);
    mtx_out[ 0] = axis[0]*axis[0]*(1.0f-c) + c;
    mtx_out[ 1] = axis[0]*axis[1]*(1.0f-c) - axis[2]*s;
    mtx_out[ 2] = axis[0]*axis[2]*(1.0f-c) + axis[1]*s;
    mtx_out[ 3] = 0.0f;
    mtx_out[ 4] = axis[1]*axis[0]*(1.0f-c) + axis[2]*s;
    mtx_out[ 5] = axis[1]*axis[1]*(1.0f-c) + c;
    mtx_out[ 6] = axis[1]*axis[2]*(1.0f-c) - axis[0]*s;
    mtx_out[ 7] = 0.0f;
    mtx_out[ 8] = axis[2]*axis[0]*(1.0f-c) - axis[1]*s;
    mtx_out[ 9] = axis[2]*axis[1]*(1.0f-c) + axis[0]*s;
    mtx_out[10] = axis[2]*axis[2]*(1.0f-c) + c;
    mtx_out[11] = 0.0f;
    mtx_out[12] = 0.0f;
    mtx_out[13] = 0.0f;
    mtx_out[14] = 0.0f;
    mtx_out[15] = 1.0f;
}

//****************************************************************************
void daeu_scale_to_matrix(
    const dae_scale_type* scale,
    float* mtx_out)
{
    daeu_matrix_identity(mtx_out);
    mtx_out[ 0] = scale->data[0];
    mtx_out[ 5] = scale->data[1];
    mtx_out[10] = scale->data[2];
}

//****************************************************************************
void daeu_translate_to_matrix(
    const dae_translate_type* translate,
    float* mtx_out)
{
    daeu_matrix_identity(mtx_out);
    mtx_out[ 3] = translate->data[0];
    mtx_out[ 7] = translate->data[1];
    mtx_out[11] = translate->data[2];
}

//****************************************************************************
void daeu_calc_transform_matrix(
    dae_obj_ptr obj,
    float* mtx_out)
{
    float mtxa[16];
    float mtxb[16];
    dae_obj_ptr el;

    daeu_matrix_identity(mtx_out);
    el = dae_get_first_element(obj);
    while(el != NULL)
    {
        int istransform = 1;
        dae_obj_typeid eltype = dae_get_typeid(el);
        memcpy(mtxa, mtx_out, sizeof(mtxa));
        daeu_matrix_identity(mtxb);
        switch(eltype)
        {
        case dae_ID_LOOKAT_TYPE:
            daeu_lookat_to_matrix((dae_lookat_type*) el, mtxb);
            break;
        case dae_ID_MATRIX_TYPE:
            memcpy(mtxb, ((dae_matrix_type*) el)->data, sizeof(mtxb));
            break;
        case dae_ID_ROTATE_TYPE:
            daeu_rotate_to_matrix((dae_rotate_type*) el, mtxb);
            break;
        case dae_ID_SCALE_TYPE:
            daeu_scale_to_matrix((dae_scale_type*) el, mtxb);
            break;
        case dae_ID_SKEW_TYPE:
            // TODO support SKEW
            assert(0);
            break;
        case dae_ID_TRANSLATE_TYPE:
            daeu_translate_to_matrix((dae_translate_type*) el, mtxb);
            break;
        default:
            istransform = 0;
            break;
        }
        if(istransform)
        {
            daeu_matrix_multiply(mtxa, mtxb, mtx_out);
        }
        el = dae_get_next(el);
    }
}

//****************************************************************************
dae_obj_ptr daeu_find_attrib(
    dae_obj_ptr obj,
    const char* name)
{
    dae_obj_ptr at = NULL;
    dae_obj_ptr itr = dae_get_first_attrib(obj);
    while(itr != NULL)
    {
        const char* itname = dae_get_name(itr);
        if(!strcmp(itname, name))
        {
            at = itr;
            break;
        }
        itr = dae_get_next(itr);
    }
    return at;
}

//****************************************************************************
int daeu_search_sid(
    dae_obj_ptr searchroot,
    const char* ref,
    dae_obj_ptr* obj_out,
    int* dataindex_out)
{
    const char* sep = "/.(";
    size_t result = 0;
    int isrootless = 0;
    int ismatch = 0;
    if(dataindex_out != NULL)
    {
        *dataindex_out = -1;
    }
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
        const char* atnames[] = { "sid", "id" };
        size_t i;
        for(i = 0; i < sizeof(atnames)/sizeof(*atnames); ++i)
        {
            dae_obj_ptr at = daeu_find_attrib(searchroot, atnames[i]);
            if(at != NULL)
            {
                dae_native_typeid attype;
                void* ataddr;
                size_t atsize;
                if(dae_get_data(at, &attype, &ataddr, &atsize) > 0)
                {
                    // compare the attribute value against the search key
                    const char* atval = *((char**) ataddr);
                    if(attype == dae_NATIVE_STRING && atval != NULL)
                    {
                        const char* end = strpbrk(ref, sep);
                        size_t len;
                        len = (end!=NULL) ? (size_t)(end-ref) : strlen(ref);
                        ismatch = !memcmp(atval,ref,len) && atval[len]=='\0';
                        if(ismatch)
                        {
                            *obj_out = searchroot;
                            isrootless = 0;
                            ref += len;
                            ++result;
                            break;
                        }
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
            dae_native_typeid datatype;
            float* data;
            size_t datalen;
            ++ref;
            if(dae_get_data(searchroot,&datatype,(void**)&data,&datalen) != 0)
            {
                if(datatype == dae_NATIVE_FLOAT && dataindex_out != NULL)
                {
                    int i = -1;
                    switch(*ref)
                    {
                    case 'A':
                        // ANGLE or A (ALPHA)
                        if(!strcmp(ref, "ANGLE") || ref[1] == '\0'){ i = 3; }
                        break;
                    case 'B':
                        // B (BLUE)
                        if(ref[1] == '\0') { i = 2; }
                        break;
                    case 'G':
                        // G (GREEN)
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
                        if(i < (int) datalen)
                        {
                            *dataindex_out = i;
                            ++result;
                        }
                    }
                }
            }
        }
        else if(*ref == '(')
        {
            // referencing a component of the current element by index
            dae_native_typeid datatype;
            float* data;
            size_t datalen;
            ++ref;
            if(dae_get_data(searchroot,&datatype,(void**)&data,&datalen) != 0)
            {
                if(datatype == dae_NATIVE_FLOAT && dataindex_out != NULL)
                {
                    int i = 0;
                    if(sscanf(ref,"%d",&i) == 1)
                    {
                        if(i >= 0)
                        {
                            if(i < (int) datalen)
                            {
                                *dataindex_out = i;
                                ++result;
                            }
                        }
                    }
                }
            }
        }
        else if(*ref == '/' || isrootless)
        {
            // traverse children
            dae_obj_ptr elem = dae_get_first_element(searchroot);
            while(elem != NULL)
            {
                result = daeu_search_sid(elem, ref, obj_out, dataindex_out);
                if(result != 0)
                {
                    break;
                }
                elem = dae_get_next(elem);
            }
        }
    }
    return result;
}

//****************************************************************************
dae_obj_ptr daeu_search_uri(
    dae_obj_ptr searchroot,
    const char* uri)
{
    const char* key = uri + 1;
    dae_obj_ptr result = NULL;
    dae_obj_ptr itr = searchroot;
    assert(*uri == '#');
    do
    {
        dae_obj_ptr at;
        dae_obj_ptr next;
        // attempt to find the id attribute for this object
        at = daeu_find_attrib(itr, "id");
        if(at != NULL)
        {
            // attempt the get the attribute value
            dae_native_typeid attype;
            void* ataddr;
            size_t atsize;
            if(dae_get_data(at, &attype, &ataddr, &atsize))
            {
                const char* atval = *((char**) ataddr);
                if(attype == dae_NATIVE_STRING && atval != NULL)
                {
                    // compare the attribute value against the search key
                    if(!strcmp(atval, key))
                    {
                        // found match
                        result = itr;
                        break;
                    }
                }
            }
        }
        // attempt to traverse children
        next = dae_get_first_element(itr);
        while(next == NULL)
        {
            // if no children exist, attempt to move to next sibling
            next = dae_get_next(itr);
            if(next == NULL)
            {
                // if a sibling did not exist, attempt to move up to the
                // parent's sibling.
                next = dae_get_parent(itr);
                if(next != searchroot)
                {
                    // only move to the parent's sibling if not at the root
                    itr = next;
                    next = dae_get_next(next);
                }
            }
        }
        itr = next;
    }
    while(itr != searchroot);
    return result;
}

//****************************************************************************
void daeu_xml_chardata(
    void *userdata,
     const char *s,
     int len)
{
    daeu_xml_parser parser = (daeu_xml_parser) userdata;
    char* chars = parser->chardata.str;
    size_t off = parser->chardata.len;
    size_t cap = parser->chardata.cap;
    if((off + len + 1) > cap)
    {
        cap = (off + len + 1024) & ~1023;
        chars = (char*) realloc(chars, cap * sizeof(*chars));
        parser->chardata.str = chars;
        parser->chardata.cap = cap;
    }
    memcpy(chars + off, s, len);
    chars[off + len] = '\0';
    parser->chardata.len += len;
}

//****************************************************************************
void daeu_xml_create(
    dae_COLLADA* root,
    daeu_xml_parser* parser_out)
{
    daeu_xml_parser parser = (daeu_xml_parser) malloc(sizeof(*parser));
    memset(parser, 0, sizeof(*parser));
    parser->root = root;
    *parser_out = parser;
}

//****************************************************************************
void daeu_xml_destroy(
    daeu_xml_parser parser)
{
    free(parser->chardata.str);
    free(parser);
}

//****************************************************************************
void daeu_xml_endelement(
    void* userdata,
    const char* el)
{
    daeu_xml_parser parser = (daeu_xml_parser) userdata;
    if(parser->current != NULL)
    {
        if(parser->chardata.len > 0)
        {
            dae_obj_ptr obj = parser->current;
            if(dae_get_typeid(obj) == dae_ID_IMAGE_TYPE_INIT_FROM)
            {
                // this is a hack for compatibility with the maya exporter.
                // it places the init_from character data directly within the
                // init_from instead of within a ref element per the spec
                obj = dae_add_element(obj, "ref");
            }
            dae_set_string(obj, parser->chardata.str);
        }
        parser->current = dae_get_parent(parser->current);
    }
}

//****************************************************************************
void daeu_xml_startelement(
    void* userdata,
    const char* el,
    const char** att)
{
    daeu_xml_parser parser = (daeu_xml_parser) userdata;
    dae_obj_ptr parent = parser->current;
    dae_obj_ptr obj = NULL;
    if(parent == NULL)
    {
        if(!strcmp(el, "COLLADA"))
        {
            obj = parser->root;
        }
    }
    else
    {
        obj = dae_add_element(parent, el);
    }
    if(obj != NULL)
    {
        while(att[0] != NULL && att[1] != NULL)
        {
            dae_add_attrib(obj, att[0], att[1]);
            att += 2;
        }
        parser->current = obj;
    }
    else
    {
        assert(0);
    }
    parser->chardata.len = 0;
}
