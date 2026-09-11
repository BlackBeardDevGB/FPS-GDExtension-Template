#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/shape3d.hpp>

#include "player.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "macros.hpp"
#include "step_character.hpp"

using namespace HandyMacros;
using namespace godot;

void Player::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("SetAnimPlayerPath", "arg"), &Player::SetAnimPlayerPath);
    ClassDB::bind_method(D_METHOD("GetAnimPlayerPath"), &Player::GetAnimPlayerPath);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "animPlayerPath", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "AnimationPlayer"), "SetAnimPlayerPath", "GetAnimPlayerPath");
    ClassDB::bind_method(D_METHOD("SetHeadPath", "arg"), &Player::SetHeadPath);
    ClassDB::bind_method(D_METHOD("GetHeadPath"), &Player::GetHeadPath);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "playerHeadPath", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Node"), "SetHeadPath", "GetHeadPath");

    ClassDB::bind_method(D_METHOD("SetWalkSpeed", "arg"), &Player::SetWalkSpeed);
    ClassDB::bind_method(D_METHOD("GetWalkSpeed"), &Player::GetWalkSpeed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fWalkSpeed"), "SetWalkSpeed", "GetWalkSpeed");
    ClassDB::bind_method(D_METHOD("SetCrouchWalkSpeed", "arg"), &Player::SetCrouchWalkSpeed);
    ClassDB::bind_method(D_METHOD("GetCrouchWalkSpeed"), &Player::GetCrouchWalkSpeed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fCrouchWalkSpeed"), "SetCrouchWalkSpeed", "GetCrouchWalkSpeed");
    ClassDB::bind_method(D_METHOD("SetJumpVelocity", "arg"), &Player::SetJumpVelocity);
    ClassDB::bind_method(D_METHOD("GetJumpVelocity"), &Player::GetJumpVelocity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fJumpVelocity"), "SetJumpVelocity", "GetJumpVelocity");

    ClassDB::bind_method(D_METHOD("SetMouseSensitivity", "arg"), &Player::SetMouseSensitivity);
    ClassDB::bind_method(D_METHOD("GetMouseSensitivity"), &Player::GetMouseSensitivity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fMouseSensitivity"), "SetMouseSensitivity", "GetMouseSensitivity");

    ClassDB::bind_method(D_METHOD("SetInputAcceleration", "arg"), &Player::SetInputAcceleration);
    ClassDB::bind_method(D_METHOD("GetInputAcceleration"), &Player::GetInputAcceleration);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fInputAcceleration"), "SetInputAcceleration", "GetInputAcceleration");
    ClassDB::bind_method(D_METHOD("SetInputDecceleration", "arg"), &Player::SetInputDecceleration);
    ClassDB::bind_method(D_METHOD("GetInputDecceleration"), &Player::GetInputDecceleration);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fInputDecceleration"), "SetInputDecceleration", "GetInputDecceleration");
    ClassDB::bind_method(D_METHOD("SetAcceleration", "arg"), &Player::SetAcceleration);
    ClassDB::bind_method(D_METHOD("GetAcceleration"), &Player::GetAcceleration);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fAcceleration"), "SetAcceleration", "GetAcceleration");
    ClassDB::bind_method(D_METHOD("SetDecceleration", "arg"), &Player::SetDecceleration);
    ClassDB::bind_method(D_METHOD("GetDecceleration"), &Player::GetDecceleration);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fDecceleration"), "SetDecceleration", "GetDecceleration");

    ClassDB::bind_method(D_METHOD("SetFootStepSoundInterval", "arg"), &Player::SetFootStepSoundInterval);
    ClassDB::bind_method(D_METHOD("GetFootStepSoundInterval"), &Player::GetFootStepSoundInterval);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fFootStepSoundInterval"), "SetFootStepSoundInterval", "GetFootStepSoundInterval");

} 

void Player::_ready()
{
    if (Engine::get_singleton()->is_editor_hint())
        return;

    StepCharacter::_ready();

    input = Input::get_singleton();

    head = get_node<godot::Node3D>(headPath);
    animPlayer = get_node<godot::AnimationPlayer>(animPlayerPath);

    if (!head)
        UtilityFunctions::push_error("Player: head is not assigned!");
    if(!animPlayer)
        UtilityFunctions::push_error("Player: amimPlayer is not assigned!");
    if(!input)
        UtilityFunctions::push_error("Player: input is not assigned!");

    if(input)
    {
        input->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
        bMouseCaptured = true;
    }

}

void Player::_physics_process(double delta)
{
    if (Engine::get_singleton()->is_editor_hint())
        return;

    StepCharacter::_physicsprocess(delta);
    
    UpdateInput(delta);
    UpdateMovement(delta);
    Rotate(delta);
}

void Player::UpdateInput(double delta)
{
    if (input->is_action_just_pressed("crouch"))
    {
        Crouch(true);
    }
    if (input->is_action_just_released("crouch") && is_on_floor() || !input->is_action_pressed("crouch") && is_on_floor())
    {
        if(bIsCrouching)
            Crouch(false);
    }

    if(input->is_action_just_pressed("ui_cancel"))
    {
        if(bMouseCaptured)
            input->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
        else
            input->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
         
        bMouseCaptured = !bMouseCaptured;
    }

#ifdef WEB_ENABLED
    if(input->is_action_just_pressed("fire"))
        input->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
#endif
}

void Player::_unhandled_input(const Ref<InputEvent> &event)
{
    if (Engine::get_singleton()->is_editor_hint())
        return;

    Ref<InputEventMouseMotion> mouseMotion = event;

    if(mouseMotion.is_valid() && Input::get_singleton()->get_mouse_mode() == Input::MOUSE_MODE_CAPTURED)
    {
        SetRotationTarget(mouseMotion->get_relative());
    }
}

void Player::SetRotationTarget(Vector2 relativeMotion)
{
    fRotationTargetPlayer += -relativeMotion.x * fMouseSensitivity;
    fRotationTargetHead += -relativeMotion.y * fMouseSensitivity;
    fRotationTargetHead = Math::clamp(fRotationTargetHead, -RAD90, RAD90);
}

void Player::Rotate(double delta)
{
    if(!head)
        return;

    float clampedHeadRotation = Math::clamp(fRotationTargetHead, -RAD90, RAD90);
    Quaternion headPitch = Quaternion(RIGHT, clampedHeadRotation);

    head->set_quaternion(Quaternion(Math::clamp(headPitch.x, -RAD90, RAD90), headPitch.y, headPitch.z, headPitch.w));

    set_quaternion(Quaternion(UP, fRotationTargetPlayer));
}

void Player::Crouch(bool crouch)
{
    if (crouch)
    {
        if(!is_on_floor())
        {
            animPlayer->play("cam-anims/midair-crouch");
            bIsMidairCrouching = true;
        }
        else
            animPlayer->play("cam-anims/crouch");

        bIsCrouching = true;
    }
    else
    {
        if(bIsMidairCrouching)
        {
            animPlayer->play_backwards("cam-anims/midair-crouch");
            bIsMidairCrouching = false;
        }
        else
            animPlayer->play_backwards("cam-anims/crouch");

        bIsCrouching = false;
    }
}

void Player::UpdateMovement(double delta)
{
    if (Engine::get_singleton()->is_editor_hint())
        return;

    if(!input || !animPlayer)
        return;

    Vector3 newVelocity = get_velocity();

    if(!is_on_floor())
    {
        newVelocity += get_gravity() * delta * 3.f; 
    }
    
    if (input->is_action_just_pressed(("ui_accept")) && is_on_floor())
    {
        newVelocity.y = fJumpVelocity;
    }

        Vector2 inputDir = input->get_vector("left", "right", "up", "down");
        Vector3 direction = (get_transform().basis.xform(Vector3(inputDir.x, 0, inputDir.y))).normalized();

        vDesiredVelocity = direction * 1.5f;

        if (direction.length_squared() > 0.1f)
        {
            float baseSpeed = fWalkSpeed;
            if(bIsCrouching && is_on_floor())
                baseSpeed = fCrouchWalkSpeed;

            fCurrentSpeed = Math::move_toward(fCurrentSpeed, baseSpeed, (float)delta * fAcceleration);
            vCurrentDirection = vCurrentDirection.move_toward(direction, (float)delta * fInputAcceleration);
        }
        else
        {
            fCurrentSpeed = Math::move_toward(fCurrentSpeed, 0.0f, (float)delta * fDecceleration);
            vCurrentDirection = vCurrentDirection.move_toward(direction, (float)delta * fInputDecceleration);
        }

        iFootStepCounter += (float)delta;
        if (iFootStepCounter >= fFootStepSoundInterval)
            iFootStepCounter = 0;

        newVelocity.x = vCurrentDirection.x * fCurrentSpeed;
        newVelocity.z = vCurrentDirection.z * fCurrentSpeed;

        set_velocity(newVelocity);

        MoveAndStairStep();
}
