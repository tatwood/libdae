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

enum dae_typeid_e
{
    dae_ID_INVALID = -1,
    dae_ID_TRANSFORMATION_ELEMENT,
    dae_ID_LOOKAT,
    dae_ID_MATRIX,
    dae_ID_ROTATE,
    dae_ID_SCALE,
    dae_ID_SKEW,
    dae_ID_TRANSLATE,
    dae_ID_ACCESSOR,
    dae_ID_PARAM,
    dae_ID_AMBIENT,
    dae_ID_TARGETABLE_FLOAT3,
    dae_ID_ANIMATION,
    dae_ID_ASSET,
    dae_ID_SOURCE,
    dae_ID_SAMPLER,
    dae_ID_CHANNEL,
    dae_ID_EXTRA,
    dae_ID_ANIMATION_CLIP,
    dae_ID_INSTANCE_ANIMATION,
    dae_ID_ANNOTATE,
    dae_ID_BOOL,
    dae_ID_BOOL2,
    dae_ID_BOOL3,
    dae_ID_BOOL4,
    dae_ID_INT,
    dae_ID_INT2,
    dae_ID_INT3,
    dae_ID_INT4,
    dae_ID_FLOAT,
    dae_ID_FLOAT2,
    dae_ID_FLOAT3,
    dae_ID_FLOAT4,
    dae_ID_FLOAT2X2,
    dae_ID_FLOAT3X3,
    dae_ID_FLOAT4X4,
    dae_ID_STRING,
    dae_ID_CONTRIBUTOR,
    dae_ID_ASSET_COVERAGE,
    dae_ID_ASSET_UNIT,
    dae_ID_GEOGRAPHIC_LOCATION,
    dae_ID_BOOL_ARRAY,
    dae_ID_CAMERA,
    dae_ID_OPTICS,
    dae_ID_IMAGER,
    dae_ID_COLLADA,
    dae_ID_LIBRARY_ANIMATION_CLIPS,
    dae_ID_LIBRARY_ANIMATIONS,
    dae_ID_LIBRARY_ARTICULATED_SYSTEMS,
    dae_ID_LIBRARY_CAMERAS,
    dae_ID_LIBRARY_CONTROLLERS,
    dae_ID_LIBRARY_EFFECTS,
    dae_ID_LIBRARY_FORCE_FIELDS,
    dae_ID_LIBRARY_GEOMETRIES,
    dae_ID_LIBRARY_IMAGES,
    dae_ID_LIBRARY_JOINTS,
    dae_ID_LIBRARY_KINEMATIC_MODELS,
    dae_ID_LIBRARY_KINEMATIC_SCENES,
    dae_ID_LIBRARY_LIGHTS,
    dae_ID_LIBRARY_MATERIALS,
    dae_ID_LIBRARY_NODES,
    dae_ID_LIBRARY_PHYSICS_MATERIALS,
    dae_ID_LIBRARY_PHYSICS_MODELS,
    dae_ID_LIBRARY_PHYSICS_SCENES,
    dae_ID_LIBRARY_VISUAL_SCENES,
    dae_ID_SCENE,
    dae_ID_CONTROLLER,
    dae_ID_SKIN,
    dae_ID_MORPH,
    dae_ID_CONTROL_VERTICES,
    dae_ID_INPUT_UNSHARED,
    dae_ID_DIRECTIONAL,
    dae_ID_EVALUATE_SCENE,
    dae_ID_RENDER,
    dae_ID_TECHNIQUE,
    dae_ID_FLOAT_ARRAY,
    dae_ID_FLOAT_OR_PARAM,
    dae_ID_TARGETABLE_FLOAT,
    dae_ID_PARAM_REF,
    dae_ID_FORMULA,
    dae_ID_TECHNIQUE_COMMON_FORMULA,
    dae_ID_GEOGRAPHIC_LOCATION_ALTITUDE,
    dae_ID_GEOMETRY,
    dae_ID_CONVEX_MESH,
    dae_ID_MESH,
    dae_ID_SPLINE,
    dae_ID_BREP,
    dae_ID_IDREF_ARRAY,
    dae_ID_INPUT_SHARED,
    dae_ID_INSTANCE_CAMERA,
    dae_ID_INSTANCE_CONTROLLER,
    dae_ID_SKELETON,
    dae_ID_BIND_MATERIAL,
    dae_ID_INSTANCE_FORMULA,
    dae_ID_SETPARAM,
    dae_ID_INSTANCE_GEOMETRY,
    dae_ID_INSTANCE_LIGHT,
    dae_ID_INSTANCE_NODE,
    dae_ID_INSTANCE_VISUAL_SCENE,
    dae_ID_INT_ARRAY,
    dae_ID_JOINTS,
    dae_ID_LIBRARY_FORMULAS,
    dae_ID_LIGHT,
    dae_ID_NODE,
    dae_ID_VISUAL_SCENE,
    dae_ID_TECHNIQUE_COMMON_LIGHT,
    dae_ID_POINT,
    dae_ID_SPOT,
    dae_ID_LINES,
    dae_ID_LIST_OF_UINTS,
    dae_ID_LINESTRIPS,
    dae_ID_LIST_OF_INTS,
    dae_ID_VERTICES,
    dae_ID_POLYGONS,
    dae_ID_POLYLIST,
    dae_ID_TRIANGLES,
    dae_ID_TRIFANS,
    dae_ID_TRISTRIPS,
    dae_ID_TARGETS,
    dae_ID_NAME_ARRAY,
    dae_ID_NEWPARAM,
    dae_ID_FX_SAMPLER_COMMON,
    dae_ID_TECHNIQUE_COMMON_OPTICS,
    dae_ID_ORTHOGRAPHIC,
    dae_ID_PERSPECTIVE,
    dae_ID_PH,
    dae_ID_INSTANCE_PHYSICS_SCENE,
    dae_ID_INSTANCE_KINEMATICS_SCENE,
    dae_ID_SIDREF_ARRAY,
    dae_ID_VERTEX_WEIGHTS,
    dae_ID_TOKEN_ARRAY,
    dae_ID_TECHNIQUE_COMMON_SOURCE,
    dae_ID_TARGETABLE_FLOAT4,
    dae_ID_TECHNIQUE_HINT,
    dae_ID_UINT,
    dae_ID_TECHNIQUE_COMMON_BIND_MATERIAL,
    dae_ID_INSTANCE_MATERIAL_GEOMETRY,
    dae_ID_BLINN,
    dae_ID_COLOR_OR_TEXTURE,
    dae_ID_TEXTURE_FX,
    dae_ID_CONSTANT,
    dae_ID_EFFECT,
    dae_ID_PROFILE_COMMON,
    dae_ID_INSTANCE_IMAGE,
    dae_ID_IMAGE,
    dae_ID_RENDERABLE,
    dae_ID_IMAGE_INIT_FROM,
    dae_ID_CREATE_2D,
    dae_ID_CREATE_3D,
    dae_ID_CREATE_CUBE,
    dae_ID_IMAGE_RENDERABLE,
    dae_ID_IMAGE_HEX,
    dae_ID_INSTANCE_EFFECT,
    dae_ID_BIND_FX,
    dae_ID_BIND_VERTEX_INPUT,
    dae_ID_LAMBERT,
    dae_ID_MATERIAL,
    dae_ID_PHONG,
    dae_ID_TECHNIQUE_FX,
};

