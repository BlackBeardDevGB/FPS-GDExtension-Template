#pragma once

#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/ref.hpp>

class StepCharacter : public godot::CharacterBody3D
{  
    GDCLASS(StepCharacter, CharacterBody3D)

protected:
	static void     _bind_methods();

public:
    void _ready() override;
    void _physicsprocess(double delta);

    void MoveAndStairStep();

protected:
    void StairStepUp();
    void StairStepDown();

protected:
    float   fColliderMargin;

    godot::Vector3  vDesiredVelocity;

private:
    float   fStepHeight = 0.33f;

    bool            bForceStairStep;
    float           fTempStepHeight;

public:
    float           GetStepHeight() const                     {return fStepHeight;}
    void            SetStepHeight(float arg)                  {fStepHeight = arg;}

};