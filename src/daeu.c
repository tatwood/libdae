/**
 * @brief     libdae utility source
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#include <dae/daeu.h>
#include <assert.h>
#include <math.h>
#include <string.h>

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
#define daeuMATRIXMUL_COMP(r_, c_) \
    out[(r_)*4 + (c_)] = \
        a[(r_)*4+0]*b[0*4+(c_)] + \
        a[(r_)*4+1]*b[1*4+(c_)] + \
        a[(r_)*4+2]*b[2*4+(c_)] + \
        a[(r_)*4+3]*b[3*4+(c_)]
    daeuMATRIXMUL_COMP(0,0);
    daeuMATRIXMUL_COMP(0,1);
    daeuMATRIXMUL_COMP(0,2);
    daeuMATRIXMUL_COMP(0,3);
    daeuMATRIXMUL_COMP(1,0);
    daeuMATRIXMUL_COMP(1,1);
    daeuMATRIXMUL_COMP(1,2);
    daeuMATRIXMUL_COMP(1,3);
    daeuMATRIXMUL_COMP(2,0);
    daeuMATRIXMUL_COMP(2,1);
    daeuMATRIXMUL_COMP(2,2);
    daeuMATRIXMUL_COMP(2,3);
    daeuMATRIXMUL_COMP(3,0);
    daeuMATRIXMUL_COMP(3,1);
    daeuMATRIXMUL_COMP(3,2);
    daeuMATRIXMUL_COMP(3,3);
#undef daeuMATRIXMUL_COMP
}

//****************************************************************************
void daeu_lookat_to_matrix(
    const dae_lookat* lookat,
    float* mtxout)
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
    daeu_matrix_multiply(mtxrot, mtxtrans, mtxout);
}

//****************************************************************************
void daeu_rotate_to_matrix(
    const dae_rotate* rotate,
    float* mtxout)
{
    const float* axis = rotate->data;
    float rad = rotate->data[3] * (3.14159265f/180.0f);
    float c = cosf(rad);
    float s = sinf(rad);
    mtxout[ 0] = axis[0]*axis[0]*(1.0f-c) + c;
    mtxout[ 1] = axis[0]*axis[1]*(1.0f-c) - axis[2]*s;
    mtxout[ 2] = axis[0]*axis[2]*(1.0f-c) + axis[1]*s;
    mtxout[ 3] = 0.0f;
    mtxout[ 4] = axis[1]*axis[0]*(1.0f-c) + axis[2]*s;
    mtxout[ 5] = axis[1]*axis[1]*(1.0f-c) + c;
    mtxout[ 6] = axis[1]*axis[2]*(1.0f-c) - axis[0]*s;
    mtxout[ 7] = 0.0f;
    mtxout[ 8] = axis[2]*axis[0]*(1.0f-c) - axis[1]*s;
    mtxout[ 9] = axis[2]*axis[1]*(1.0f-c) + axis[0]*s;
    mtxout[10] = axis[2]*axis[2]*(1.0f-c) + c;
    mtxout[11] = 0.0f;
    mtxout[12] = 0.0f;
    mtxout[13] = 0.0f;
    mtxout[14] = 0.0f;
    mtxout[15] = 1.0f;
}

//****************************************************************************
void daeu_scale_to_matrix(
    const dae_scale* scale,
    float* mtxout)
{
    daeu_matrix_identity(mtxout);
    mtxout[ 0] = scale->data[0];
    mtxout[ 5] = scale->data[1];
    mtxout[10] = scale->data[2];
}

//****************************************************************************
void daeu_translate_to_matrix(
    const dae_translate* translate,
    float* mtxout)
{
    daeu_matrix_identity(mtxout);
    mtxout[ 3] = translate->data[0];
    mtxout[ 7] = translate->data[1];
    mtxout[11] = translate->data[2];
}

//****************************************************************************
void daeu_calc_transform_matrix(
    const dae_transformation_element_list* transforms,
    float* mtxout)
{
    float mtxa[16];
    float mtxb[16];
    const dae_transformation_element* xform = transforms->values;
    const dae_transformation_element* xformend = xform + transforms->size;

    daeu_matrix_identity(mtxout);
    while(xform != xformend)
    {
        memcpy(mtxa, mtxout, sizeof(mtxa));
        daeu_matrix_identity(mtxb);
        switch(xform->daebase.type)
        {
        case dae_ID_LOOKAT:
            daeu_lookat_to_matrix(&xform->lookat, mtxb);
            break;
        case dae_ID_MATRIX:
            memcpy(mtxb, xform->matrix.data, sizeof(mtxb));
            break;
        case dae_ID_ROTATE:
            daeu_rotate_to_matrix(&xform->rotate, mtxb);
            break;
        case dae_ID_SCALE:
            daeu_scale_to_matrix(&xform->scale, mtxb);
            break;
        case dae_ID_SKEW:
            // TODO support SKEW
            assert(0);
            break;
        case dae_ID_TRANSLATE:
            daeu_translate_to_matrix(&xform->translate, mtxb);
            break;
        default:
            assert(0);
            break;
        }
        daeu_matrix_multiply(mtxa, mtxb, mtxout);
        ++xform;
    }
}
