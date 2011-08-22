/**
 * @brief     libdae header
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef DAE_DAE_H_
#define DAE_DAE_H_

#include <stddef.h>

typedef enum dae_typeid_e dae_typeid;
typedef enum dae_targettype_e dae_targettype;
typedef struct dae_listbase_s dae_listbase;
typedef struct dae_typebase_s dae_typebase;
typedef void* dae_parser;

//****************************************************************************

enum dae_targettype_e
{
    dae_TARGET_ELEM,
    dae_TARGET_FLOAT
};

/*GENH_BGN*/

enum dae_typeid_e
{
    dae_ID_INVALID = -1,
    dae_ID_COLLADA
};

typedef struct dae_COLLADA_S dae_COLLADA;

struct dae_listbase_s
{
    dae_typeid type;
};

struct dae_typebase_s
{
    dae_typeid type;
};

/*GENH_END*/
//****************************************************************************

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

size_t dae_add(
    dae_typebase* plist,
    const dae_typebase* pval);

void dae_construct(
    dae_typeid type,
    dae_typebase* pout);

void dae_construct_list(
    dae_typeid type,
    dae_listbase* pout);

dae_parser dae_create_parser(
    dae_COLLADA* target);

void dae_destroy_parser(
    dae_parser p);

void dae_destruct(
    dae_typebase* pval);

void dae_destruct_list(
    dae_listbase* pout);

/**
 * performs a iterative, non-recursive search of a list by attribute value
 * @param list list to iterate during the search
 * @param type typeid used to filter results, or dae_ID_INVALID for any type
 * @param attname name of the attribute to compare against
 * @param attval value to search for
 * @return pointer to the result or NULL
 */
dae_typebase* dae_find_in_list(
    dae_listbase* list,
    dae_typeid type,
    const char* attname,
    const char* attval);

/**
 * recursively searches for an element using the sid addressing scheme
 * <p>The list will be filled with items matching the provided path with the
 * parent elements at the front of the list and the children on the end. If
 * the number of items in the path exceeds the maximum targets to return, the
 * list will be culled from the front, removing parent targets closer to the
 * root and preserving children closer to the leaves.</p>
 * @param searchroot the root element to begin the search
 * @param ref the path of the targets to search for
 * @param targetsout a list of pointers that will be filled with the results
 * @param typesout a matching list of target types to be filled
 * @param maxtargets the maximum number of targets to return
 * @return the number of items set in the list
 */
size_t dae_search_sid(
    dae_typebase* searchroot,
    const char* ref,
    void** targetsout,
    dae_targettype* typesout,
    size_t maxtargets);

/**
 * recursively searches for an element using the uri addressing scheme
 * @param searchroot the root element to begin the search
 * @param type typeid used to filter results, or dae_ID_INVALID for any type
 * @param uri a string formatted according to the uri addressing scheme
 * @return pointer to the result or NULL
 */
dae_typebase* dae_search_uri(
    dae_typebase* searchroot,
    dae_typeid type,
    const char* uri);

void dae_xml_startelement(
    dae_parser p,
    const char *el,
    const char **attr);

void dae_xml_chardata(
    dae_parser p,
    const char *s,
    int len);

void dae_xml_endelement(
    dae_parser p,
    const char *el);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DAE_DAE_H_