typedef union dae_transformation_element_u dae_transformation_element;
typedef struct dae_lookat_s dae_lookat;
typedef struct dae_matrix_s dae_matrix;
typedef struct dae_rotate_s dae_rotate;
typedef struct dae_scale_s dae_scale;
typedef struct dae_skew_s dae_skew;
typedef struct dae_translate_s dae_translate;
typedef struct dae_accessor_s dae_accessor;
typedef struct dae_param_s dae_param;
typedef struct dae_param_list_s dae_param_list;
typedef struct dae_ambient_s dae_ambient;
typedef struct dae_targetable_float3_s dae_targetable_float3;
typedef struct dae_animation_s dae_animation;
typedef struct dae_asset_s dae_asset;
typedef struct dae_animation_list_s dae_animation_list;
typedef struct dae_source_s dae_source;
typedef struct dae_source_list_s dae_source_list;
typedef struct dae_sampler_s dae_sampler;
typedef struct dae_sampler_list_s dae_sampler_list;
typedef struct dae_channel_s dae_channel;
typedef struct dae_channel_list_s dae_channel_list;
typedef struct dae_extra_s dae_extra;
typedef struct dae_extra_list_s dae_extra_list;
typedef struct dae_animation_clip_s dae_animation_clip;
typedef struct dae_instance_animation_s dae_instance_animation;
typedef struct dae_instance_animation_list_s dae_instance_animation_list;
typedef struct dae_annotate_s dae_annotate;
typedef struct dae_bool_s dae_bool;
typedef struct dae_bool2_s dae_bool2;
typedef struct dae_bool3_s dae_bool3;
typedef struct dae_bool4_s dae_bool4;
typedef struct dae_int_s dae_int;
typedef struct dae_int2_s dae_int2;
typedef struct dae_int3_s dae_int3;
typedef struct dae_int4_s dae_int4;
typedef struct dae_float_s dae_float;
typedef struct dae_float2_s dae_float2;
typedef struct dae_float3_s dae_float3;
typedef struct dae_float4_s dae_float4;
typedef struct dae_float2x2_s dae_float2x2;
typedef struct dae_float3x3_s dae_float3x3;
typedef struct dae_float4x4_s dae_float4x4;
typedef struct dae_string_s dae_string;
typedef struct dae_contributor_s dae_contributor;
typedef struct dae_contributor_list_s dae_contributor_list;
typedef struct dae_asset_coverage_s dae_asset_coverage;
typedef struct dae_asset_unit_s dae_asset_unit;
typedef struct dae_geographic_location_s dae_geographic_location;
typedef struct dae_bool_array_s dae_bool_array;
typedef struct dae_camera_s dae_camera;
typedef struct dae_optics_s dae_optics;
typedef struct dae_imager_s dae_imager;
typedef struct dae_COLLADA_s dae_COLLADA;
typedef struct dae_library_animation_clips_s dae_library_animation_clips;
typedef struct dae_library_animation_clips_list_s dae_library_animation_clips_list;
typedef struct dae_library_animations_s dae_library_animations;
typedef struct dae_library_animations_list_s dae_library_animations_list;
typedef struct dae_library_articulated_systems_s dae_library_articulated_systems;
typedef struct dae_library_articulated_systems_list_s dae_library_articulated_systems_list;
typedef struct dae_library_cameras_s dae_library_cameras;
typedef struct dae_library_cameras_list_s dae_library_cameras_list;
typedef struct dae_library_controllers_s dae_library_controllers;
typedef struct dae_library_controllers_list_s dae_library_controllers_list;
typedef struct dae_library_effects_s dae_library_effects;
typedef struct dae_library_effects_list_s dae_library_effects_list;
typedef struct dae_library_force_fields_s dae_library_force_fields;
typedef struct dae_library_force_fields_list_s dae_library_force_fields_list;
typedef struct dae_library_geometries_s dae_library_geometries;
typedef struct dae_library_geometries_list_s dae_library_geometries_list;
typedef struct dae_library_images_s dae_library_images;
typedef struct dae_library_images_list_s dae_library_images_list;
typedef struct dae_library_joints_s dae_library_joints;
typedef struct dae_library_joints_list_s dae_library_joints_list;
typedef struct dae_library_kinematic_models_s dae_library_kinematic_models;
typedef struct dae_library_kinematic_models_list_s dae_library_kinematic_models_list;
typedef struct dae_library_kinematic_scenes_s dae_library_kinematic_scenes;
typedef struct dae_library_kinematic_scenes_list_s dae_library_kinematic_scenes_list;
typedef struct dae_library_lights_s dae_library_lights;
typedef struct dae_library_lights_list_s dae_library_lights_list;
typedef struct dae_library_materials_s dae_library_materials;
typedef struct dae_library_materials_list_s dae_library_materials_list;
typedef struct dae_library_nodes_s dae_library_nodes;
typedef struct dae_library_nodes_list_s dae_library_nodes_list;
typedef struct dae_library_physics_materials_s dae_library_physics_materials;
typedef struct dae_library_physics_materials_list_s dae_library_physics_materials_list;
typedef struct dae_library_physics_models_s dae_library_physics_models;
typedef struct dae_library_physics_models_list_s dae_library_physics_models_list;
typedef struct dae_library_physics_scenes_s dae_library_physics_scenes;
typedef struct dae_library_physics_scenes_list_s dae_library_physics_scenes_list;
typedef struct dae_library_visual_scenes_s dae_library_visual_scenes;
typedef struct dae_library_visual_scenes_list_s dae_library_visual_scenes_list;
typedef struct dae_scene_s dae_scene;
typedef struct dae_controller_s dae_controller;
typedef struct dae_skin_s dae_skin;
typedef struct dae_morph_s dae_morph;
typedef struct dae_control_vertices_s dae_control_vertices;
typedef struct dae_input_unshared_s dae_input_unshared;
typedef struct dae_input_unshared_list_s dae_input_unshared_list;
typedef struct dae_directional_s dae_directional;
typedef struct dae_evaluate_scene_s dae_evaluate_scene;
typedef struct dae_render_s dae_render;
typedef struct dae_render_list_s dae_render_list;
typedef struct dae_technique_s dae_technique;
typedef struct dae_technique_list_s dae_technique_list;
typedef struct dae_float_array_s dae_float_array;
typedef struct dae_float_or_param_s dae_float_or_param;
typedef struct dae_targetable_float_s dae_targetable_float;
typedef struct dae_param_ref_s dae_param_ref;
typedef struct dae_formula_s dae_formula;
typedef struct dae_technique_common_formula_s dae_technique_common_formula;
typedef struct dae_geographic_location_altitude_s dae_geographic_location_altitude;
typedef struct dae_geometry_s dae_geometry;
typedef struct dae_convex_mesh_s dae_convex_mesh;
typedef struct dae_mesh_s dae_mesh;
typedef struct dae_spline_s dae_spline;
typedef struct dae_brep_s dae_brep;
typedef struct dae_IDREF_array_s dae_IDREF_array;
typedef struct dae_input_shared_s dae_input_shared;
typedef struct dae_instance_camera_s dae_instance_camera;
typedef struct dae_instance_controller_s dae_instance_controller;
typedef struct dae_skeleton_s dae_skeleton;
typedef struct dae_skeleton_list_s dae_skeleton_list;
typedef struct dae_bind_material_s dae_bind_material;
typedef struct dae_instance_formula_s dae_instance_formula;
typedef struct dae_setparam_s dae_setparam;
typedef struct dae_setparam_list_s dae_setparam_list;
typedef struct dae_instance_geometry_s dae_instance_geometry;
typedef struct dae_instance_light_s dae_instance_light;
typedef struct dae_instance_node_s dae_instance_node;
typedef struct dae_instance_visual_scene_s dae_instance_visual_scene;
typedef struct dae_int_array_s dae_int_array;
typedef struct dae_joints_s dae_joints;
typedef struct dae_animation_clip_list_s dae_animation_clip_list;
typedef struct dae_camera_list_s dae_camera_list;
typedef struct dae_controller_list_s dae_controller_list;
typedef struct dae_library_formulas_s dae_library_formulas;
typedef struct dae_formula_list_s dae_formula_list;
typedef struct dae_geometry_list_s dae_geometry_list;
typedef struct dae_light_s dae_light;
typedef struct dae_light_list_s dae_light_list;
typedef struct dae_node_s dae_node;
typedef struct dae_node_list_s dae_node_list;
typedef struct dae_visual_scene_s dae_visual_scene;
typedef struct dae_visual_scene_list_s dae_visual_scene_list;
typedef struct dae_technique_common_light_s dae_technique_common_light;
typedef struct dae_point_s dae_point;
typedef struct dae_spot_s dae_spot;
typedef struct dae_lines_s dae_lines;
typedef struct dae_input_shared_list_s dae_input_shared_list;
typedef struct dae_list_of_uints_s dae_list_of_uints;
typedef struct dae_linestrips_s dae_linestrips;
typedef struct dae_list_of_uints_list_s dae_list_of_uints_list;
typedef struct dae_list_of_ints_s dae_list_of_ints;
typedef struct dae_vertices_s dae_vertices;
typedef struct dae_lines_list_s dae_lines_list;
typedef struct dae_linestrips_list_s dae_linestrips_list;
typedef struct dae_polygons_s dae_polygons;
typedef struct dae_polygons_list_s dae_polygons_list;
typedef struct dae_polylist_s dae_polylist;
typedef struct dae_polylist_list_s dae_polylist_list;
typedef struct dae_triangles_s dae_triangles;
typedef struct dae_triangles_list_s dae_triangles_list;
typedef struct dae_trifans_s dae_trifans;
typedef struct dae_trifans_list_s dae_trifans_list;
typedef struct dae_tristrips_s dae_tristrips;
typedef struct dae_tristrips_list_s dae_tristrips_list;
typedef struct dae_targets_s dae_targets;
typedef struct dae_Name_array_s dae_Name_array;
typedef struct dae_newparam_s dae_newparam;
typedef struct dae_fx_sampler_common_s dae_fx_sampler_common;
typedef struct dae_transformation_element_list_s dae_transformation_element_list;
typedef struct dae_instance_camera_list_s dae_instance_camera_list;
typedef struct dae_instance_controller_list_s dae_instance_controller_list;
typedef struct dae_instance_geometry_list_s dae_instance_geometry_list;
typedef struct dae_instance_light_list_s dae_instance_light_list;
typedef struct dae_instance_node_list_s dae_instance_node_list;
typedef struct dae_technique_common_optics_s dae_technique_common_optics;
typedef struct dae_orthographic_s dae_orthographic;
typedef struct dae_perspective_s dae_perspective;
typedef struct dae_ph_s dae_ph;
typedef struct dae_ph_list_s dae_ph_list;
typedef struct dae_instance_physics_scene_s dae_instance_physics_scene;
typedef struct dae_instance_physics_scene_list_s dae_instance_physics_scene_list;
typedef struct dae_instance_kinematics_scene_s dae_instance_kinematics_scene;
typedef struct dae_SIDREF_array_s dae_SIDREF_array;
typedef struct dae_vertex_weights_s dae_vertex_weights;
typedef struct dae_token_array_s dae_token_array;
typedef struct dae_technique_common_source_s dae_technique_common_source;
typedef struct dae_targetable_float4_s dae_targetable_float4;
typedef struct dae_technique_hint_s dae_technique_hint;
typedef struct dae_uint_s dae_uint;
typedef struct dae_evaluate_scene_list_s dae_evaluate_scene_list;
typedef struct dae_technique_common_bind_material_s dae_technique_common_bind_material;
typedef struct dae_instance_material_geometry_s dae_instance_material_geometry;
typedef struct dae_instance_material_geometry_list_s dae_instance_material_geometry_list;
typedef struct dae_blinn_s dae_blinn;
typedef struct dae_color_or_texture_s dae_color_or_texture;
typedef struct dae_texture_fx_s dae_texture_fx;
typedef struct dae_constant_s dae_constant;
typedef struct dae_effect_s dae_effect;
typedef struct dae_annotate_list_s dae_annotate_list;
typedef struct dae_newparam_list_s dae_newparam_list;
typedef struct dae_profile_COMMON_s dae_profile_COMMON;
typedef struct dae_instance_image_s dae_instance_image;
typedef struct dae_image_s dae_image;
typedef struct dae_renderable_s dae_renderable;
typedef struct dae_image_init_from_s dae_image_init_from;
typedef struct dae_create_2d_s dae_create_2d;
typedef struct dae_create_3d_s dae_create_3d;
typedef struct dae_create_cube_s dae_create_cube;
typedef struct dae_image_renderable_s dae_image_renderable;
typedef struct dae_image_hex_s dae_image_hex;
typedef struct dae_instance_effect_s dae_instance_effect;
typedef struct dae_technique_hint_list_s dae_technique_hint_list;
typedef struct dae_bind_fx_s dae_bind_fx;
typedef struct dae_bind_fx_list_s dae_bind_fx_list;
typedef struct dae_bind_vertex_input_s dae_bind_vertex_input;
typedef struct dae_bind_vertex_input_list_s dae_bind_vertex_input_list;
typedef struct dae_lambert_s dae_lambert;
typedef struct dae_effect_list_s dae_effect_list;
typedef struct dae_image_list_s dae_image_list;
typedef struct dae_material_s dae_material;
typedef struct dae_material_list_s dae_material_list;
typedef struct dae_asset_list_s dae_asset_list;
typedef struct dae_phong_s dae_phong;
typedef struct dae_technique_fx_s dae_technique_fx;

