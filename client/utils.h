#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <string>
#include <random>
#include <functional>

#define SIZE_OF_ARRAY(x) sizeof(x) / sizeof(*x)

namespace utils
{
	template <typename T>
	class Timer
	{
	private:

		std::chrono::high_resolution_clock::time_point last_time,
													   current_time;
		uint32_t interval = 0;

	public:

		Timer()				{ reset(); }
		Timer(uint32_t val) { reset(val); }

		void reset(uint32_t val = 0)
		{
			last_time = current_time = std::chrono::high_resolution_clock::now();
			interval = val;
		}

		bool ready()
		{
			if (interval <= 0)
				return false;

			if (std::chrono::duration_cast<T>((current_time = std::chrono::high_resolution_clock::now()) - last_time).count() >= interval)
			{
				last_time = current_time;
				return true;
			}

			return false;
		}
	};

	class MemProt
	{
	private:

		size_t size = 0;

		uintptr_t addr = 0;

		unsigned long old_prot = 0;

		bool ok = false,
			 permanent = false;

	public:

		MemProt(uintptr_t addr, size_t size, uint32_t prot, bool permanent = false);
		~MemProt();

		static void execute_under_prot(uintptr_t addr, size_t size, uint32_t prot, const std::function<void()>& fn);
	};

	std::string rand_str(int32_t len);
	std::wstring rand_wstr(int32_t len);
};

#endif