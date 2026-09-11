#pragma once

#include <godot_cpp/classes/collision_object3d.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/animation_player.hpp>

#include "step_character.hpp"

#define RAD90 1.571f

class Player : public StepCharacter
{
    GDCLASS(Player, StepCharacter)

protected:
	static void     _bind_methods();

public:
	Player() = default;
	~Player() override = default;

    void            _ready() override;
    void            _physics_process(double delta) override;
    void            _unhandled_input(const godot::Ref<godot::InputEvent> &event) override;

private:
    void            UpdateInput(double delta);
    void            UpdateMovement(double delta);
    void            SetRotationTarget(godot::Vector2 mouseMotion);
    void            Rotate(double delta);
    void            Crouch(bool crouch);

    godot::AnimationPlayer* animPlayer = nullptr;
    godot::NodePath         animPlayerPath;
    godot::Node3D*          head = nullptr;
    godot::NodePath         headPath;

    float                   fWalkSpeed;
    float                   fCrouchWalkSpeed;
    float                   fJumpVelocity;
    float                   fMouseSensitivity;

    float                   fInputAcceleration;
    float                   fInputDecceleration;
    float                   fAcceleration;
    float                   fDecceleration;

    float                   fFootStepSoundInterval;


public:
    godot::NodePath GetAnimPlayerPath() const                       {return animPlayerPath;}
    void            SetAnimPlayerPath(const godot::NodePath &arg)   {animPlayerPath = arg;}
    godot::NodePath GetHeadPath() const                             {return headPath;}
    void            SetHeadPath(const godot::NodePath &arg)         {headPath = arg;}
    float           GetWalkSpeed() const                            {return fWalkSpeed;}
    void            SetWalkSpeed(float arg)                         {fWalkSpeed = arg;}
    float           GetCrouchWalkSpeed()                            {return fCrouchWalkSpeed;}
    void            SetCrouchWalkSpeed(float arg)                   {fCrouchWalkSpeed = arg;}
    float           GetJumpVelocity() const                         {return fJumpVelocity;}
    void            SetJumpVelocity(float arg)                      {fJumpVelocity = arg;}
    float           GetMouseSensitivity() const                     {return fMouseSensitivity;}
    void            SetMouseSensitivity(float arg)                  {fMouseSensitivity = arg;}

    float           GetInputAcceleration() const                    {return fInputAcceleration;}
    void            SetInputAcceleration(float arg)                 {fInputAcceleration = arg;}
    float           GetInputDecceleration() const                   {return fInputDecceleration;}
    void            SetInputDecceleration(float arg)                {fInputDecceleration = arg;}
    float           GetAcceleration() const                         {return fAcceleration;}
    void            SetAcceleration(float arg)                      {fAcceleration = arg;}
    float           GetDecceleration() const                        {return fDecceleration;}
    void            SetDecceleration(float arg)                     {fDecceleration = arg;}

    int             GetFootStepSoundInterval() const                {return fFootStepSoundInterval;}
    void            SetFootStepSoundInterval(int arg)               {fFootStepSoundInterval = arg;}

private:
    godot::Input*   input = nullptr;

    float           fRotationTargetPlayer;
    float           fRotationTargetHead;
    int             iFootStepCounter;
    float           fCurrentSpeed;
    godot::Vector3  vCurrentDirection;
    bool            bMouseCaptured;
    bool            bIsCrouching;
    bool            bIsMidairCrouching;

};