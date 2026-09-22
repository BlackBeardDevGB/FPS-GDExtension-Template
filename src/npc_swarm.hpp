#pragma once

#include "godot_cpp/classes/character_body3d.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/sprite3d.hpp"
#include "godot_cpp/variant/node_path.hpp"
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/classes/ray_cast3d.hpp>
#include <vector>

class SwarmMember : public godot::CharacterBody3D 
{
    GDCLASS(SwarmMember, godot::CharacterBody3D)

protected:
	static void _bind_methods();

public:
	SwarmMember() = default;
	~SwarmMember() override = default;

    void         _ready() override;

public:
    Node3D* ChooseAttractor(std::vector<godot::Node3D*> attractors);

public:
    godot::RayCast3D* GetLookRay() const { return gdLookRay; }
    godot::NodePath GetLookRayPath() const {return gdLookRayPath;}
    void SetLookRayPath(const godot::NodePath& arg) { gdLookRayPath = arg; }

    godot::Sprite3D* GetSprite() const { return gdSprite; }
    godot::NodePath GetSpritePath() const {return gdSpritePath;}
    void SetSpritePath(const godot::NodePath& arg) { gdSpritePath = arg; }

private:
    godot::RayCast3D* gdLookRay;
    godot::NodePath  gdLookRayPath;
    godot::Sprite3D* gdSprite;
    godot::NodePath  gdSpritePath;
};

class NPCSwarm : public godot::Node3D
{
    GDCLASS(NPCSwarm, godot::Node3D)

    protected:
	static void     _bind_methods();

public:
	NPCSwarm() = default;
	~NPCSwarm() override = default;

    void            _ready() override;
    void            _physics_process(double delta) override;

public:
    void    UpdateAttractors(); 
    void    UpdateSwarm(double delta);
    
private:
    std::vector<godot::Node3D*> vAttractors;
    std::vector<SwarmMember*> vMembers;

    int iTick = 0;
    int iUpdateAttractorsRate = 60;
    float fGravity = -9.8;
    float fGravityMultiplier = 2;
    float fSpeed = 3.f;
    float fClimbSpeed = 3.f;
    float fAvoidDistance = 0.5f;
    float fStartClimbSpeed = 2.f;

private:
    int iAttractorChooseRange = 20;

public:
    int GetAttractorChooseRange() const {return iAttractorChooseRange;}
    void SetAttractorChooseRange(const int arg) { iAttractorChooseRange = arg; }
}; 