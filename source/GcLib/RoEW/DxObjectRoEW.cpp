#include "source/GcLib/pch.h"

#include "../directx/DxObject.hpp"
#include "../directx/DxScript.hpp"
#include "../directx/DirectInput.hpp"

using namespace directx;

DxMenuObject::DxMenuObject() {
	timer = 0;
	// set up buttonTimer correctly
	buttonTimer.insert({EDirectInput::KEY_LEFT, 0});
	buttonTimer.insert({EDirectInput::KEY_RIGHT, 0});
	buttonTimer.insert({EDirectInput::KEY_UP, 0});
	buttonTimer.insert({EDirectInput::KEY_DOWN, 0});
	buttonTimer.insert({EDirectInput::KEY_SHOT, 0});
	buttonTimer.insert({EDirectInput::KEY_BOMB, 0});
	buttonTimer.insert({EDirectInput::KEY_USER1, 0});
	buttonTimer.insert({EDirectInput::KEY_USER2, 0});
}

void DxMenuObject::Activate() {
	//Only activate your menu object *after* you set up all the important parts!
}

void DxMenuObject::Work() {
	// Do stuff
	
}

DxMenuObjectManager::DxMenuObjectManager() {
}

void DxMenuObjectManager::ForceCloseAllMenus() {
}
