import pyzed.sl as sl
import math

obj_classes = [sl.OBJECT_CLASS.PERSON,
    sl.OBJECT_CLASS.VEHICLE,
    sl.OBJECT_CLASS.BAG,
    sl.OBJECT_CLASS.ANIMAL]

vehicle_subclasses = [sl.OBJECT_SUBCLASS.BICYCLE,
    sl.OBJECT_SUBCLASS.CAR,
    sl.OBJECT_SUBCLASS.MOTORBIKE,
    sl.OBJECT_SUBCLASS.BUS,
    sl.OBJECT_SUBCLASS.TRUCK]
animal_subclasses = [sl.OBJECT_SUBCLASS.CAT,
    sl.OBJECT_SUBCLASS.DOG]


# Get Object Types
# Is Warning
def is_warning(obj):
    if get_obj_type(obj) == 0: return True
    return False
# Is Person
def is_person(obj):
    if get_obj_type(obj) == 1: return True
    return False
# Is Bike or Motorcycle
def is_bike(obj):
    obj_type = get_obj_type(obj)
    if obj_type == 2 or obj_type == 4: return True
    return False
# Is Car
def is_car(obj):
    obj_type = get_obj_type(obj)
    if obj_type == 3: return True
    return False
# Is Bus or Truck
def is_bus(obj):
    obj_type = get_obj_type(obj)
    if obj_type == 5 or obj_type == 6: return True
    return False
# Is Cat
def is_cat(obj):
    if get_obj_type(obj) == 8: return True
    return False
# Is Dog
def is_dog(obj):
    if get_obj_type(obj) == 9: return True
    return False



# Get General Info on the Object
def get_obj_type(obj):
    if obj.label == sl.OBJECT_CLASS.PERSON:
        return 1                                            # 1 Person
    elif obj.sublabel == sl.OBJECT_SUBCLASS.BICYCLE or obj.sublabel == sl.OBJECT_SUBCLASS.MOTORBIKE:
        return 2
    elif obj.sublabel == sl.OBJECT_SUBCLASS.CAR:
        return 3
    elif obj.sublabel == sl.OBJECT_SUBCLASS.BUS or obj.sublabel == sl.OBJECT_SUBCLASS.TRUCK:
        return 4
    elif obj.sublabel == sl.OBJECT_SUBCLASS.CAR:
        return 5
    elif obj.sublabel == sl.OBJECT_SUBCLASS.CAT:
        return 6
    elif obj.sublabel == sl.OBJECT_SUBCLASS.DOG:
        return 7
    return 0

# Get Object State (moving or idle)
def get_obj_state(obj):
    if obj.action_state == sl.OBJECT_ACTION_STATE.IDLE: return False
    return True

# Get Object Distance to User, get a 3D Vector and returns a numeric value considering only X, Y axes. 
def get_obj_distance(obj):
    return math.sqrt(obj.position[0]**2+obj.position[1]**2)

# Get Object Velocity, calculated similar to Distance
def get_obj_velocity(obj):
    return math.sqrt(obj.velocity[0]**2+obj.velocity[1]**2)