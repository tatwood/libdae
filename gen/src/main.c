/**
 * @brief     dae code gen_eration function main and schema definitions
 * @author    Thomas Atwood (tatwood.net)
 * @date      2011
 * @copyright unlicense / public domain
 ****************************************************************************/
#if !defined(NDEBUG) && defined(_MSC_FULL_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "gen.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// from genc.c
extern void gen_writesource(
    const gen_schema* schema,
    FILE* fp);

// from genh.c
extern void gen_writeheader(
    const gen_schema* schema,
    FILE* fp);

//****************************************************************************
static void createschema(
    gen_schema* schema)
{
    gen_typeid type;
    const char* transforms[] =
    {
        "lookat", "matrix", "rotate", "scale", "skew", "translate"
    };
    memset(schema, 0, sizeof(*schema));

    type = gen_add_choice(
        schema,
        "transformation_element",
        transforms,
        sizeof(transforms)/sizeof(*transforms));

    // accessor
    type = gen_add_complex(schema, "accessor");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "offset", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "source", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "stride", NULL, gen_DATA_UINT);
    gen_add_element(schema, type, "param", NULL, NULL, 0, -1);

    // ambient
    type = gen_add_complex(schema, "ambient");
    gen_add_element(schema, type, "color", NULL, "targetable_float3", 1, 1);

    // animation
    type = gen_add_complex(schema, "animation");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "animation", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "source", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "sampler", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "channel", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // animation_clip
    type = gen_add_complex(schema, "animation_clip");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "start", NULL, gen_DATA_FLOAT);
    gen_add_attr(schema, type, "end", NULL, gen_DATA_FLOAT);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "instance_animation", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // annotate
    type = gen_add_complex(schema, "annotate");
    gen_add_element(schema, type, "bool", "bool_", NULL, 0, 1);
    gen_add_element(schema, type, "bool2", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "bool3", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "bool4", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "int", "int_", NULL, 0, 1);
    gen_add_element(schema, type, "int2", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "int3", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "int4", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float", "float_", NULL, 0, 1);
    gen_add_element(schema, type, "float2", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float3", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float4", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float2x2", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float3x3", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float4x4", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "string", NULL, NULL, 0, 1);

    // asset
    type = gen_add_complex(schema, "asset");
    gen_add_element(schema, type, "contributor", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "coverage", NULL, "asset_coverage", 0, 1);
    gen_add_element(schema, type, "created", NULL, "string", 1, 1);
    gen_add_element(schema, type, "keywords", NULL, "string", 0, 1);
    gen_add_element(schema, type, "modified", NULL, "string", 1, 1);
    gen_add_element(schema, type, "revision", NULL, "string", 0, 1);
    gen_add_element(schema, type, "subject", NULL, "string", 0, 1);
    gen_add_element(schema, type, "title", NULL, "string", 0, 1);
    gen_add_element(schema, type, "unit", NULL, "asset_unit", 0, 1);
    gen_add_element(schema, type, "up_axis", NULL, "string", 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);
    type = gen_add_complex(schema, "asset_coverage");
    gen_add_element(schema, type, "geographic_location", NULL, NULL, 0, 1);
    type = gen_add_complex(schema, "asset_unit");
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "meter", NULL, gen_DATA_FLOAT);

    // bool
    type = gen_add_complex(schema, "bool");
    gen_set_data(schema, type, gen_DATA_BOOL, 1, 1);

    // bool2
    type = gen_add_complex(schema, "bool2");
    gen_set_data(schema, type, gen_DATA_BOOL, 2, 2);

    // bool3
    type = gen_add_complex(schema, "bool3");
    gen_set_data(schema, type, gen_DATA_BOOL, 3, 3);

    // bool4
    type = gen_add_complex(schema, "bool4");
    gen_set_data(schema, type, gen_DATA_BOOL, 4, 4);

    // bool_array
    type = gen_add_complex(schema, "bool_array");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_BOOL, 0, -1);

    // camera
    type = gen_add_complex(schema, "camera");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "optics", NULL, NULL, 1, 1);
    gen_add_element(schema, type, "imager", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // channel
    type = gen_add_complex(schema, "channel");
    gen_add_attr(schema, type, "source", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "target", NULL, gen_DATA_STRING);

    // COLLADA
    type = gen_add_complex(schema, "COLLADA");
    gen_add_attr(schema, type, "version", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "xmlns", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "base", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 1, 1);
    gen_add_element(schema, type, "library_animation_clips", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_animations", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_articulated_systems", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_cameras", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_controllers", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_effects", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_force_fields", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_geometries", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_images", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_joints", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_kinematic_models", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_kinematic_scenes", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_lights", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_materials", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_nodes", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_physics_materials", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_physics_models", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_physics_scenes", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "library_visual_scenes", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "scene", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // contributor
    type = gen_add_complex(schema, "contributor");
    gen_add_element(schema, type, "author", NULL, "string", 0, 1);
    gen_add_element(schema, type, "authoring_email", NULL, "string", 0, 1);
    gen_add_element(schema, type, "authoring_website", NULL, "string", 0, 1);
    gen_add_element(schema, type, "authoring_tool", NULL, "string", 0, 1);
    gen_add_element(schema, type, "comments", NULL, "string", 0, 1);
    gen_add_element(schema, type, "copyright", NULL, "string", 0, 1);
    gen_add_element(schema, type, "source_data", NULL, "string", 0, 1);

    // controller
    type = gen_add_complex(schema, "controller");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "skin", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "morph", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // control_vertices
    type = gen_add_complex(schema, "control_vertices");
    gen_add_element(schema, type, "input", NULL, "input_unshared", 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // directional
    type = gen_add_complex(schema, "directional");
    gen_add_element(schema, type, "color", NULL, "targetable_float3", 1, 1);

    // evaluate_scene
    type = gen_add_complex(schema, "evaluate_scene");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "enable", NULL, gen_DATA_BOOL);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "render", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // extra
    type = gen_add_complex(schema, "extra");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "type", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "technique", NULL, NULL, 1, -1);

    // float
    type = gen_add_complex(schema, "float");
    gen_set_data(schema, type, gen_DATA_FLOAT, 1, 1);

    // float2
    type = gen_add_complex(schema, "float2");
    gen_set_data(schema, type, gen_DATA_FLOAT, 2, 2);

    // float3
    type = gen_add_complex(schema, "float3");
    gen_set_data(schema, type, gen_DATA_FLOAT, 3, 3);

    // float4
    type = gen_add_complex(schema, "float4");
    gen_set_data(schema, type, gen_DATA_FLOAT, 4, 4);

    // float2x2
    type = gen_add_complex(schema, "float2x2");
    gen_set_data(schema, type, gen_DATA_FLOAT, 4, 4);

    // float3x3
    type = gen_add_complex(schema, "float3x3");
    gen_set_data(schema, type, gen_DATA_FLOAT, 9, 9);

    // float4x4
    type = gen_add_complex(schema, "float4x4");
    gen_set_data(schema, type, gen_DATA_FLOAT, 16, 16);

    // float_array
    type = gen_add_complex(schema, "float_array");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "digits", NULL, gen_DATA_INT);
    gen_add_attr(schema, type, "magnitude", NULL, gen_DATA_INT);
    gen_set_data(schema, type, gen_DATA_FLOAT, 0, -1);

    // float_or_param
    type = gen_add_complex(schema, "float_or_param");
    gen_add_element(schema, type, "float", "float_", "targetable_float", 0, 1);
    gen_add_element(schema, type, "param", NULL, "param_ref", 0, 1);

    // formula
    type = gen_add_complex(schema, "formula");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "target", NULL, "float_or_param", 1, 1);
    gen_add_element(schema, type, "technique_common", NULL, "technique_common_formula", 1, 1);
    gen_add_element(schema, type, "technique", NULL, NULL, 0, -1);
    type = gen_add_complex(schema, "technique_common_formula");
    gen_set_data(schema, type, gen_DATA_STRING, 0, 1);

    // geographic_location
    type = gen_add_complex(schema, "geographic_location");
    gen_add_element(schema, type, "longitude", NULL, "float", 1, 1);
    gen_add_element(schema, type, "latitude", NULL, "float", 1, 1);
    gen_add_element(schema, type, "altitude", NULL, "geographic_location_altitude", 1, 1);
    type = gen_add_complex(schema, "geographic_location_altitude");
    gen_add_attr(schema, type, "mode", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 1, 1);

    // geometry
    type = gen_add_complex(schema, "geometry");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "convex_mesh", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "mesh", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "spline", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "brep", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // IDREF_array
    type = gen_add_complex(schema, "IDREF_array");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_STRING, 0, -1);

    // imager
    type = gen_add_complex(schema, "imager");
    gen_add_element(schema, type, "technique", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // input_shared
    type = gen_add_complex(schema, "input_shared");
    gen_add_attr(schema, type, "offset", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "semantic", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "source", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "set", NULL, gen_DATA_UINT);

    // input_unshared
    type = gen_add_complex(schema, "input_unshared");
    gen_add_attr(schema, type, "semantic", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "source", NULL, gen_DATA_STRING);

    // instance_animation
    type = gen_add_complex(schema, "instance_animation");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // instance_camera
    type = gen_add_complex(schema, "instance_camera");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // instance_controller
    type = gen_add_complex(schema, "instance_controller");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "skeleton", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "bind_material", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // instance_formula
    type = gen_add_complex(schema, "instance_formula");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "setparam", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // instance_geometry
    type = gen_add_complex(schema, "instance_geometry");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "bind_material", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // instance_light
    type = gen_add_complex(schema, "instance_light");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // instance_node
    type = gen_add_complex(schema, "instance_node");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "proxy", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // instance_visual_scene
    type = gen_add_complex(schema, "instance_visual_scene");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // int
    type = gen_add_complex(schema, "int");
    gen_set_data(schema, type, gen_DATA_INT, 1, 1);

    // int2
    type = gen_add_complex(schema, "int2");
    gen_set_data(schema, type, gen_DATA_INT, 2, 2);

    // int3
    type = gen_add_complex(schema, "int3");
    gen_set_data(schema, type, gen_DATA_INT, 3, 3);

    // int4
    type = gen_add_complex(schema, "int4");
    gen_set_data(schema, type, gen_DATA_INT, 4, 4);

    // int_array
    type = gen_add_complex(schema, "int_array");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "mininclusive", NULL, gen_DATA_INT);
    gen_add_attr(schema, type, "maxinclusive", NULL, gen_DATA_INT);
    gen_set_data(schema, type, gen_DATA_INT, 0, -1);

    // joints
    type = gen_add_complex(schema, "joints");
    gen_add_element(schema, type, "input",  NULL,"input_unshared", 2, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_animation_clips
    type = gen_add_complex(schema, "library_animation_clips");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "animation_clip", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_animations
    type = gen_add_complex(schema, "library_animations");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "animation", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_cameras
    type = gen_add_complex(schema, "library_cameras");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "camera", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_controllers
    type = gen_add_complex(schema, "library_controllers");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "controller", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_formulas
    type = gen_add_complex(schema, "library_formulas");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "formula", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_geometries
    type = gen_add_complex(schema, "library_geometries");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "geometry", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_lights
    type = gen_add_complex(schema, "library_lights");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "light", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_nodes
    type = gen_add_complex(schema, "library_nodes");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "node", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_visual_scenes
    type = gen_add_complex(schema, "library_visual_scenes");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "visual_scene", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // light
    type = gen_add_complex(schema, "light");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "technique_common", NULL, "technique_common_light", 1, 1);
    gen_add_element(schema, type, "technique", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);
    // technique_common_light
    type = gen_add_complex(schema, "technique_common_light");
    gen_add_element(schema, type, "ambient", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "directional", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "point", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "spot", NULL, NULL, 0, 1);

    // lines
    type = gen_add_complex(schema, "lines");
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "material", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_shared", 0, -1);
    gen_add_element(schema, type, "p", NULL, "list_of_uints", 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // linestrips
    type = gen_add_complex(schema, "linestrips");
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "material", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_shared", 0, -1);
    gen_add_element(schema, type, "p", NULL, "list_of_uints", 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // list_of_ints
    type = gen_add_complex(schema, "list_of_ints");
    gen_set_data(schema, type, gen_DATA_INT, 0, -1);

    // list_of_uints
    type = gen_add_complex(schema, "list_of_uints");
    gen_set_data(schema, type, gen_DATA_UINT, 0, -1);

    // lookat
    type = gen_add_complex(schema, "lookat");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 9, 9);

    // matrix
    type = gen_add_complex(schema, "matrix");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 16, 16);

    // mesh
    type = gen_add_complex(schema, "mesh");
    gen_add_element(schema, type, "source", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "vertices", NULL, NULL, 1, 1);
    gen_add_element(schema, type, "lines", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "linestrips", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "polygons", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "polylist", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "triangles", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "trifans", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "tristrips", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // morph
    type = gen_add_complex(schema, "morph");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "method", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "source", NULL, NULL, 2, -1);
    gen_add_element(schema, type, "targets", NULL, NULL, 1, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // Name_array
    type = gen_add_complex(schema, "Name_array");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_STRING, 0, -1);

    // newparam
    type = gen_add_complex(schema, "newparam");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "annotate", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "semantic", NULL, "string", 0, 1);
    gen_add_element(schema, type, "modifier", NULL, "string", 0, 1);
    gen_add_element(schema, type, "float", "float_", NULL, 0, 1);
    gen_add_element(schema, type, "float2", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float3", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float4", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "sampler2D", NULL, "fx_sampler_common", 0, 1);

    // node
    type = gen_add_complex(schema, "node");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "type", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "layer", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "transformation_element", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "instance_camera", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "instance_controller", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "instance_geometry", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "instance_light", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "instance_node", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "node", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // optics
    type = gen_add_complex(schema, "optics");
    gen_add_element(schema, type, "technique_common", NULL, "technique_common_optics", 1, 1);
    gen_add_element(schema, type, "technique", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);
    type = gen_add_complex(schema, "technique_common_optics");
    gen_add_element(schema, type, "orthographic", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "perspective", NULL, NULL, 0, 1);

    // orthographic
    type = gen_add_complex(schema, "orthographic");
    gen_add_element(schema, type, "xmag", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "ymag", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "aspect_ratio", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "znear", NULL, "targetable_float", 1, 1);
    gen_add_element(schema, type, "zfar", NULL, "targetable_float", 1, 1);

    // param
    type = gen_add_complex(schema, "param");
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "type", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "semantic", NULL, gen_DATA_STRING);

    // param_ref
    type = gen_add_complex(schema, "param_ref");
    gen_add_attr(schema, type, "ref", NULL, gen_DATA_STRING);

    // perspective
    type = gen_add_complex(schema, "perspective");
    gen_add_element(schema, type, "xfov", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "yfov", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "aspect_ratio", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "znear", NULL, "targetable_float", 1, 1);
    gen_add_element(schema, type, "zfar", NULL, "targetable_float", 1, 1);

    // ph
    type = gen_add_complex(schema, "ph");
    gen_add_element(schema, type, "p", NULL, "list_of_uints", 0, 1);
    gen_add_element(schema, type, "h", NULL, "list_of_uints", 0, 1);

    // point
    type = gen_add_complex(schema, "point");
    gen_add_element(schema, type, "color", NULL, "targetable_float3", 1, 1);
    gen_add_element(schema, type, "constant_attenuation", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "linear_attenuation", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "quadratic_attenuation", NULL, "targetable_float", 0, 1);

    // polygons
    type = gen_add_complex(schema, "polygons");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "material", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_shared", 0, -1);
    gen_add_element(schema, type, "p", NULL, "list_of_uints", 0, -1);
    gen_add_element(schema, type, "ph", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // polylist
    type = gen_add_complex(schema, "polylist");
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "material", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_shared", 0, -1);
    gen_add_element(schema, type, "vcount", NULL, "list_of_uints", 0, 1);
    gen_add_element(schema, type, "p", NULL, "list_of_uints", 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // rotate
    type = gen_add_complex(schema, "rotate");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 4, 4);

    // sampler
    type = gen_add_complex(schema, "sampler");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "pre_behavior", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "post_behavior", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_shared", 1, -1);

    // scale
    type = gen_add_complex(schema, "scale");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 3, 3);

    // scene
    type = gen_add_complex(schema, "scene");
    gen_add_element(schema, type, "instance_physics_scene", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "instance_visual_scene", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "instance_kinematics_scene", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // TODO: finish
    // setparam
    type = gen_add_complex(schema, "setparam");
    gen_add_attr(schema, type, "ref", NULL, gen_DATA_STRING);

    // SIDREF_array
    type = gen_add_complex(schema, "SIDREF_array");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_STRING, 0, -1);

    // skeleton
    type = gen_add_complex(schema, "skeleton");
    gen_set_data(schema, type, gen_DATA_STRING, 0, 1);

    // skew
    type = gen_add_complex(schema, "skew");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 7, 7);

    // skin
    type = gen_add_complex(schema, "skin");
    gen_add_attr(schema, type, "source", "source", gen_DATA_STRING);
    gen_add_element(schema, type, "bind_shape_matrix", NULL, "float4x4", 0, 1);
    gen_add_element(schema, type, "source", "source_", NULL, 3, -1);
    gen_add_element(schema, type, "joints", NULL, NULL, 1, 1);
    gen_add_element(schema, type, "vertex_weights", NULL, NULL, 1, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // source
    type = gen_add_complex(schema, "source");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "bool_array", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "float_array", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "IDREF_array", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "int_array", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "Name_array", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "SIDREF_array", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "token_array", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "technique_common", NULL, "technique_common_source", 0, 1);
    gen_add_element(schema, type, "technique", NULL, NULL, 0, -1);
    type = gen_add_complex(schema, "technique_common_source");
    gen_add_element(schema, type, "accessor", NULL, NULL, 1, 1);

    // spline
    type = gen_add_complex(schema, "spline");
    gen_add_attr(schema, type, "closed", NULL, gen_DATA_BOOL);
    gen_add_element(schema, type, "source", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "control_vertices", NULL, NULL, 1, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // spot
    type = gen_add_complex(schema, "spot");
    gen_add_element(schema, type, "color", NULL, "targetable_float3", 1, 1);
    gen_add_element(schema, type, "constant_attenuation", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "linear_attenuation", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "quadratic_attenuation", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "falloff_angle", NULL, "targetable_float", 0, 1);
    gen_add_element(schema, type, "falloff_exponent", NULL, "targetable_float", 0, 1);

    // string
    type = gen_add_complex(schema, "string");
    gen_set_data(schema, type, gen_DATA_STRING, 0, 1);

    // targetable_float
    type = gen_add_complex(schema, "targetable_float");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 1, 1);

    // targetable_float3
    type = gen_add_complex(schema, "targetable_float3");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 3, 3);

    // targetable_float4
    type = gen_add_complex(schema, "targetable_float4");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 4, 4);

    // targets
    type = gen_add_complex(schema, "targets");
    gen_add_element(schema, type, "input", NULL, "input_unshared", 2, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // technique
    type = gen_add_complex(schema, "technique");
    gen_add_attr(schema, type, "profile", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "xmlns", NULL, gen_DATA_STRING);

    // technique_hint
    type = gen_add_complex(schema, "technique_hint");
    gen_add_attr(schema, type, "platform", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "ref", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "profile", NULL, gen_DATA_STRING);

    // translate
    type = gen_add_complex(schema, "translate");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_FLOAT, 3, 3);

    // triangles
    type = gen_add_complex(schema, "triangles");
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "material", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_shared", 0, -1);
    gen_add_element(schema, type, "p", NULL, "list_of_uints", 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // trifans
    type = gen_add_complex(schema, "trifans");
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "material", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_shared", 0, -1);
    gen_add_element(schema, type, "p", NULL, "list_of_uints", 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // tristrips
    type = gen_add_complex(schema, "tristrips");
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_attr(schema, type, "material", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_shared", 0, -1);
    gen_add_element(schema, type, "p", NULL, "list_of_uints", 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // uint
    type = gen_add_complex(schema, "uint");
    gen_set_data(schema, type, gen_DATA_UINT, 0, 1);

    // vertex_weights
    type = gen_add_complex(schema, "vertex_weights");
    gen_add_attr(schema, type, "count", NULL, gen_DATA_UINT);
    gen_add_element(schema, type, "input", NULL, "input_shared", 2, -1);
    gen_add_element(schema, type, "vcount", NULL, "list_of_uints", 0, 1);
    gen_add_element(schema, type, "v", NULL, "list_of_ints", 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // vertices
    type = gen_add_complex(schema, "vertices");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "input", NULL, "input_unshared", 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // visual_scene
    type = gen_add_complex(schema, "visual_scene");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "node", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "evaluate_scene", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

#if 1 // FX

    // bind_material
    type = gen_add_complex(schema, "bind_material");
    gen_add_element(schema, type, "param", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "technique_common", NULL, "technique_common_bind_material", 1, 1);
    gen_add_element(schema, type, "technique", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);
    type = gen_add_complex(schema, "technique_common_bind_material");
    gen_add_element(schema, type, "instance_material", NULL, "instance_material_geometry", 1, -1);

    // blinn
    type = gen_add_complex(schema, "blinn");
    gen_add_element(schema, type, "emission", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "ambient", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "diffuse", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "specular", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "shininess", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "reflective", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "reflectivity", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "transparent", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "transparency", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "index_of_refraction", NULL, "float_or_param", 0, 1);

    // color_or_texture
    type = gen_add_complex(schema, "color_or_texture");
    gen_add_attr(schema, type, "opaque", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "color", NULL, "float4", 0, 1);
    gen_add_element(schema, type, "param", NULL, "param_ref", 0, 1);
    gen_add_element(schema, type, "texture", NULL, "texture_fx", 0, 1);

    // constant
    type = gen_add_complex(schema, "constant");
    gen_add_element(schema, type, "emission", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "reflective", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "reflectivity", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "transparent", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "transparency", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "index_of_refraction", NULL, "float_or_param", 0, 1);

    // effect
    type = gen_add_complex(schema, "effect");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "annotate", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "newparam", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "profile_COMMON", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // fx_sampler_common
    type = gen_add_complex(schema, "fx_sampler_common");
    gen_add_element(schema, type, "instance_image", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "wrap_s", NULL, "string", 0, 1);
    gen_add_element(schema, type, "wrap_t", NULL, "string", 0, 1);
    gen_add_element(schema, type, "wrap_p", NULL, "string", 0, 1);
    gen_add_element(schema, type, "minfilter", NULL, "string", 0, 1);
    gen_add_element(schema, type, "magfilter", NULL, "string", 0, 1);
    gen_add_element(schema, type, "mipfilter", NULL, "string", 0, 1);
    gen_add_element(schema, type, "border_color", NULL, "float4", 0, 1);
    gen_add_element(schema, type, "mip_max_level", NULL, "uint", 0, 1);
    gen_add_element(schema, type, "mip_min_level", NULL, "uint", 0, 1);
    gen_add_element(schema, type, "mip_bias", NULL, "float", 0, 1);
    gen_add_element(schema, type, "max_anisotropy", NULL, "uint", 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // image
    type = gen_add_complex(schema, "image");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "renderable", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "init_from", NULL, "image_init_from", 0, 1);
    gen_add_element(schema, type, "create_2d", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "create_3d", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "create_cube", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);
    type = gen_add_complex(schema, "image_renderable");
    gen_add_attr(schema, type, "share", NULL, gen_DATA_BOOL);
    type = gen_add_complex(schema, "image_init_from");
    gen_add_attr(schema, type, "mips_gen_erate", NULL, gen_DATA_BOOL);
    gen_add_element(schema, type, "ref",  NULL,"string", 0, 1);
    gen_add_element(schema, type, "hex",  NULL,"image_hex", 0, 1);
    gen_set_data(schema, type, gen_DATA_STRING, 0, 1); // for fbxdae compatibility
    type = gen_add_complex(schema, "image_hex");
    gen_add_attr(schema, type, "format", NULL, gen_DATA_STRING);
    gen_set_data(schema, type, gen_DATA_STRING, 0, 1);

    // instance_effect
    type = gen_add_complex(schema, "instance_effect");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "url", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "technique_hint", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "setparam", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // instance_material_geometry
    type = gen_add_complex(schema, "instance_material_geometry");
    gen_add_attr(schema, type, "sid", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "target", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "symbol", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "bind", NULL, "bind_fx", 0, -1);
    gen_add_element(schema, type, "bind_vertex_input", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // lambert
    type = gen_add_complex(schema, "lambert");
    gen_add_element(schema, type, "emission", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "ambient", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "diffuse", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "reflective", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "reflectivity", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "transparent", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "transparency", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "index_of_refraction", NULL, "float_or_param", 0, 1);

    // library_effects
    type = gen_add_complex(schema, "library_effects");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "effect", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_images
    type = gen_add_complex(schema, "library_images");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "image", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // library_materials
    type = gen_add_complex(schema, "library_materials");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "material", NULL, NULL, 1, -1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // material
    type = gen_add_complex(schema, "material");
    gen_add_attr(schema, type, "id", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "name", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "asset", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "instance_effect", NULL, NULL, 1, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // phong
    type = gen_add_complex(schema, "phong");
    gen_add_element(schema, type, "emission", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "ambient", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "diffuse", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "specular", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "shininess", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "reflective", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "reflectivity", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "transparent", NULL, "color_or_texture", 0, 1);
    gen_add_element(schema, type, "transparency", NULL, "float_or_param", 0, 1);
    gen_add_element(schema, type, "index_of_refraction", NULL, "float_or_param", 0, 1);

    // profile_COMMON
    type = gen_add_complex(schema, "profile_COMMON");
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "newparam", NULL, NULL, 0, -1);
    gen_add_element(schema, type, "technique", NULL, "technique_fx", 1, 1);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);

    // technique_fx
    type = gen_add_complex(schema, "technique_fx");
    gen_add_element(schema, type, "asset", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "constant", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "lambert", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "phong", NULL, NULL, 0, 1);
    gen_add_element(schema, type, "blinn", NULL, NULL, 0, 1);

    // texture_fx
    type = gen_add_complex(schema, "texture_fx");
    gen_add_attr(schema, type, "texture", NULL, gen_DATA_STRING);
    gen_add_attr(schema, type, "texcoord", NULL, gen_DATA_STRING);
    gen_add_element(schema, type, "extra", NULL, NULL, 0, -1);
#endif
}

//****************************************************************************
static void destroyschema(
    gen_schema* schema)
{
}

#define GENC_BGN "/*GENC_BGN*/"
#define GENC_END "/*GENC_END*/"
#define GENH_BGN "/*GENH_BGN*/"
#define GENH_END "/*GENH_END*/"
static void writeoutput(
    const gen_schema* schema,
    const char* tmplt,
    FILE* fp)
{
    const char* bgn = tmplt;
    const char* end = tmplt;    
    while(*end != '\0')
    {
        if(!strncmp(end, GENC_BGN, sizeof(GENC_BGN)-1))
        {
            fwrite(bgn, sizeof(*bgn), end - bgn, fp);        
            gen_writesource(schema, fp);
            bgn = end + sizeof(GENC_BGN)-1;
            end = bgn;
        }
        else if(!strncmp(end, GENC_END, sizeof(GENC_END)-1))
        {
            bgn = end + sizeof(GENC_END)-1;
            end = bgn;
        }
        else if(!strncmp(end, GENH_BGN, sizeof(GENH_BGN)-1))
        {
            fwrite(bgn, sizeof(*bgn), end - bgn, fp);        
            gen_writeheader(schema, fp);
            bgn = end + sizeof(GENH_BGN)-1;
            end = bgn;
        }
        else if(!strncmp(end, GENH_END, sizeof(GENH_END)-1))
        {
            bgn = end + sizeof(GENH_END)-1;
            end = bgn;
        }
        else
        {
            ++end;
        }
    }
    if(bgn != end)
    {
        fwrite(bgn, sizeof(*bgn), end - bgn, fp);        
    }
}

//****************************************************************************
int main(int argc, const char* argv[])
{
#if !defined(NDEBUG) && defined(_MSC_FULL_VER)
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
    //_CrtSetBreakAlloc(0);
    {
#endif
    if(argc == 3)
    {
        FILE* fpi = fopen(argv[1], "rb");
        FILE* fpo = fopen(argv[2], "wb");
        if(fpi != NULL && fpo != NULL)
        {
            char* data = NULL;
            size_t size = 0;
            fseek(fpi, 0, SEEK_END);
            size = ftell(fpi);
            fseek(fpi, 0, SEEK_SET);
            if(size > 0)
            {
                data = (char*) malloc(sizeof(*data) * (size+1));
                if(fread(data, 1, size, fpi) != size)
                {
                    free(data);
                    data = NULL;
                    size = 0;
                }
            }
            if(data != NULL)
            {
                gen_schema schema;
                createschema(&schema);
                data[size] = '\0';
                writeoutput(&schema, data, fpo);
                destroyschema(&schema);
                free(data);
            }
        }
        if(fpi != NULL)
        {
            fclose(fpi);
        }
        else
        {
            printf("error: could not open file \"%s\"\n", argv[1]);
        }
        if(fpo != NULL)
        {
            fclose(fpo);
        }
        else
        {
            printf("error: could not open file \"%s\"\n", argv[2]);
        }
    }    
    else
    {
        printf("usage: daegen_ <template file> <output file>\n");
    }
#if !defined(NDEBUG) && defined(_MSC_FULL_VER)
    _CrtCheckMemory();
    _CrtDumpMemoryLeaks();
    }
#endif
    return EXIT_SUCCESS;
}

