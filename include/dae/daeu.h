/**
 * @brief     libdae utility header
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef DAE_DAEU_H_
#define DAE_DAEU_H_

#include "dae.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void daeu_calc_transform_matrix(
    const dae_transformation_element_list* transforms,
    float* mtxout);

void daeu_lookat_to_matrix(
    const dae_lookat* lookat,
    float* mtxout);

void daeu_rotate_to_matrix(
    const dae_rotate* rotate,
    float* mtxout);

void daeu_scale_to_matrix(
    const dae_scale* scale,
    float* mtxout);

void daeu_translate_to_matrix(
    const dae_translate* translate,
    float* mtxout);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DAE_DAEU_H_
