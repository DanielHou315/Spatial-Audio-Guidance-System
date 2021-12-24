import pyzed.sl as sl
import playsound
from datetime import datetime
import time
import ctypes
import video_module.AudioInterface as Audio


# Globals 

frames = 0
past_milli_time = 0
detection_classes = [sl.OBJECT_CLASS.PERSON,sl.OBJECT_CLASS.VEHICLE,sl.OBJECT_CLASS.ANIMAL]



# CAMERA PREPARATIONS

# This step sets up the camera before it is even opened up.

# Define a Camera object 
Zedcam = sl.Camera() 

# Init Parameters
init_params = sl.InitParameters(camera_resolution = sl.RESOLUTION.HD720,              
    camera_fps = 30,                                                        # To acquire better FPS, we sacrifice some Resolution
    depth_mode = sl.DEPTH_MODE.PERFORMANCE,
    coordinate_system = sl.COORDINATE_SYSTEM.RIGHT_HANDED_Z_UP,
    coordinate_units = sl.UNIT.METER,
    sdk_verbose = True,
    depth_minimum_distance = 0.4,
    depth_maximum_distance = 35.0)

# Runtime Parameters
runtime_params = sl.RuntimeParameters(sensing_mode = sl.SENSING_MODE.STANDARD)

# Tracking position
pos_tracking_params = sl.PositionalTrackingParameters()            
pos_tracking_params.enable_area_memory = False
pos_tracking_params.set_floor_as_origin = True
pos_tracking_params.set_as_static = True

# Batch Parameters
batch_params = sl.BatchParameters(enable = True,
    id_retention_time = 3,
    batch_duration = 0.25)	

# Detection Parameters
detection_params = sl.ObjectDetectionParameters(image_sync = True,
    enable_tracking = True,
    enable_mask_output = False,
    max_range = 30,
    batch_trajectories_parameters = batch_params)

# Setup Runtime parameters for object detection
detection_params_runtime = sl.ObjectDetectionRuntimeParameters(detection_confidence_threshold = 65,
    object_class_filter = detection_classes)




# I recorded camera feed live during this Demo
# Setup recording parameters
start_time = datetime.now()
now = start_time.strftime("%b-%d-%H-%M-%S")
file_name = "Recordings/Recording_at_"+now+".svo"
record_params = sl.RecordingParameters(file_name, sl.SVO_COMPRESSION_MODE.H265)

# Setup Variables needed by ZED
image_map_l = sl.Mat()                                      # Create L&R image mapping
point_cloud_map = sl.Mat()                                  # Create point cloud mapping
detected_objects = sl.Objects()                             # Create objects holder
Zed_pose = sl.Pose()




# Miscellaneous Functions

# Return current time in milliseconds
def current_milli_time():
    return round(time.time() * 1000)

# Display how many FPS the program runs at
def fps_recorder():
    global frames
    global past_milli_time
    if current_milli_time() - past_milli_time >= 1000 and current_milli_time() - past_milli_time < 2000:
        print("FPS: ", frames)
        frames = 0
        past_milli_time = current_milli_time()
    frames += 1
    
# Initialize the Camera, will be called only once in main
def camera_init():
    open_err = Zedcam.open(init_params)
    # Report if Error
    handle_video_error("opening camera",open_err)
    # Confirm success in Opening Camera
    print("Camera opened Successfully!")
    # Enable Tracking Feature
    err_track = Zedcam.enable_positional_tracking(pos_tracking_params)
    handle_video_error("enabling positional tracking",err_track)
    print("Tracking Enabled!")                                   
    # Load Module for Object Detection
    print("Loading Module for Object Detection")
    err_obj_detect = Zedcam.enable_object_detection(detection_params)
    handle_video_error("obj detection enabling",err_obj_detect)
    print("Object Detection Module Successfully Loaded")

# Enable Recording on ZED
def enable_recording():
    enable_record_err = Zedcam.enable_recording(record_params)
    if enable_record_err != sl.ERROR_CODE.SUCCESS:
        handle_video_error("Enabling Recording", enable_record_err)

# This retrieves a frame from the ZED camera
def ZED_retrieve():
    global detected_objects
    global runtime_params
    global image_map_l
    global point_cloud_map
    # Grab an image
    if Zedcam.grab(runtime_params) == sl.ERROR_CODE.SUCCESS: 
        # Retrieve left image
        Zedcam.retrieve_image(image_map_l, sl.VIEW.LEFT)
        # Retrieve point cloud measurement
        Zedcam.retrieve_measure(point_cloud_map, sl.MEASURE.XYZ)
        # Retrieve Objects
        detection_err = Zedcam.retrieve_objects(detected_objects, detection_params_runtime)

# Deal with erros thrown by ZED
def handle_video_error(step,err):
    if err != sl.ERROR_CODE.SUCCESS:
        print("There is an error during video {0}: {1}".format(step,err))
        playsound.playsound("sound_source/procedure/camera_fail.wav")
        camera_close()
        Audio.audio_close()
        exit()
    return

# Close Camera
def camera_close():
    print("Closing ZED 2 Camera")
    Zedcam.disable_positional_tracking()
    Zedcam.close()
    return





# The Main Function

def main():
    # Declare Globals
    global frames
    global past_milli_time

    # Initialize Video and Audio
    print("Starting Program")
    playsound.playsound("sound_source/procedure/startup.wav")
    Audio.init()
    camera_init()
    playsound.playsound("sound_source/procedure/prepared.wav")
    frames = 0

    # Enable Recording for the Demo
    enable_recording()

    # Main Process Starts
    try: 
        # Set current time
        past_milli_time = current_milli_time()
        while True:
            # Monitor the FPS of Program (ie. The Performance)
            fps_recorder()
            # Grab a new frame and call audio functions
            ZED_retrieve()

            # If no object detected, delete everything -- Retention objects (trying to be detected again) are counted as detected. 
            if len(detected_objects.object_list) == 0:
                Audio.delete_all_sources()
                continue

            # Otherwise, process each object in OpenAL
            for obj in detected_objects.object_list:
                Audio.process_object(obj)
            Audio.cleanup_current_sources()

    # If keyboard interrupted (Ctrl-C), the program will close.
    except KeyboardInterrupt:
        print("Pressed Ctrl-C, Closing Program")
        pass

    # Close Program
    camera_close()
    Audio.audio_close()
    playsound.playsound("sound_source/procedure/shutdown.wav")
    print("Program Ended! Thank you!")
    return



# Run Main
if __name__ == "__main__":
    main()