struct dae_listbase_s
{
    dae_typeid type;
};

struct dae_typebase_s
{
    dae_typeid type;
};

struct dae_lookat_s
{
    dae_typebase daebase;
    char* sid;
    float data[9];
};

struct dae_matrix_s
{
    dae_typebase daebase;
    char* sid;
    float data[16];
};

struct dae_rotate_s
{
    dae_typebase daebase;
    char* sid;
    float data[4];
};

struct dae_scale_s
{
    dae_typebase daebase;
    char* sid;
    float data[3];
};

struct dae_skew_s
{
    dae_typebase daebase;
    char* sid;
    float data[7];
};

struct dae_translate_s
{
    dae_typebase daebase;
    char* sid;
    float data[3];
};

struct dae_param_s
{
    dae_typebase daebase;
    char* name;
    char* sid;
    char* type;
    char* semantic;
};

struct dae_param_list_s
{
    dae_listbase daebase;
    dae_param* values;
    size_t size;
};

struct dae_targetable_float3_s
{
    dae_typebase daebase;
    char* sid;
    float data[3];
};

struct dae_animation_list_s
{
    dae_listbase daebase;
    dae_animation* values;
    size_t size;
};

struct dae_source_list_s
{
    dae_listbase daebase;
    dae_source* values;
    size_t size;
};

