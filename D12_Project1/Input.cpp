#include "Input.h"

#include<vector>
#include <Windows.h>

namespace
{
	std::vector<bool> previousKey(256);
	std::vector<bool> currentKey(256);

	HWND windowHandle = nullptr;

	int mouseX = 0;
	int mouseY = 0;

	LONG pendingMouseDeltaX = 0;
	LONG pendingMouseDeltaY = 0;

	LONG mouseDeltaX = 0;
	LONG mouseDeltaY = 0;
}

void Input::Initialize(HWND hwnd) {
	for (int i = 0; i < previousKey.size(); i++) previousKey[i] = false;
	for (int i = 0; i < currentKey.size(); i++) currentKey[i] = false;

	windowHandle = hwnd;

	RAWINPUTDEVICE rawInputDevice = {};

	rawInputDevice.usUsagePage = 0x01;
	rawInputDevice.usUsage = 0x02;
	rawInputDevice.dwFlags = 0;
	rawInputDevice.hwndTarget = hwnd;

	if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(RAWINPUTDEVICE))) {
		return;
	}
}

void Input::Update() {
	// キーボード入力を取得
	for (int i = 0; i < previousKey.size(); i++) {
		previousKey[i] = currentKey[i];
	}

	for (int i = 0; i < currentKey.size(); i++) {
		if (GetAsyncKeyState(i) < 0) currentKey[i] = true;
		else currentKey[i] = false;
	}

	// マウス入力を取得
	POINT point;

	if (GetCursorPos(&point)) {
		ScreenToClient(windowHandle, &point);

		mouseX = point.x;
		mouseY = point.y;
	}

	mouseDeltaX = pendingMouseDeltaX;
	mouseDeltaY = pendingMouseDeltaY;

	pendingMouseDeltaX = 0;
	pendingMouseDeltaY = 0;

}

bool Input::GetKey(short key) {
	return currentKey[key];
}

bool Input::GetKeyDown(short key)
{
	return !previousKey[key] && currentKey[key];
}

bool Input::GetKeyUp(short key)
{
	return previousKey[key] && !currentKey[key];
}

int Input::GetMouseX() {
	return mouseX;
}

int Input::GetMouseY() {
	return mouseY;
}

LONG Input::GetMouseDeltaX() {
	return mouseDeltaX;
}

LONG Input::GetMouseDeltaY() {
	return mouseDeltaY;
}

void Input::ReceiveRawInput(LPARAM lParam) {
	RAWINPUT rawInput = {};
	UINT size = sizeof(RAWINPUT);

	UINT result = GetRawInputData(
		reinterpret_cast<HRAWINPUT>(lParam),
		RID_INPUT,
		&rawInput,
		&size,
		sizeof(RAWINPUTHEADER)
	);

	if (result == static_cast<UINT>(-1)) {
		return;
	}

	if (rawInput.header.dwType != RIM_TYPEMOUSE)
	{
		return;
	}

	if ((rawInput.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == 0)
	{
		pendingMouseDeltaX += rawInput.data.mouse.lLastX;
		pendingMouseDeltaY += rawInput.data.mouse.lLastY;
	}
}