#include "Input.h"

#include<vector>
#include <Windows.h>

namespace
{
	std::vector<bool> previousKey(256);
	std::vector<bool> currentKey(256);
}

void Input::Initialize() {
	for (int i = 0; i < previousKey.size(); i++) previousKey[i] = false;
	for (int i = 0; i < currentKey.size(); i++) currentKey[i] = false;
}

void Input::Update() {
	for (int i = 0; i < previousKey.size(); i++) {
		previousKey[i] = currentKey[i];
	}

	for (int i = 0; i < currentKey.size(); i++) {
		if (GetAsyncKeyState(i) < 0) currentKey[i] = true;
		else currentKey[i] = false;
	}
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