#pragma once

#include <Windows.h>

// キーボードやマウスからの入力を管理する
namespace Input
{
	bool Initialize(HWND hwnd);
	void Update();

	bool GetKey(SHORT key);
	bool GetKeyDown(SHORT key);
	bool GetKeyUp(SHORT key);

	int GetMouseX();
	int GetMouseY();

	LONG GetMouseDeltaX();
	LONG GetMouseDeltaY();

	void ReceiveRawInput(LPARAM lParam);
}