struct dae_sampler_list_s
{
    dae_listbase daebase;
    dae_sampler* values;
    size_t size;
};

struct dae_channel_s
{
    dae_typebase daebase;
    char* source;
    char* target;
};

struct dae_channel_list_s
{
    dae_listbase daebase;
    dae_channel* values;
    size_t size;
};

struct dae_extra_list_s
{
    dae_listbase daebase;
    dae_extra* values;
    size_t size;
};

struct dae_instance_animation_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    dae_extra_list extra;
};

struct dae_instance_animation_list_s
{
    dae_listbase daebase;
    dae_instance_animation* values;
    size_t size;
};

struct dae_annotate_s
{
    dae_typebase daebase;
    dae_bool* bool_;
    dae_bool2* bool2;
    dae_bool3* bool3;
    dae_bool4* bool4;
    dae_int* int_;
    dae_int2* int2;
    dae_int3* int3;
    dae_int4* int4;
    dae_float* float_;
    dae_float2* float2;
    dae_float3* float3;
    dae_float4* float4;
    dae_float2x2* float2x2;
    dae_float3x3* float3x3;
    dae_float4x4* float4x4;
    dae_string* string;
};

struct dae_bool_s
{
    dae_typebase daebase;
    int data;
};

struct dae_bool2_s
{
    dae_typebase daebase;
    int data[2];
};

struct dae_bool3_s
{
    dae_typebase daebase;
    int data[3];
};

struct dae_bool4_s
{
    dae_typebase daebase;
    int data[4];
};

struct dae_int_s
{
    dae_typebase daebase;
    int data;
};

struct dae_int2_s
{
    dae_typebase daebase;
    int data[2];
};

struct dae_int3_s
{
    dae_typebase daebase;
    int data[3];
};

struct dae_int4_s
{
    dae_typebase daebase;
    int data[4];
};

struct dae_float_s
{
    dae_typebase daebase;
    float data;
};

struct dae_float2_s
{
    dae_typebase daebase;
    float data[2];
};

struct dae_float3_s
{
    dae_typebase daebase;
    float data[3];
};

struct dae_float4_s
{
    dae_typebase daebase;
    float data[4];
};

struct dae_float2x2_s
{
    dae_typebase daebase;
    float data[4];
};

struct dae_float3x3_s
{
    dae_typebase daebase;
    float data[9];
};

struct dae_float4x4_s
{
    dae_typebase daebase;
    float data[16];
};

struct dae_string_s
{
    dae_typebase daebase;
    char* data;
};

struct dae_contributor_s
{
    dae_typebase daebase;
    dae_string* author;
    dae_string* authoring_email;
    dae_string* authoring_website;
    dae_string* authoring_tool;
    dae_string* comments;
    dae_string* copyright;
    dae_string* source_data;
};

struct dae_contributor_list_s
{
    dae_listbase daebase;
    dae_contributor* values;
    size_t size;
};

struct dae_asset_coverage_s
{
    dae_typebase daebase;
    dae_geographic_location* geographic_location;
};

struct dae_asset_unit_s
{
    dae_typebase daebase;
    char* name;
    float meter;
};

struct dae_bool_array_s
{
    dae_typebase daebase;
    unsigned count;
    char* id;
    char* name;
    int* data;
    size_t datasize;
};

struct dae_library_animation_clips_list_s
{
    dae_listbase daebase;
    dae_library_animation_clips* values;
    size_t size;
};

struct dae_library_animations_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_animation_list animation;
    dae_extra_list extra;
};

struct dae_library_animations_list_s
{
    dae_listbase daebase;
    dae_library_animations* values;
    size_t size;
};

struct dae_library_articulated_systems_s
{
    dae_typebase daebase;
};

struct dae_library_articulated_systems_list_s
{
    dae_listbase daebase;
    dae_library_articulated_systems* values;
    size_t size;
};

struct dae_library_cameras_list_s
{
    dae_listbase daebase;
    dae_library_cameras* values;
    size_t size;
};

struct dae_library_controllers_list_s
{
    dae_listbase daebase;
    dae_library_controllers* values;
    size_t size;
};

struct dae_library_effects_list_s
{
    dae_listbase daebase;
    dae_library_effects* values;
    size_t size;
};

struct dae_library_force_fields_s
{
    dae_typebase daebase;
};

struct dae_library_force_fields_list_s
{
    dae_listbase daebase;
    dae_library_force_fields* values;
    size_t size;
};

struct dae_library_geometries_list_s
{
    dae_listbase daebase;
    dae_library_geometries* values;
    size_t size;
};

struct dae_library_images_list_s
{
    dae_listbase daebase;
    dae_library_images* values;
    size_t size;
};

struct dae_library_joints_s
{
    dae_typebase daebase;
};

