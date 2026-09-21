#pragma once

#include <Windows.h>

namespace Input
{
	void Initialize(HWND hwnd);
	void Update();

	bool GetKey(short key);
	bool GetKeyDown(short key);
	bool GetKeyUp(short key);

	int GetMouseX();
	int GetMouseY();

	LONG GetMouseDeltaX();
	LONG GetMouseDeltaY();

	void ReceiveRawInput(LPARAM lParam);
}