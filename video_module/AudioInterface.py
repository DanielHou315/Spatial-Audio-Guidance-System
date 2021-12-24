from . import ObjectInfo as Info
from . import ObjectFilter as Filter
import pyzed.sl as sl
import math
import ctypes
audio_lib = ctypes.cdll.LoadLibrary("./audio_module/audiolib.so")


# Update information about an object in OpenAL
def update_source(id, type, pos, vel, state):
    if audio_lib.is_new_source(id):
        audio_lib.create_source(ctypes.c_int(id),ctypes.c_int(type))
    audio_lib.add_current_source(ctypes.c_int(id))
    # audio_lib.update_source_type(ctypes.c_int(id),ctypes.c_int(type))
    audio_lib.update_source_position(ctypes.c_int(id),ctypes.c_float(pos[0]),ctypes.c_float(pos[1]),ctypes.c_float(pos[2]))
    audio_lib.update_source_velocity(ctypes.c_int(id),ctypes.c_float(vel[0]),ctypes.c_float(vel[1]),ctypes.c_float(vel[2]))
    # audio_lib.update_source_state(ctypes.c_int(id),ctypes.c_bool(state))



# APIs for Main 

# Initialize OpenAL
def init():
    audio_lib.audio_init()
    audio_lib.SetAttunation(4)
    audio_lib.create_buffers()

# Load buffers into memory
def load_buffers():
    audio_lib.load_buffers()

# Do filtering and determine if an object should be rendered by OpenAL or not. 
# For the purpose of this demo, we implemented no filters. 
# Proper filtering algorithms are potential research areas for the future. 
def process_object(obj):
    if Filter.no_filter(obj):
        update_source(obj.id, Info.get_obj_type(obj),obj.position, obj.velocity, Info.get_obj_state(obj))

# Cleanup current_sources list. 
def cleanup_current_sources():
    audio_lib.cleanup_current_source()

# Delete all sources rendered in OpenAL. 
def delete_all_sources():
    audio_lib.delete_all_sources()

# Close OpenAL, at the end of the program. 
def audio_close():
    audio_lib.audio_cleanup()