struct dae_library_joints_list_s
{
    dae_listbase daebase;
    dae_library_joints* values;
    size_t size;
};

struct dae_library_kinematic_models_s
{
    dae_typebase daebase;
};

struct dae_library_kinematic_models_list_s
{
    dae_listbase daebase;
    dae_library_kinematic_models* values;
    size_t size;
};

struct dae_library_kinematic_scenes_s
{
    dae_typebase daebase;
};

struct dae_library_kinematic_scenes_list_s
{
    dae_listbase daebase;
    dae_library_kinematic_scenes* values;
    size_t size;
};

struct dae_library_lights_list_s
{
    dae_listbase daebase;
    dae_library_lights* values;
    size_t size;
};

struct dae_library_materials_list_s
{
    dae_listbase daebase;
    dae_library_materials* values;
    size_t size;
};

struct dae_library_nodes_list_s
{
    dae_listbase daebase;
    dae_library_nodes* values;
    size_t size;
};

struct dae_library_physics_materials_s
{
    dae_typebase daebase;
};

struct dae_library_physics_materials_list_s
{
    dae_listbase daebase;
    dae_library_physics_materials* values;
    size_t size;
};

struct dae_library_physics_models_s
{
    dae_typebase daebase;
};

struct dae_library_physics_models_list_s
{
    dae_listbase daebase;
    dae_library_physics_models* values;
    size_t size;
};

struct dae_library_physics_scenes_s
{
    dae_typebase daebase;
};

struct dae_library_physics_scenes_list_s
{
    dae_listbase daebase;
    dae_library_physics_scenes* values;
    size_t size;
};

struct dae_library_visual_scenes_list_s
{
    dae_listbase daebase;
    dae_library_visual_scenes* values;
    size_t size;
};

struct dae_controller_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_skin* skin;
    dae_morph* morph;
    dae_extra_list extra;
};

struct dae_input_unshared_s
{
    dae_typebase daebase;
    char* semantic;
    char* source;
};

struct dae_input_unshared_list_s
{
    dae_listbase daebase;
    dae_input_unshared* values;
    size_t size;
};

struct dae_directional_s
{
    dae_typebase daebase;
    dae_targetable_float3 color;
};

struct dae_render_s
{
    dae_typebase daebase;
};

struct dae_render_list_s
{
    dae_listbase daebase;
    dae_render* values;
    size_t size;
};

struct dae_technique_s
{
    dae_typebase daebase;
    char* profile;
    char* xmlns;
};

struct dae_technique_list_s
{
    dae_listbase daebase;
    dae_technique* values;
    size_t size;
};

struct dae_float_array_s
{
    dae_typebase daebase;
    unsigned count;
    char* id;
    char* name;
    int digits;
    int magnitude;
    float* data;
    size_t datasize;
};

struct dae_float_or_param_s
{
    dae_typebase daebase;
    dae_targetable_float* float_;
    dae_param_ref* param;
};

struct dae_targetable_float_s
{
    dae_typebase daebase;
    char* sid;
    float data;
};

struct dae_param_ref_s
{
    dae_typebase daebase;
    char* ref;
};

struct dae_technique_common_formula_s
{
    dae_typebase daebase;
    char* data;
};

struct dae_geographic_location_altitude_s
{
    dae_typebase daebase;
    char* mode;
    float data;
};

struct dae_geometry_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_convex_mesh* convex_mesh;
    dae_mesh* mesh;
    dae_spline* spline;
    dae_brep* brep;
    dae_extra_list extra;
};

struct dae_convex_mesh_s
{
    dae_typebase daebase;
};

struct dae_brep_s
{
    dae_typebase daebase;
};

struct dae_IDREF_array_s
{
    dae_typebase daebase;
    unsigned count;
    char* id;
    char* name;
    char** data;
    size_t datasize;
};

struct dae_input_shared_s
{
    dae_typebase daebase;
    unsigned offset;
    char* semantic;
    char* source;
    unsigned set;
};

struct dae_instance_camera_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    dae_extra_list extra;
};

struct dae_skeleton_s
{
    dae_typebase daebase;
    char* data;
};

struct dae_skeleton_list_s
{
    dae_listbase daebase;
    dae_skeleton* values;
    size_t size;
};

struct dae_setparam_s
{
    dae_typebase daebase;
    char* ref;
};

struct dae_setparam_list_s
{
    dae_listbase daebase;
    dae_setparam* values;
    size_t size;
};

struct dae_instance_geometry_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    dae_bind_material* bind_material;
    dae_extra_list extra;
};

struct dae_instance_light_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    dae_extra_list extra;
};

struct dae_instance_node_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    char* proxy;
    dae_extra_list extra;
};

struct dae_instance_visual_scene_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    dae_extra_list extra;
};

struct dae_int_array_s
{
    dae_typebase daebase;
    unsigned count;
    char* id;
    char* name;
    int mininclusive;
    int maxinclusive;
    int* data;
    size_t datasize;
};

struct dae_joints_s
{
    dae_typebase daebase;
    dae_input_unshared_list input;
    dae_extra_list extra;
};

struct dae_animation_clip_list_s
{
    dae_listbase daebase;
    dae_animation_clip* values;
    size_t size;
};

struct dae_camera_list_s
{
    dae_listbase daebase;
    dae_camera* values;
    size_t size;
};

struct dae_controller_list_s
{
    dae_listbase daebase;
    dae_controller* values;
    size_t size;
};

struct dae_formula_list_s
{
    dae_listbase daebase;
    dae_formula* values;
    size_t size;
};

struct dae_geometry_list_s
{
    dae_listbase daebase;
    dae_geometry* values;
    size_t size;
};

struct dae_light_list_s
{
    dae_listbase daebase;
    dae_light* values;
    size_t size;
};

struct dae_node_list_s
{
    dae_listbase daebase;
    dae_node* values;
    size_t size;
};

struct dae_visual_scene_list_s
{
    dae_listbase daebase;
    dae_visual_scene* values;
    size_t size;
};

struct dae_technique_common_light_s
{
    dae_typebase daebase;
    dae_ambient* ambient;
    dae_directional* directional;
    dae_point* point;
    dae_spot* spot;
};

struct dae_point_s
{
    dae_typebase daebase;
    dae_targetable_float3 color;
    dae_targetable_float* constant_attenuation;
    dae_targetable_float* linear_attenuation;
    dae_targetable_float* quadratic_attenuation;
};

struct dae_spot_s
{
    dae_typebase daebase;
    dae_targetable_float3 color;
    dae_targetable_float* constant_attenuation;
    dae_targetable_float* linear_attenuation;
    dae_targetable_float* quadratic_attenuation;
    dae_targetable_float* falloff_angle;
    dae_targetable_float* falloff_exponent;
};

