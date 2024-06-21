#include "source/GcLib/pch.h"

#include "../directx/DxObject.hpp"
#include "../directx/DxScript.hpp"
#include "../directx/DirectInput.hpp"
#include "../../TouhouDanmakufu/Common/DnhGcLibImpl.hpp"

using namespace directx;

// DxMenuObject

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
	if (flags.disable) { return; }
	timer++;

	ProcessMenuInputs();
}

void DxMenuObject::ProcessMenuInputs() {
	EDirectInput* input = EDirectInput::GetInstance();
	input_bitfield thisFrame;

	int16_t keys[8] = {EDirectInput::KEY_LEFT, EDirectInput::KEY_RIGHT, EDirectInput::KEY_UP, EDirectInput::KEY_DOWN,
	EDirectInput::KEY_SHOT, EDirectInput::KEY_BOMB, EDirectInput::KEY_USER1, EDirectInput::KEY_USER2};

	//there's probably an efficient loop I can make for this but I don't feel like it...
	//nvm yes i do
	//edit: 2 hours later, i regret every decision that led me to this.
	//nvm fuck compact efficient loops i give up C has bested me in combat
	if ( (input->GetVirtualKeyState(EDirectInput::KEY_LEFT) == KEY_PUSH) || (input->GetVirtualKeyState(EDirectInput::KEY_LEFT) == KEY_HOLD) ) {
		thisFrame.left = true;
		buttonTimer[keys[0]]++;
	}
	else {
		buttonTimer[keys[0]] = 0;
	}

	if ((input->GetVirtualKeyState(EDirectInput::KEY_RIGHT) == KEY_PUSH) || (input->GetVirtualKeyState(EDirectInput::KEY_RIGHT) == KEY_HOLD)) {
		thisFrame.right = true;
		buttonTimer[keys[1]]++;
	}
	else {
		buttonTimer[keys[1]] = 0;
	}

	if ((input->GetVirtualKeyState(EDirectInput::KEY_UP) == KEY_PUSH) || (input->GetVirtualKeyState(EDirectInput::KEY_UP) == KEY_HOLD)) {
		thisFrame.up = true;
		buttonTimer[keys[2]]++;
	}
	else {
		buttonTimer[keys[2]] = 0;
	}

	if ((input->GetVirtualKeyState(EDirectInput::KEY_DOWN) == KEY_PUSH) || (input->GetVirtualKeyState(EDirectInput::KEY_DOWN) == KEY_HOLD)) {
		thisFrame.down = true;
		buttonTimer[keys[3]]++;
	}
	else {
		buttonTimer[keys[3]] = 0;
	}

	if ((input->GetVirtualKeyState(EDirectInput::KEY_SHOT) == KEY_PUSH) || (input->GetVirtualKeyState(EDirectInput::KEY_SHOT) == KEY_HOLD)) {
		thisFrame.shot = true;
		buttonTimer[keys[4]]++;
	}
	else {
		buttonTimer[keys[4]] = 0;
	}

	if ((input->GetVirtualKeyState(EDirectInput::KEY_BOMB) == KEY_PUSH) || (input->GetVirtualKeyState(EDirectInput::KEY_BOMB) == KEY_HOLD)) {
		thisFrame.bomb = true;
		buttonTimer[keys[5]]++;
	}
	else {
		buttonTimer[keys[5]] = 0;
	}

	if ((input->GetVirtualKeyState(EDirectInput::KEY_USER1) == KEY_PUSH) || (input->GetVirtualKeyState(EDirectInput::KEY_USER1) == KEY_HOLD)) {
		thisFrame.user1 = true;
		buttonTimer[keys[6]]++;
	}
	else {
		buttonTimer[keys[6]] = 0;
	}

	if ((input->GetVirtualKeyState(EDirectInput::KEY_USER2) == KEY_PUSH) || (input->GetVirtualKeyState(EDirectInput::KEY_USER2) == KEY_HOLD)) {
		thisFrame.user2 = true;
		buttonTimer[keys[7]]++;
	}
	else {
		buttonTimer[keys[7]] = 0;
	}

	lastKey = thisFrame;
}

void DxMenuObject::OptionHandler_Keyboard() {
}

void DxMenuObject::OptionHandler() {
	flags.actionT = false;

	// keeping this the same, would love to make it const, but meh
	int16_t keys[8] = { EDirectInput::KEY_LEFT, EDirectInput::KEY_RIGHT, EDirectInput::KEY_UP, EDirectInput::KEY_DOWN,
	EDirectInput::KEY_SHOT, EDirectInput::KEY_BOMB, EDirectInput::KEY_USER1, EDirectInput::KEY_USER2 };

	if (optionType[optionIndex] == TYPE_KEYBOARD) {
		OptionHandler_Keyboard();
		return;
	}

	if (optionType[optionIndex] == TYPE_XAXIS) {
		if (lastKey.left) {
			if (((buttonTimer[keys[0]] > buttonDelay) && (buttonTimer[keys[0]] % scrollInterval == 0)) || (buttonTimer[keys[0]] == 1)) {
				optionIndexX[optionIndex] = (optionIndexX[optionIndex] - 1 + maxIndexX[optionIndex]) % maxIndexX[optionIndex];
			}
		}
		if (lastKey.right) {
			if (((buttonTimer[keys[1]] > buttonDelay) && (buttonTimer[keys[1]] % scrollInterval == 0)) || (buttonTimer[keys[1]] == 1)) {
				optionIndexX[optionIndex] = (optionIndexX[optionIndex] + 1) % maxIndexX[optionIndex];
			}
		}
	}
	else if (optionType[optionIndex] == TYPE_SLIDER) {
		float sVal = sliderValue[optionIndex];
		int sMax = sliderMax[optionIndex];
		int sMin = sliderMin[optionIndex];
		float sIncr = sliderIncr[optionIndex];

		if (lastKey.left) {
			if (((buttonTimer[keys[0]] > buttonDelay) && (buttonTimer[keys[0]] % scrollInterval_slider == 0)) || (buttonTimer[keys[0]] == 1)) {
				sliderValue[optionIndex] = std::max(sVal - sIncr, (float)sMin);
			}
		}
		if (lastKey.right) {
			if (((buttonTimer[keys[1]] > buttonDelay) && (buttonTimer[keys[1]] % scrollInterval_slider == 0)) || (buttonTimer[keys[1]] == 1)) {
				sliderValue[optionIndex] = std::min(sVal + sIncr, (float)sMax);
			}
		}
	}

	// up/down/shot logic here
}

// DxMenuObjectManger

DxMenuObjectManager::DxMenuObjectManager() {
}

void DxMenuObjectManager::ForceCloseAllMenus() {
}
