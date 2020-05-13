#pragma once

#ifndef MATH_H
#define MATH_H

#include <stdint.h>
#include <cmath>

struct ivec3
{
	int32_t x = 0,
			y = 0,
			z = 0;

	ivec3() {}
	ivec3(int32_t _x, int32_t _y, int32_t _z) : x(_x), y(_y), z(_z) {}

	ivec3 operator + (const ivec3& v)		{ return ivec3(x + v.x, y + v.y, z + v.z); }

	ivec3 operator - (const ivec3& v)		{ return ivec3(x - v.x, y - v.y, z - v.z); }

	int32_t distance(const ivec3& v)		{ return static_cast<int32_t>(std::sqrtf(static_cast<float>((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z)))); }

	ivec3 interpolate_to(const ivec3& v, float factor)
	{
		x = static_cast<int32_t>(std::lerp(static_cast<float>(x), static_cast<float>(v.x), factor));
		y = static_cast<int32_t>(std::lerp(static_cast<float>(y), static_cast<float>(v.y), factor));
		z = static_cast<int32_t>(std::lerp(static_cast<float>(z), static_cast<float>(v.z), factor));
		
		return *this;
	}
};

struct small_ivec3
{
	uint16_t x = 0, y = 0, z = 0;

	small_ivec3() {}
	small_ivec3(uint16_t _x, uint16_t _y, uint16_t _z) : x(_x), y(_y), z(_z) {}

	small_ivec3 operator + (const small_ivec3& v)
	{
		return small_ivec3(x + v.x, y + v.y, z + v.z);
	}

	small_ivec3 operator - (const small_ivec3& v)
	{
		return small_ivec3(x - v.x, y - v.y, z - v.z);
	}

	small_ivec3 interpolate_to(const small_ivec3& v, float factor)
	{
		x = static_cast<uint32_t>(std::lerp(static_cast<float>(x), static_cast<float>(v.x), factor));
		y = static_cast<uint16_t>(std::lerp(static_cast<float>(y), static_cast<float>(v.y), factor));
		z = static_cast<uint32_t>(std::lerp(static_cast<float>(z), static_cast<float>(v.z), factor));

		return *this;
	}
};

#endif