struct dae_input_shared_list_s
{
    dae_listbase daebase;
    dae_input_shared* values;
    size_t size;
};

struct dae_list_of_uints_s
{
    dae_typebase daebase;
    unsigned* data;
    size_t datasize;
};

struct dae_list_of_uints_list_s
{
    dae_listbase daebase;
    dae_list_of_uints* values;
    size_t size;
};

struct dae_list_of_ints_s
{
    dae_typebase daebase;
    int* data;
    size_t datasize;
};

struct dae_vertices_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_input_unshared_list input;
    dae_extra_list extra;
};

struct dae_lines_list_s
{
    dae_listbase daebase;
    dae_lines* values;
    size_t size;
};

struct dae_linestrips_list_s
{
    dae_listbase daebase;
    dae_linestrips* values;
    size_t size;
};

struct dae_polygons_list_s
{
    dae_listbase daebase;
    dae_polygons* values;
    size_t size;
};

struct dae_polylist_s
{
    dae_typebase daebase;
    char* name;
    unsigned count;
    char* material;
    dae_input_shared_list input;
    dae_list_of_uints* vcount;
    dae_list_of_uints* p;
    dae_extra_list extra;
};

struct dae_polylist_list_s
{
    dae_listbase daebase;
    dae_polylist* values;
    size_t size;
};

struct dae_triangles_s
{
    dae_typebase daebase;
    char* name;
    unsigned count;
    char* material;
    dae_input_shared_list input;
    dae_list_of_uints* p;
    dae_extra_list extra;
};

struct dae_triangles_list_s
{
    dae_listbase daebase;
    dae_triangles* values;
    size_t size;
};

struct dae_trifans_s
{
    dae_typebase daebase;
    char* name;
    unsigned count;
    char* material;
    dae_input_shared_list input;
    dae_list_of_uints_list p;
    dae_extra_list extra;
};

struct dae_trifans_list_s
{
    dae_listbase daebase;
    dae_trifans* values;
    size_t size;
};

struct dae_tristrips_s
{
    dae_typebase daebase;
    char* name;
    unsigned count;
    char* material;
    dae_input_shared_list input;
    dae_list_of_uints_list p;
    dae_extra_list extra;
};

struct dae_tristrips_list_s
{
    dae_listbase daebase;
    dae_tristrips* values;
    size_t size;
};

struct dae_targets_s
{
    dae_typebase daebase;
    dae_input_unshared_list input;
    dae_extra_list extra;
};

struct dae_Name_array_s
{
    dae_typebase daebase;
    unsigned count;
    char* id;
    char* name;
    char** data;
    size_t datasize;
};

struct dae_newparam_s
{
    dae_typebase daebase;
    char* sid;
    dae_annotate* annotate;
    dae_string* semantic;
    dae_string* modifier;
    dae_float* float_;
    dae_float2* float2;
    dae_float3* float3;
    dae_float4* float4;
    dae_fx_sampler_common* sampler2D;
};

struct dae_fx_sampler_common_s
{
    dae_typebase daebase;
    dae_instance_image* instance_image;
    dae_string* wrap_s;
    dae_string* wrap_t;
    dae_string* wrap_p;
    dae_string* minfilter;
    dae_string* magfilter;
    dae_string* mipfilter;
    dae_float4* border_color;
    dae_uint* mip_max_level;
    dae_uint* mip_min_level;
    dae_float* mip_bias;
    dae_uint* max_anisotropy;
    dae_extra_list extra;
};

struct dae_transformation_element_list_s
{
    dae_listbase daebase;
    dae_transformation_element* values;
    size_t size;
};

struct dae_instance_camera_list_s
{
    dae_listbase daebase;
    dae_instance_camera* values;
    size_t size;
};

struct dae_instance_controller_list_s
{
    dae_listbase daebase;
    dae_instance_controller* values;
    size_t size;
};

struct dae_instance_geometry_list_s
{
    dae_listbase daebase;
    dae_instance_geometry* values;
    size_t size;
};

struct dae_instance_light_list_s
{
    dae_listbase daebase;
    dae_instance_light* values;
    size_t size;
};

struct dae_instance_node_list_s
{
    dae_listbase daebase;
    dae_instance_node* values;
    size_t size;
};

struct dae_technique_common_optics_s
{
    dae_typebase daebase;
    dae_orthographic* orthographic;
    dae_perspective* perspective;
};

struct dae_orthographic_s
{
    dae_typebase daebase;
    dae_targetable_float* xmag;
    dae_targetable_float* ymag;
    dae_extra_list extra;
    dae_targetable_float* aspect_ratio;
    dae_targetable_float znear;
    dae_targetable_float zfar;
};

struct dae_perspective_s
{
    dae_typebase daebase;
    dae_targetable_float* xfov;
    dae_targetable_float* yfov;
    dae_extra_list extra;
    dae_targetable_float* aspect_ratio;
    dae_targetable_float znear;
    dae_targetable_float zfar;
};

struct dae_ph_s
{
    dae_typebase daebase;
    dae_list_of_uints* p;
    dae_list_of_uints* h;
};

struct dae_ph_list_s
{
    dae_listbase daebase;
    dae_ph* values;
    size_t size;
};

struct dae_instance_physics_scene_s
{
    dae_typebase daebase;
};

struct dae_instance_physics_scene_list_s
{
    dae_listbase daebase;
    dae_instance_physics_scene* values;
    size_t size;
};

struct dae_instance_kinematics_scene_s
{
    dae_typebase daebase;
};

struct dae_SIDREF_array_s
{
    dae_typebase daebase;
    unsigned count;
    char* id;
    char* name;
    char** data;
    size_t datasize;
};

struct dae_vertex_weights_s
{
    dae_typebase daebase;
    unsigned count;
    dae_input_shared_list input;
    dae_list_of_uints* vcount;
    dae_list_of_ints* v;
    dae_extra_list extra;
};

struct dae_token_array_s
{
    dae_typebase daebase;
};

struct dae_targetable_float4_s
{
    dae_typebase daebase;
    char* sid;
    float data[4];
};

struct dae_technique_hint_s
{
    dae_typebase daebase;
    char* platform;
    char* ref;
    char* profile;
};

struct dae_uint_s
{
    dae_typebase daebase;
    unsigned data;
};

struct dae_evaluate_scene_list_s
{
    dae_listbase daebase;
    dae_evaluate_scene* values;
    size_t size;
};

struct dae_instance_material_geometry_list_s
{
    dae_listbase daebase;
    dae_instance_material_geometry* values;
    size_t size;
};

struct dae_blinn_s
{
    dae_typebase daebase;
    dae_color_or_texture* emission;
    dae_color_or_texture* ambient;
    dae_color_or_texture* diffuse;
    dae_color_or_texture* specular;
    dae_float_or_param* shininess;
    dae_color_or_texture* reflective;
    dae_float_or_param* reflectivity;
    dae_color_or_texture* transparent;
    dae_float_or_param* transparency;
    dae_float_or_param* index_of_refraction;
};

