#pragma once

#include "../EDGOCA/ComponentSystem.h"
using namespace EDGOCA;

#include "AttributeReaders.h"

namespace Behaviors
{
	extern const BehaviorNameKey BHVR_CONTROLLER;
	extern const BehaviorNameKey BHVR_RENDERABLE;
	extern const BehaviorNameKey BHVR_COLLIDER;
	extern const BehaviorNameKey BHVR_PHYSICS;
	extern const BehaviorNameKey BHVR_GROUNDCLAMP;
	extern const BehaviorNameKey BHVR_CHARACTER_CONTROLLER;
	extern const BehaviorNameKey BHVR_TRANSLATOR_CONTROLLER;
	
	extern const BehaviorNameKey BHVR_KEYBOARD;
	extern const BehaviorNameKey BHVR_JOYSTICK;

	extern const BehaviorNameKey BHVR_HARD_ATTACH;
	extern const BehaviorNameKey BHVR_LOOK_AT;
	extern const BehaviorNameKey BHVR_TURN_TO;
	extern const BehaviorNameKey BHVR_MOUSE_LOOK;
	extern const BehaviorNameKey BHVR_CAMERA;

	extern const BehaviorNameKey BHVR_DIRECTIONAL_LIGHT;
	extern const BehaviorNameKey BHVR_LIGHT;

	extern const BehaviorNameKey BHVR_SKY_BOX;
	extern const BehaviorNameKey BHVR_SPIN;

	extern const BehaviorNameKey BHVR_POINT_LIGHT;
	extern const BehaviorNameKey BHVR_SPOT_LIGHT;

	extern const BehaviorNameKey BVHR_BASE_RENDERABLE;

	extern const BehaviorNameKey BHVR_TURRET_CONTROLLER;
	extern const BehaviorNameKey BHVR_WHEEL;

	extern const BehaviorNameKey BHVR_PROJECTILE;
}