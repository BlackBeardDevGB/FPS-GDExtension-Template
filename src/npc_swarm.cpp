#include "npc_swarm.hpp"
#include "godot_cpp/classes/ray_cast3d.hpp"
#include "godot_cpp/variant/color.hpp"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;


// SWARM MEMBER

void SwarmMember::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("GetLookRayPath"), &SwarmMember::GetLookRayPath);
    ClassDB::bind_method(D_METHOD("SetLookRayPath", "arg"), &SwarmMember::SetLookRayPath);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "gdLookRayPath"), "SetLookRayPath", "GetLookRayPath");

    ClassDB::bind_method(D_METHOD("GetSpritePath"), &SwarmMember::GetSpritePath);
    ClassDB::bind_method(D_METHOD("SetSpritePath", "arg"), &SwarmMember::SetSpritePath);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "gdSpritePath"), "SetSpritePath", "GetSpritePath");
}

void SwarmMember::_ready()
{
    gdLookRay = get_node<RayCast3D>(gdLookRayPath);

    if(gdLookRay == nullptr)
        UtilityFunctions::push_error("Swarm member has no Look-ray, name: ", get_name());
}

Node3D* SwarmMember::ChooseAttractor(std::vector<godot::Node3D*> attractors)
{
    Node3D* attractor = attractors[0];

    for(Node3D* node : attractors)
    {
        if(!node)
            continue;

        if((node->get_global_position() - get_global_position()).length() < (attractor->get_global_position() - get_global_position()).length())
        {
            attractor = node;
        }
    }

    return attractor;
}

// SWARM

void NPCSwarm::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("GetAttractorChooseRange"), &NPCSwarm::GetAttractorChooseRange);
    ClassDB::bind_method(D_METHOD("SetAttractorChooseRange", "arg"), &NPCSwarm::SetAttractorChooseRange);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "iAttractorChooseRange", PROPERTY_HINT_RANGE, "1,200,1"), "SetAttractorChooseRange", "GetAttractorChooseRange");
}

void NPCSwarm::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
        return;

    Array children = get_children();
    vMembers.clear();
    vMembers.reserve(children.size());

    for(int i = 0; i < children.size(); i++)
    {
        SwarmMember* node = Object::cast_to<SwarmMember>(children[i]);
        if(!node)
            continue;
        if(!node->is_in_group("member"))
            continue;

        vMembers.push_back(node);
    }
}

void NPCSwarm::_physics_process(double delta)
{
    if(Engine::get_singleton()->is_editor_hint())
        return;

    UpdateAttractors();
    UpdateSwarm(delta);

    iTick++;
}

void NPCSwarm::UpdateAttractors()
{
    if(iTick % iUpdateAttractorsRate != 0)
        return;

    vAttractors.clear();

    Array children = get_parent()->get_children(true);
    vAttractors.reserve(children.size());
    
    for(int i = 0; i < children.size(); i++)
    {
        Node3D* node = Object::cast_to<Node3D>(children[i]);
        if(!node)
            continue;
        if(!node->is_in_group("attractor"))
            continue;

        vAttractors.push_back(node);
    }
}

void NPCSwarm::UpdateSwarm(double delta)
{
    for(SwarmMember* member : vMembers)
    {
        Node3D* attractor = member->ChooseAttractor(vAttractors);

        if(!attractor)
            continue;

        Vector3 newVelocity = member->get_velocity();
        bool climbing = false, grounded = true;

        member->look_at(attractor->get_global_position());

        RayCast3D* lookRay = member->GetLookRay();
        if(lookRay && !climbing)
        {
            lookRay->set_global_position(member->get_global_position());
            if(!climbing)
            {
                lookRay->force_raycast_update();
                if(lookRay->is_colliding() && member->get_velocity().length() < fStartClimbSpeed)
                    climbing = true;
            }
        }

        if(!member->is_on_floor() && !climbing)
        {
            grounded = false;
            newVelocity.y += fGravity * fGravityMultiplier * delta; 
        }

        if(climbing)
        {
            newVelocity.y += fClimbSpeed * delta;

            lookRay->force_raycast_update();
            if(!lookRay->is_colliding())
                climbing = false;
        }

        for(SwarmMember* avoidMember : vMembers)
        {
            if(avoidMember == member)
                continue;

            if(member->get_global_position().distance_to(avoidMember->get_global_position()) < fAvoidDistance)
            {
                Vector3 avoidVelocity = avoidMember->get_global_position().direction_to(member->get_global_position()) * fSpeed * delta;
                avoidVelocity.y = 0;

                newVelocity += avoidVelocity;
            }
        }

        newVelocity += -member->get_transform().get_basis().get_column(2) * fSpeed * delta;
        member->set_velocity(newVelocity);        

        member->move_and_slide();
    }
}