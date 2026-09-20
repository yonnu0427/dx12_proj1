#include "Time.h"
#include <chrono>

namespace
{
	using Clock = std::chrono::steady_clock;

	Clock::time_point previousTime;
	float deltaTime = 0.0f;
}

void Time::Initialize()
{
	previousTime = Clock::now();
	deltaTime = 0.0f;
}

void Time::Update()
{
	auto currentTime = Clock::now();

	std::chrono::duration<float> elapsed = currentTime - previousTime;

	deltaTime = elapsed.count();

	previousTime = currentTime;
}

float Time::DeltaTime()
{
	return deltaTime;
}