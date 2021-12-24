import pyzed.sl as sl
import math
import ctypes
from . import ObjectInfo as Info
# Custom Filters can be implemented in this file and called upon by other functions. 
# I made a simple filter based on distance and work out a weight for each object.
# For the purpose of this demo, I implemented no object filtering because it is inconsistent and not really scientific either. 

# Calculate smallest distance to user and time to reach that point given velocity of object
def outdoor_filter(obj):
    thrs = 0
    dst = 0

    # Set threashold values, unit in meters
    if Info.is_car(obj) or Info.is_bus(obj):
        thrs = 6
    else:
        thrs = 3

    # Find Velocity Vector 
    if obj.velocity[0]*obj.position[0] < 0 and obj.velocity[1]*obj.position[2] < 0:
        a = obj.velocity[1]/obj.velocity[0]
        b = obj.position[1]-obj.position[0]*a
        
        # Find intersect between X^2+Y^2 = R^2 and the previous line
        theta = ((2*a*b)**2)-4*(a**2+1)*(b**2-thrs**2)

        if theta >= 0:
            x1 = (-2*a*b+math.sqrt(theta))/(2*a**2+2)
            x2 = (-2*a*b-math.sqrt(theta))/(2*a**2+2)
            if abs(obj.position[0]-x1) < abs(obj.position[0]-x2):
                dst = math.sqrt((obj.position[0]-x1)**2+(obj.position[1]-a*x1-b)**2)
            else:
                dst = math.sqrt((obj.position[0]-x2)**2+(obj.position[1]-a*x2-b)**2)
            time_to_thrs = dst / Info.get_obj_velocity(obj)
            if time_to_thrs < 5:
                return time_to_thrs / 5
    return 0

# Does not filter detected objects and render everything seen by camera. 
def no_filter(obj):
    # No Filtering
    return 1
