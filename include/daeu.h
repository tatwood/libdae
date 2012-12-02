/**
 * @brief     libdae utility header
 * @details   These utility functions are written using only the external API.
 *            They are provided as a convenience, but their functionality
 *            could be replicated without access to the library internals.
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef DAEU_H_
#define DAEU_H_

#include "dae.h"

typedef struct daeu_xml_parser_s* daeu_xml_parser;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void daeu_calc_transform_matrix(
    dae_obj_ptr obj,
    float* mtx_out);

void daeu_lookat_to_matrix(
    const dae_lookat_type* lookat,
    float* mtx_out);

void daeu_rotate_to_matrix(
    const dae_rotate_type* rotate,
    float* mtx_out);

void daeu_scale_to_matrix(
    const dae_scale_type* scale,
    float* mtx_out);

void daeu_translate_to_matrix(
    const dae_translate_type* translate,
    float* mtx_out);

/**
 * @details This function can be used to perform a sid search that targets an
 * element and possibly a data component within that element. For example, if
 * the search string is "node_0/translate.X", this function will set the
 * output parameters so that obj_out points to the object with id "translate"
 * within "node_0", and dataindex_out will be set to 0, representing an index
 * to the first float value in the data array of
 * "translate". The data_out parameter is optional.
 * @param obj_out out param that will be filled with target object
 * @param data_out optional out param that will be filled with the array index
 *        of the target data component referenced by the search string. If no
 *        component was specified, or it does not exist, it will be set to -1
 * @return The number of outputs found: 2 if both the object and data target
 *         were found, 1 if only an object target was found, and 0 if no 
 *         targets were found.
 */
int daeu_search_sid(
    dae_obj_ptr searchroot,
    const char* ref,
    dae_obj_ptr* obj_out,
    int* dataindex_out);

dae_obj_ptr daeu_search_uri(
    dae_obj_ptr searchroot,
    const char* uri);

void daeu_xml_chardata(
    void *userdata,
     const char *s,
     int len);

void daeu_xml_create(
    dae_COLLADA* root,
    daeu_xml_parser* parser_out);

void daeu_xml_destroy(
    daeu_xml_parser parser);

void daeu_xml_endelement(
    void* userdata,
    const char* el);

void daeu_xml_startelement(
    void* userdata,
    const char* el,
    const char** att);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DAEU_H_
