/**
 * @brief     libdae header
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef DAE_H_
#define DAE_H_

#include <stddef.h>

//****************************************************************************
// enums

/*GEN_TYPEIDS_BGN*/
enum dae_obj_typeid_e
{
    dae_ID_BOOLEAN,
    dae_ID_HEXBINARY,
    dae_ID_FLOAT,
    dae_ID_BYTE,
    dae_ID_SHORT,
    dae_ID_INT,
    dae_ID_STRING,
    dae_ID_UNSIGNEDBYTE,
    dae_ID_UNSIGNEDINT,
    dae_ID_COLLADA,
    dae_ID_INVALID = -1
};
/*GEN_TYPEIDS_END*/

enum dae_native_typeid_e
{
    /// can be evaluated as int32_t
    dae_NATIVE_BOOL32 = dae_ID_BOOLEAN,
    /// can be evaluated as uint8_t
    dae_NATIVE_HEX8 = dae_ID_HEXBINARY,
    /// can be evaluated as float
    dae_NATIVE_FLOAT = dae_ID_FLOAT,
    /// can be evaluated as int8_t
    dae_NATIVE_INT8 = dae_ID_BYTE,
    /// can be evaluated as int16_t
    dae_NATIVE_INT16 = dae_ID_SHORT,
    /// can be evaluated as int32_t
    dae_NATIVE_INT32 = dae_ID_INT,
    /// can be evaluated as char*
    dae_NATIVE_STRING = dae_ID_STRING,
    /// can be evaluated as uint8_t
    dae_NATIVE_UINT8 = dae_ID_UNSIGNEDBYTE,
    /// can be evaluated as uint32_t
    dae_NATIVE_UINT32 = dae_ID_UNSIGNEDINT
};

//****************************************************************************
// typedefs

typedef enum dae_native_typeid_e dae_native_typeid;
typedef enum dae_obj_typeid_e dae_obj_typeid;

typedef void* dae_obj_ptr;

/*GEN_TYPEDEFS_BGN*/
typedef struct dae_COLLADA_S dae_COLLADA;
/*GEN_TYPEDEFS_END*/

//****************************************************************************
// structs

/*GEN_STRUCTS_BGN*/
/*GEN_STRUCTS_END*/

//****************************************************************************
// functions

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

dae_obj_ptr dae_add_attrib(
    dae_obj_ptr parent,
    const char* name,
    const char* value);

dae_obj_ptr dae_add_element(
    dae_obj_ptr parent,
    const char* name);

dae_COLLADA* dae_create();

void dae_destroy(
    dae_COLLADA* doc);

size_t dae_get_data(
    dae_obj_ptr obj,
    dae_native_typeid* type_out,
    void** data_out,
    size_t* datalen_out);

dae_obj_ptr dae_get_first_attrib(
    dae_obj_ptr obj);

dae_obj_ptr dae_get_first_element(
    dae_obj_ptr obj);

const char* dae_get_name(
    dae_obj_ptr obj);

dae_obj_ptr dae_get_next(
    dae_obj_ptr obj);

dae_obj_ptr dae_get_parent(
    dae_obj_ptr child);

dae_obj_typeid dae_get_typeid(
    dae_obj_ptr obj);

void dae_set_string(
    dae_obj_ptr obj,
    const char* data);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DAE_H_

