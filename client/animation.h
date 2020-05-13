#pragma once

#ifndef ANIMATION_H
#define ANIMATION_H

#include <shared/game/offsets.h>

struct Animation
{
	char pad0[0x10];
	uint16_t frame_start,
			 frame_end;
};

struct AnimationManager
{
	Animation* get_anim(uint16_t i)
	{
		return (Animation*)((uintptr_t)this + (i * offsets::PER_ANIM_SIZE));
	}

	static AnimationManager* get()
	{
		return *(AnimationManager**)offsets::ANIM_TABLE;
	}
};


#endif