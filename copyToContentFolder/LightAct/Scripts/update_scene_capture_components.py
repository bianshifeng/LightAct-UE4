import unreal
import json
import os
import sys


ADDITIONAL_OBJECTS_LABEL = 'LightAct_3D_View_additional_objects'
PROJECTORS_LABEL = 'LightAct_3D_View_Projector_Object'
GENERAL_OBJECT_DATA_LABEL = 'General_object_data'


def read_projectors_info(input_filename):
    with open(input_filename, 'r') as inp:
        file_as_json = json.loads(inp.read())

    return file_as_json[ADDITIONAL_OBJECTS_LABEL][PROJECTORS_LABEL]


def find_projector_actors():
    AssetTools = unreal.AssetToolsHelpers.get_asset_tools()        
    return dict((level_actor.get_actor_label(), level_actor) for level_actor in unreal.EditorLevelLibrary.get_all_level_actors())


def find_render_targets():
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    render_targets = asset_registry.get_assets_by_class('TextureRenderTarget2D', False)
    return dict((str(render_target.package_name).split('/')[-1], render_target) for render_target in render_targets)   

    
def get_projector_render_target(projector_name, render_targets_info):
    render_target_name = projector_name + '_RT'
    
    if render_target_name in render_targets_info:
        render_target = render_targets_info[render_target_name].get_asset()
        
        return render_target
        
    return None
    

def set_up_projector(projector_info, projector_actor, projector_render_target):
    projector_name = projector_info[GENERAL_OBJECT_DATA_LABEL]['name']
    
    projector_general_data = projector_info[GENERAL_OBJECT_DATA_LABEL]
           
    location_x = projector_general_data['location_x']
    location_y = projector_general_data['location_y']
    location_z = projector_general_data['location_z']
    projector_location = unreal.Vector(location_x, location_y, location_z) * unreal.Vector(location_scale, location_scale, location_scale)
    
    # swap coordinates since Lightact has Y up, -Z forward , X right coordinate system
    projector_location = unreal.Vector(-projector_location.z, projector_location.x, projector_location.y)


    rotation_roll = projector_general_data['rot_roll']
    rotation_pitch = projector_general_data['rot_pitch']
    rotation_yaw = projector_general_data['rot_yaw']
    
    # swap roll and pitch since Lightact has Gimbal lock problem
    projector_rotation = unreal.Rotator(rotation_pitch, rotation_roll, -rotation_yaw) 
  
    
    projector_transform = unreal.Transform(projector_location, projector_rotation, unreal.Vector(1, 1, 1))
    
    
    projector_actor.set_actor_transform(projector_transform, False, True)
    
    
    projector_resolution_x = projector_info['Projector_resolution_x']
    projector_resolution_y = projector_info['Projector_resolution_y']
    projector_vertical_fov_deg = float(projector_info['Projector_vertical_fov_deg'])
    
    
    projector_horizontal_fov_deg = projector_vertical_fov_deg * (projector_resolution_x / projector_resolution_y)
    
    capture_component2d = projector_actor.capture_component2d
    capture_component2d.fov_angle = projector_horizontal_fov_deg
    
    
    # set params given render target exists 
    if projector_render_target:
        projector_render_target.set_editor_property('size_x', projector_resolution_x)
        projector_render_target.set_editor_property('size_y', projector_resolution_y)
        
        capture_component2d.texture_target = projector_render_target
    
    
def set_up_projectors(projectors_info, projector_actors, render_targets_info):
    for projector_info in projectors_info:
        projector_name = projector_info[GENERAL_OBJECT_DATA_LABEL]['name']
        
        if projector_name not in projector_actors:
            continue
            
        print('Setting params for projector {0}'.format(projector_name))
            
        projector_actor = projector_actors[projector_name]
        projector_render_target = get_projector_render_target(projector_name, render_targets_info)
            
        set_up_projector(projector_info, projector_actor, projector_render_target)
        
        
    

def set_up_scene(input_filename, location_scale):
    print("PROJECTORS SCRIPT STARTED...")
   
    
    projectors_info = read_projectors_info(input_filename)
    projector_actors = find_projector_actors()
    render_targets_info = find_render_targets()
    print(render_targets_info)
    
    set_up_projectors(projectors_info, projector_actors, render_targets_info)
            

    unreal.EditorLevelLibrary.save_current_level()
            
    print("PROJECTORS SCRIPT FINISHED!")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print('Please path to .la file and location scale modifier')
        exit(-1)
        

    input_filename = sys.argv[1]
    location_scale = float(sys.argv[2])    


    set_up_scene(input_filename, location_scale)



