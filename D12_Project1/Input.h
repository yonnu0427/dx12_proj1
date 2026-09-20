#pragma once

namespace Input
{
	void Initialize();
	void Update();

	bool GetKey(short key);
	bool GetKeyDown(short key);
	bool GetKeyUp(short key);
}