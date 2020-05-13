#pragma once

#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdint.h>

#include <shared/game/math.h>

#include <offsets.h>

enum class eParticleType : uint16_t
{
	WATER_SPLASH = 220,
	WHITE_ECLIPSE = 224,
	EXPLOSION = 229,
	BOAT_WATER_RAIL = 230,
	BLOOD = 233,
	FLARE_EFFECT = 233,
	RICOCHET = 238,
	MUZZLE_FLASH = 240,
	M16_MUZZLE_FLASH = 241,
	DISK_HIT = 245,
	FIREBALL = 246,
	DISK_HIT_2 = 247,
	FIRE = 252,
};

struct Particle
{
	ivec3 position;
};

struct ParticleSystem
{
	Particle* get_particle(uint32_t i)
	{
		return (Particle*)((uintptr_t)this + (i * offsets::PER_PARTICLE_SIZE));
	}

	Particle* spawn_blood(const ivec3& pos, uint16_t room, uint16_t velocity, uint16_t rotation)
	{
		auto index = reinterpret_cast<int16_t(__cdecl*)(int, int, int, int16_t, int16_t, int16_t)>(offsets::SPAWN_BLOOD_PARTICLE)(pos.x,
																													 pos.y,
																													 pos.z,
																													 velocity,
																													 rotation,
																													 room);
		if (index != -1)
			return get_particle(index);
		return nullptr;
	}

	static ParticleSystem* get()
	{
		return *(ParticleSystem**)offsets::PARTICLES_TABLE;
	}
};

#endif