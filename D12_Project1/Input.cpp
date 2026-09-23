#include "Input.h"

#include<vector>
#include <Windows.h>

namespace
{
	// キーが押されているかを格納する配列
	std::vector<bool> previousKey(256);
	std::vector<bool> currentKey(256);

	HWND windowHandle = nullptr;

	// マウスの座標
	int mouseX = 0;
	int mouseY = 0;

	// 1F以内のマウスの移動距離。これを数回加算して1F間のマウスの移動距離を算出する。
	LONG pendingMouseDeltaX = 0;
	LONG pendingMouseDeltaY = 0;

	// 1Fでのマウスの移動距離
	LONG mouseDeltaX = 0;
	LONG mouseDeltaY = 0;
}

bool Input::Initialize(HWND hwnd) {
	for (int i = 0; i < previousKey.size(); i++) previousKey[i] = false;
	for (int i = 0; i < currentKey.size(); i++) currentKey[i] = false;

	// RawInputの受け取りの設定
	windowHandle = hwnd;
	if (hwnd == nullptr) {
		return false;
	}

	RAWINPUTDEVICE rawInputDevice = {};

	rawInputDevice.usUsagePage = 0x01;
	rawInputDevice.usUsage = 0x02;
	rawInputDevice.dwFlags = 0;
	rawInputDevice.hwndTarget = hwnd;

	if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(RAWINPUTDEVICE))) {
		return false;
	}

	return true;
}

void Input::Update() {
	// 直前のキー入力を保持
	for (int i = 0; i < previousKey.size(); i++) {
		previousKey[i] = currentKey[i];
	}

	// キーボード入力を取得
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

	// 最終的なマウスの移動距離を入れてpendingは初期化
	mouseDeltaX = pendingMouseDeltaX;
	mouseDeltaY = pendingMouseDeltaY;

	pendingMouseDeltaX = 0;
	pendingMouseDeltaY = 0;

}

bool Input::GetKey(SHORT key) {
	return currentKey[key];
}

bool Input::GetKeyDown(SHORT key)
{
	return !previousKey[key] && currentKey[key];
}

bool Input::GetKeyUp(SHORT key)
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

// RawInputからマウスによる入力データを取り出す
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