#include "Time.h"
#include <chrono>

namespace
{
	using Clock = std::chrono::steady_clock;
	
	// 1F前の時刻とフレーム間の経過時間
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
	// 現在の時刻と1F前の時刻からフレーム間の経過時間を計算
	auto currentTime = Clock::now();

	std::chrono::duration<float> elapsed = currentTime - previousTime;

	deltaTime = elapsed.count();

	previousTime = currentTime;
}

float Time::DeltaTime()
{
	return deltaTime;
}