struct dae_color_or_texture_s
{
    dae_typebase daebase;
    char* opaque;
    dae_float4* color;
    dae_param_ref* param;
    dae_texture_fx* texture;
};

struct dae_texture_fx_s
{
    dae_typebase daebase;
    char* texture;
    char* texcoord;
    dae_extra_list extra;
};

struct dae_constant_s
{
    dae_typebase daebase;
    dae_color_or_texture* emission;
    dae_color_or_texture* reflective;
    dae_float_or_param* reflectivity;
    dae_color_or_texture* transparent;
    dae_float_or_param* transparency;
    dae_float_or_param* index_of_refraction;
};

struct dae_annotate_list_s
{
    dae_listbase daebase;
    dae_annotate* values;
    size_t size;
};

struct dae_newparam_list_s
{
    dae_listbase daebase;
    dae_newparam* values;
    size_t size;
};

struct dae_instance_image_s
{
    dae_typebase daebase;
};

struct dae_image_s
{
    dae_typebase daebase;
    char* id;
    char* sid;
    char* name;
    dae_asset* asset;
    dae_renderable* renderable;
    dae_image_init_from* init_from;
    dae_create_2d* create_2d;
    dae_create_3d* create_3d;
    dae_create_cube* create_cube;
    dae_extra_list extra;
};

struct dae_renderable_s
{
    dae_typebase daebase;
};

struct dae_image_init_from_s
{
    dae_typebase daebase;
    int mips_gen_erate;
    char* data;
    dae_string* ref;
    dae_image_hex* hex;
};

struct dae_create_2d_s
{
    dae_typebase daebase;
};

struct dae_create_3d_s
{
    dae_typebase daebase;
};

struct dae_create_cube_s
{
    dae_typebase daebase;
};

struct dae_image_renderable_s
{
    dae_typebase daebase;
    int share;
};

struct dae_image_hex_s
{
    dae_typebase daebase;
    char* format;
    char* data;
};

struct dae_technique_hint_list_s
{
    dae_listbase daebase;
    dae_technique_hint* values;
    size_t size;
};

struct dae_bind_fx_s
{
    dae_typebase daebase;
};

struct dae_bind_fx_list_s
{
    dae_listbase daebase;
    dae_bind_fx* values;
    size_t size;
};

struct dae_bind_vertex_input_s
{
    dae_typebase daebase;
};

struct dae_bind_vertex_input_list_s
{
    dae_listbase daebase;
    dae_bind_vertex_input* values;
    size_t size;
};

struct dae_lambert_s
{
    dae_typebase daebase;
    dae_color_or_texture* emission;
    dae_color_or_texture* ambient;
    dae_color_or_texture* diffuse;
    dae_color_or_texture* reflective;
    dae_float_or_param* reflectivity;
    dae_color_or_texture* transparent;
    dae_float_or_param* transparency;
    dae_float_or_param* index_of_refraction;
};

struct dae_effect_list_s
{
    dae_listbase daebase;
    dae_effect* values;
    size_t size;
};

struct dae_image_list_s
{
    dae_listbase daebase;
    dae_image* values;
    size_t size;
};

struct dae_material_list_s
{
    dae_listbase daebase;
    dae_material* values;
    size_t size;
};

struct dae_asset_list_s
{
    dae_listbase daebase;
    dae_asset* values;
    size_t size;
};

struct dae_phong_s
{
    dae_typebase daebase;
    dae_color_or_texture* emission;
    dae_color_or_texture* ambient;
    dae_float_or_param* diffuse;
    dae_color_or_texture* specular;
    dae_color_or_texture* shininess;
    dae_color_or_texture* reflective;
    dae_float_or_param* reflectivity;
    dae_color_or_texture* transparent;
    dae_float_or_param* transparency;
    dae_float_or_param* index_of_refraction;
};

struct dae_technique_fx_s
{
    dae_typebase daebase;
    dae_asset* asset;
    dae_constant* constant;
    dae_lambert* lambert;
    dae_phong* phong;
    dae_blinn* blinn;
};

union dae_transformation_element_u
{
    dae_typebase daebase;
    dae_lookat lookat;
    dae_matrix matrix;
    dae_rotate rotate;
    dae_scale scale;
    dae_skew skew;
    dae_translate translate;
};

struct dae_accessor_s
{
    dae_typebase daebase;
    unsigned count;
    unsigned offset;
    char* source;
    unsigned stride;
    dae_param_list param;
};

struct dae_ambient_s
{
    dae_typebase daebase;
    dae_targetable_float3 color;
};

struct dae_animation_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_animation_list animation;
    dae_source_list source;
    dae_sampler_list sampler;
    dae_channel_list channel;
    dae_extra_list extra;
};

struct dae_asset_s
{
    dae_typebase daebase;
    dae_contributor_list contributor;
    dae_asset_coverage* coverage;
    dae_string created;
    dae_string* keywords;
    dae_string modified;
    dae_string* revision;
    dae_string* subject;
    dae_string* title;
    dae_asset_unit* unit;
    dae_string* up_axis;
    dae_extra_list extra;
};

struct dae_source_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_bool_array* bool_array;
    dae_float_array* float_array;
    dae_IDREF_array* IDREF_array;
    dae_int_array* int_array;
    dae_Name_array* Name_array;
    dae_SIDREF_array* SIDREF_array;
    dae_token_array* token_array;
    dae_technique_common_source* technique_common;
    dae_technique_list technique;
};

struct dae_sampler_s
{
    dae_typebase daebase;
    char* id;
    char* pre_behavior;
    char* post_behavior;
    dae_input_shared_list input;
};

struct dae_extra_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    char* type;
    dae_asset* asset;
    dae_technique_list technique;
};

struct dae_animation_clip_s
{
    dae_typebase daebase;
    char* id;
    float start;
    float end;
    char* name;
    dae_asset* asset;
    dae_instance_animation_list instance_animation;
    dae_extra_list extra;
};

struct dae_geographic_location_s
{
    dae_typebase daebase;
    dae_float longitude;
    dae_float latitude;
    dae_geographic_location_altitude altitude;
};

struct dae_optics_s
{
    dae_typebase daebase;
    dae_technique_common_optics technique_common;
    dae_technique_list technique;
    dae_extra_list extra;
};

struct dae_imager_s
{
    dae_typebase daebase;
    dae_technique_list technique;
    dae_extra_list extra;
};

