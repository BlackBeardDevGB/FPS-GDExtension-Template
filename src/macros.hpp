#pragma once

#include <godot_cpp/variant/vector3.hpp>


namespace HandyMacros
{
    static const godot::Vector3 RIGHT   = godot::Vector3(1, 0, 0);
    static const godot::Vector3 LEFT    = godot::Vector3(-1, 0, 0);
    static const godot::Vector3 UP      = godot::Vector3(0, 1, 0);
    static const godot::Vector3 DOWN    = godot::Vector3(0, -1, 0);
    static const godot::Vector3 FORWARD = godot::Vector3(0, 0, -1);
    static const godot::Vector3 BACK    = godot::Vector3(0, 0, 1);

    static const godot::Vector3 HRZNTL  = godot::Vector3(1,0,1); 
    static const godot::Vector3 ZERO    = godot::Vector3(0,0,0); 
}