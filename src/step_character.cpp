#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/shape3d.hpp>
#include <godot_cpp/classes/physics_server3d.hpp>
#include <godot_cpp/classes/physics_test_motion_result3d.hpp>
#include <godot_cpp/classes/physics_test_motion_parameters3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/variant/transform3d.hpp"
#include "macros.hpp"
#include "step_character.hpp"

using namespace HandyMacros;
using namespace godot;

void StepCharacter::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("SetStepHeight", "arg"), &StepCharacter::SetStepHeight);
    ClassDB::bind_method(D_METHOD("GetStepHeight"), &StepCharacter::GetStepHeight);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fStepHeight"), "SetStepHeight", "GetStepHeight");
}

void StepCharacter::_ready()
{
    fColliderMargin = get_node<CollisionShape3D>("Collider")->get_shape()->get_margin();
    if(fColliderMargin > 0.01f)
        UtilityFunctions::push_warning("Margin on player's collider shape is over 0.01, may snag on stair steps");
}


void StepCharacter::_physicsprocess(double delta)
{
    vDesiredVelocity = ZERO;
}

void StepCharacter::MoveAndStairStep()
{
    StairStepUp();
    move_and_slide();
    StairStepDown();
}

void StepCharacter::StairStepUp()
{
    if (!bGrounded && bForceStairStep == false) 
        return;

    Vector3 currentVelocity = get_velocity();

    Vector3 horizontalVelocity = currentVelocity * HORIZONTAL;
    Vector3 testingVelocity = horizontalVelocity;

    if (horizontalVelocity == ZERO)
        testingVelocity = vDesiredVelocity;

    if (testingVelocity == ZERO) return;

    Ref<PhysicsTestMotionResult3D> result;
    result.instantiate();
    Ref<PhysicsTestMotionParameters3D> params;
    params.instantiate();

    Transform3D transform = get_global_transform();

    Vector3 distance = testingVelocity * (float)get_physics_process_delta_time();
    params->set_from(transform);
    params->set_motion(distance);
    params->set_margin(fColliderMargin);

    if (!PhysicsServer3D::get_singleton()->body_test_motion(get_rid(), params, result))
        return;

    Vector3 remainder = result->get_remainder();
    transform = transform.translated(result->get_travel());

    Vector3 stepUp = fStepHeight * UP;
    params->set_from(transform);
    params->set_motion(stepUp);
    PhysicsServer3D::get_singleton()->body_test_motion(get_rid(), params, result);
    transform = transform.translated(result->get_travel());
    float stepUpDistance = result->get_travel().length();

    params->set_from(transform);
    params->set_motion(remainder);
    PhysicsServer3D::get_singleton()->body_test_motion(get_rid(), params, result);
    transform = transform.translated(result->get_travel());

    params->set_from(transform);
    params->set_motion(DOWN * stepUpDistance);

    if (!PhysicsServer3D::get_singleton()->body_test_motion(get_rid(), params, result))
        return;

    transform = transform.translated(result->get_travel());

    Vector3 surfaceNormal = result->get_collision_normal();
    if (surfaceNormal.angle_to(UP) > get_floor_max_angle()) 
        return;

    Vector3 gp = get_global_position();
    gp.y = transform.origin.y;
    set_global_position(gp);
}

void StepCharacter::StairStepDown()
{
    Vector3 velocity = get_velocity();

	if (bWasGrounded == false || velocity.y >= 0) return;

    Ref<PhysicsTestMotionResult3D> result;
    result.instantiate();
    Ref<PhysicsTestMotionParameters3D> params;
    params.instantiate();

    params->set_from(get_global_transform());
    params->set_motion(DOWN * fStepHeight);
    params->set_margin(fColliderMargin);


    if(!PhysicsServer3D::get_singleton()->body_test_motion(get_rid(), params, result)) 
        return;

    set_global_transform(get_global_transform().translated(result->get_travel()));
    apply_floor_snap();
}