struct dae_COLLADA_s
{
    dae_typebase daebase;
    char* version;
    char* xmlns;
    char* base;
    dae_asset asset;
    dae_library_animation_clips_list library_animation_clips;
    dae_library_animations_list library_animations;
    dae_library_articulated_systems_list library_articulated_systems;
    dae_library_cameras_list library_cameras;
    dae_library_controllers_list library_controllers;
    dae_library_effects_list library_effects;
    dae_library_force_fields_list library_force_fields;
    dae_library_geometries_list library_geometries;
    dae_library_images_list library_images;
    dae_library_joints_list library_joints;
    dae_library_kinematic_models_list library_kinematic_models;
    dae_library_kinematic_scenes_list library_kinematic_scenes;
    dae_library_lights_list library_lights;
    dae_library_materials_list library_materials;
    dae_library_nodes_list library_nodes;
    dae_library_physics_materials_list library_physics_materials;
    dae_library_physics_models_list library_physics_models;
    dae_library_physics_scenes_list library_physics_scenes;
    dae_library_visual_scenes_list library_visual_scenes;
    dae_scene* scene;
    dae_extra_list extra;
};

struct dae_library_animation_clips_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_animation_clip_list animation_clip;
    dae_extra_list extra;
};

struct dae_library_cameras_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_camera_list camera;
    dae_extra_list extra;
};

struct dae_library_controllers_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_controller_list controller;
    dae_extra_list extra;
};

struct dae_library_effects_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_effect_list effect;
    dae_extra_list extra;
};

struct dae_library_geometries_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_geometry_list geometry;
    dae_extra_list extra;
};

struct dae_library_images_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_image_list image;
    dae_extra_list extra;
};

struct dae_library_lights_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_light_list light;
    dae_extra_list extra;
};

struct dae_library_materials_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_material_list material;
    dae_extra_list extra;
};

struct dae_library_nodes_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_node_list node;
    dae_extra_list extra;
};

struct dae_library_visual_scenes_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_visual_scene_list visual_scene;
    dae_extra_list extra;
};

struct dae_scene_s
{
    dae_typebase daebase;
    dae_instance_physics_scene_list instance_physics_scene;
    dae_instance_visual_scene* instance_visual_scene;
    dae_instance_kinematics_scene* instance_kinematics_scene;
    dae_extra_list extra;
};

struct dae_skin_s
{
    dae_typebase daebase;
    char* source;
    dae_float4x4* bind_shape_matrix;
    dae_source_list source_;
    dae_joints joints;
    dae_vertex_weights vertex_weights;
    dae_extra_list extra;
};

struct dae_morph_s
{
    dae_typebase daebase;
    char* sid;
    char* method;
    dae_source_list source;
    dae_targets targets;
    dae_extra_list extra;
};

struct dae_control_vertices_s
{
    dae_typebase daebase;
    dae_input_unshared_list input;
    dae_extra_list extra;
};

struct dae_evaluate_scene_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    char* sid;
    int enable;
    dae_asset* asset;
    dae_render_list render;
    dae_extra_list extra;
};

struct dae_formula_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    char* sid;
    dae_float_or_param target;
    dae_technique_common_formula technique_common;
    dae_technique_list technique;
};

struct dae_mesh_s
{
    dae_typebase daebase;
    dae_source_list source;
    dae_vertices vertices;
    dae_lines_list lines;
    dae_linestrips_list linestrips;
    dae_polygons_list polygons;
    dae_polylist_list polylist;
    dae_triangles_list triangles;
    dae_trifans_list trifans;
    dae_tristrips_list tristrips;
    dae_extra_list extra;
};

struct dae_spline_s
{
    dae_typebase daebase;
    int closed;
    dae_source_list source;
    dae_control_vertices control_vertices;
    dae_extra_list extra;
};

struct dae_instance_controller_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    dae_skeleton_list skeleton;
    dae_bind_material* bind_material;
    dae_extra_list extra;
};

struct dae_instance_formula_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    dae_setparam_list setparam;
    dae_extra_list extra;
};

struct dae_library_formulas_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_formula_list formula;
    dae_extra_list extra;
};

struct dae_light_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_technique_common_light technique_common;
    dae_technique_list technique;
    dae_extra_list extra;
};

struct dae_node_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    char* sid;
    char* type;
    char* layer;
    dae_asset* asset;
    dae_transformation_element_list transformation_element;
    dae_instance_camera_list instance_camera;
    dae_instance_controller_list instance_controller;
    dae_instance_geometry_list instance_geometry;
    dae_instance_light_list instance_light;
    dae_instance_node_list instance_node;
    dae_node_list node;
    dae_extra_list extra;
};

struct dae_visual_scene_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_node_list node;
    dae_evaluate_scene_list evaluate_scene;
    dae_extra_list extra;
};

struct dae_lines_s
{
    dae_typebase daebase;
    char* name;
    unsigned count;
    char* material;
    dae_input_shared_list input;
    dae_list_of_uints* p;
    dae_extra_list extra;
};

struct dae_linestrips_s
{
    dae_typebase daebase;
    char* name;
    unsigned count;
    char* material;
    dae_input_shared_list input;
    dae_list_of_uints_list p;
    dae_extra_list extra;
};

struct dae_polygons_s
{
    dae_typebase daebase;
    unsigned count;
    char* material;
    char* name;
    dae_input_shared_list input;
    dae_list_of_uints_list p;
    dae_ph_list ph;
    dae_extra_list extra;
};

struct dae_technique_common_source_s
{
    dae_typebase daebase;
    dae_accessor accessor;
};

struct dae_technique_common_bind_material_s
{
    dae_typebase daebase;
    dae_instance_material_geometry_list instance_material;
};

struct dae_instance_material_geometry_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* target;
    char* symbol;
    dae_bind_fx_list bind;
    dae_bind_vertex_input_list bind_vertex_input;
    dae_extra_list extra;
};

struct dae_effect_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_annotate_list annotate;
    dae_newparam_list newparam;
    dae_profile_COMMON* profile_COMMON;
    dae_extra_list extra;
};

struct dae_profile_COMMON_s
{
    dae_typebase daebase;
    dae_asset* asset;
    dae_newparam_list newparam;
    dae_technique_fx technique;
    dae_extra_list extra;
};

struct dae_instance_effect_s
{
    dae_typebase daebase;
    char* sid;
    char* name;
    char* url;
    dae_technique_hint_list technique_hint;
    dae_setparam_list setparam;
    dae_extra_list extra;
};

struct dae_material_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset_list asset;
    dae_instance_effect instance_effect;
    dae_extra_list extra;
};

struct dae_camera_s
{
    dae_typebase daebase;
    char* id;
    char* name;
    dae_asset* asset;
    dae_optics optics;
    dae_imager* imager;
    dae_extra_list extra;
};

struct dae_bind_material_s
{
    dae_typebase daebase;
    dae_param_list param;
    dae_technique_common_bind_material technique_common;
    dae_technique_list technique;
    dae_extra_list extra;
};


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

