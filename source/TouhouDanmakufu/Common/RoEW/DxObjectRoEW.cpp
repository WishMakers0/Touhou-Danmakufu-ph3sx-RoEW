#include "source/GcLib/pch.h"

#include "../../../GcLib/directx/DxObject.hpp"
#include "../../../GcLib/directx/DirectInput.hpp"
#include "../StgControlScript.hpp"
#include "../../../GcLib/gstd/Logger.hpp"

//using namespace directx;

// DxMenuObject

DxMenuObject::DxMenuObject() {
	timer = 0;
	typeObject_ = TypeObject::Menu;
	// set up buttonTimer correctly
	buttonTimer.insert({ KEY_LEFT, 0 });
	buttonTimer.insert({ KEY_RIGHT, 0 });
	buttonTimer.insert({ KEY_UP, 0 });
	buttonTimer.insert({ KEY_DOWN, 0 });
	buttonTimer.insert({ KEY_SHOT, 0 });
	buttonTimer.insert({ KEY_BOMB, 0 });
	buttonTimer.insert({ KEY_USER1, 0 });
	buttonTimer.insert({ KEY_USER2, 0 });
	input = dynamic_cast<VirtualKeyManager*>(DirectInput::GetBase());
	parent = ID_INVALID;
}

DxMenuObject::~DxMenuObject() {
	for (auto& id : relatedIDs) {
		DxScriptObjectBase* obj = GetObjectFromID(id);
		if (obj) { obj->QueueDelete(); }
	}
	DxMenuObject* p = GetParentFromID();
	if (p) { p->Enable(); }
}

void DxMenuObject::Activate() {
	//Only activate your menu object *after* you set up all the important parts!
	bActive_ = true;
	optionIndex = 0;
	DxMenuObject* p = GetParentFromID();
	if (p) { p->Disable(); }
}

void DxMenuObject::Disable() {
	for (auto& id : relatedIDs) {
		DxScriptObjectBase* obj = GetObjectFromID(id);
		if (obj) { obj->SetVis(false); }
	}
	flags.disable = true;
}

void DxMenuObject::Enable() {
	for (auto& id : relatedIDs) {
		DxScriptObjectBase* obj = GetObjectFromID(id);
		if (obj) { obj->SetVis(true); }
	}
	flags.disable = false;
}

void DxMenuObject::Work() {
	// Do stuff
	if (!bActive_) { return; }
	if (flags.disable) {
		flags.actionT = false;
		timer = 0;
		return; 
	}
	timer++;

	ProcessMenuInputs();
	if (optionType.at(0) == TYPE_KEYBOARD) {
		OptionHandler_Keyboard();
	}
	else {
		OptionHandler();
	}
}

void DxMenuObject::ProcessMenuInputs() {
	input_bitfield thisFrame;

	int16_t keys[8] = { KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,
	KEY_SHOT, KEY_BOMB, KEY_USER1, KEY_USER2 };

	if (timer < 5) {
		return;
	}

	//there's probably an efficient loop I can make for this but I don't feel like it...
	//nvm yes i do
	//edit: 2 hours later, i regret every decision that led me to this.
	//nvm fuck compact efficient loops i give up C has bested me in combat
	if ((input->GetVirtualKeyState(KEY_LEFT) == KEY_PUSH) || (input->GetVirtualKeyState(KEY_LEFT) == KEY_HOLD)) {
		thisFrame.left = true;
		buttonTimer[keys[0]]++;
	}
	else {
		buttonTimer[keys[0]] = 0;
	}

	if ((input->GetVirtualKeyState(KEY_RIGHT) == KEY_PUSH) || (input->GetVirtualKeyState(KEY_RIGHT) == KEY_HOLD)) {
		thisFrame.right = true;
		buttonTimer[keys[1]]++;
	}
	else {
		buttonTimer[keys[1]] = 0;
	}

	if ((input->GetVirtualKeyState(KEY_UP) == KEY_PUSH) || (input->GetVirtualKeyState(KEY_UP) == KEY_HOLD)) {
		thisFrame.up = true;
		buttonTimer[keys[2]]++;
	}
	else {
		buttonTimer[keys[2]] = 0;
	}

	if ((input->GetVirtualKeyState(KEY_DOWN) == KEY_PUSH) || (input->GetVirtualKeyState(KEY_DOWN) == KEY_HOLD)) {
		thisFrame.down = true;
		buttonTimer[keys[3]]++;
	}
	else {
		buttonTimer[keys[3]] = 0;
	}

	if ((input->GetVirtualKeyState(KEY_SHOT) == KEY_PUSH) || (input->GetVirtualKeyState(KEY_SHOT) == KEY_HOLD)) {
		thisFrame.shot = true;
		buttonTimer[keys[4]]++;
	}
	else {
		buttonTimer[keys[4]] = 0;
	}

	if ((input->GetVirtualKeyState(KEY_BOMB) == KEY_PUSH) || (input->GetVirtualKeyState(KEY_BOMB) == KEY_HOLD)) {
		thisFrame.bomb = true;
		buttonTimer[keys[5]]++;
	}
	else {
		buttonTimer[keys[5]] = 0;
	}

	if ((input->GetVirtualKeyState(KEY_USER1) == KEY_PUSH) || (input->GetVirtualKeyState(KEY_USER1) == KEY_HOLD)) {
		thisFrame.user1 = true;
		buttonTimer[keys[6]]++;
	}
	else {
		buttonTimer[keys[6]] = 0;
	}

	if ((input->GetVirtualKeyState(KEY_USER2) == KEY_PUSH) || (input->GetVirtualKeyState(KEY_USER2) == KEY_HOLD)) {
		thisFrame.user2 = true;
		buttonTimer[keys[7]]++;
	}
	else {
		buttonTimer[keys[7]] = 0;
	}

	/*
	std::string msg = "";
	msg = msg + "Left: " + (thisFrame.left ? "1" : "0") + ", ";
	msg = msg + "Right: " + (thisFrame.right ? "1" : "0") + ", ";
	msg = msg + "Up: " + (thisFrame.up ? "1" : "0") + ", ";
	msg = msg + "Down: " + (thisFrame.down ? "1" : "0") + ", ";
	msg = msg + "Shot: " + (thisFrame.shot ? "1" : "0") + ", ";
	msg = msg + "Bomb: " + (thisFrame.bomb ? "1" : "0") + ", ";
	msg = msg + "User1: " + (thisFrame.user1 ? "1" : "0") + ", ";
	msg = msg + "User2: " + (thisFrame.user2 ? "1" : "0");
	Logger::WriteTop(msg);
	*/
	// Just confirmed with Logger 8/13/2024 this works.  Any changes to this fxn since unconfirmed.
	
	lastKey = thisFrame;
}

void DxMenuObject::OptionHandler() {

	flags.actionT = false;

	// keeping this the same, would love to make it const, but meh
	int16_t keys[8] = { KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,
	KEY_SHOT, KEY_BOMB, KEY_USER1, KEY_USER2 };


	// might want to consider some checking on value of optionType[optionIndex] using std::vector.at
	
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

	if (optionType[optionIndex] == TYPE_SLIDER) {
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
	if (lastKey.up) {
		if ( ((buttonTimer[keys[2]] > buttonDelay) && (buttonTimer[keys[2]] % scrollInterval == 0)) || (buttonTimer[keys[2]] == 1) ) {
			optionIndex = (optionIndex - 1 + maxIndex) % maxIndex;
		}
	}

	if (lastKey.down) {
		if ( ((buttonTimer[keys[3]] > buttonDelay) && (buttonTimer[keys[3]] % scrollInterval == 0)) || (buttonTimer[keys[3]] == 1) ) {
			optionIndex = (optionIndex + 1) % maxIndex;
		}
	}

	if (lastKey.shot) {
		if ((buttonTimer[keys[4]] == 1) && (timer > 30)) {
			flags.actionT = true;
		}
	}

	if (lastKey.bomb) {
		if ((buttonTimer[keys[5]] == 1) && (timer > 30)) {
			if (optionType[optionIndex] == TYPE_MAIN) {
				optionIndex = maxIndex - 1;
			}
			else {
				if (GetParentFromID() != nullptr) {
					manager_->DeleteObject(this);
				}
			}
		}
	}
}

void DxMenuObject::OptionHandler_Keyboard() {

	int16_t letterKeys[26] =	{DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J,
								 DIK_K, DIK_L, DIK_M, DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T,
								 DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z};
	int16_t numberKeys[10] =	{DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9};
	wchar_t charCode = 0x00;
	const uint64_t ONE = 1;
	Logger::WriteTop(L"OptionHandler_Keyboard: " + std::to_wstring(keyboardButtonValue));

	for (uint64_t i = 0; i < 26; i++) {
		uint64_t power_2_i = ONE << i;
		if ( (input->GetKeyState(letterKeys[i]) == KEY_PUSH) || (input->GetKeyState(letterKeys[i]) == KEY_HOLD) ) {
			if ((keyboardButtonValue & power_2_i) == 0) { //bitwise arithmetic still needed to check if it was pressed last frame
				charCode = i + L'a';
				if ((input->GetKeyState(DIK_LSHIFT) == KEY_PUSH) || (input->GetKeyState(DIK_LSHIFT) == KEY_HOLD) || (input->GetKeyState(DIK_RSHIFT) == KEY_PUSH) || (input->GetKeyState(DIK_RSHIFT) == KEY_HOLD)) {
					charCode = i + L'A';
				}
				keyboardButtonValue = (keyboardButtonValue | power_2_i);
			}
		}
		else {
			keyboardButtonValue = (keyboardButtonValue & ~(power_2_i));
		}
	}

	for (uint64_t i = 0; i < 10; i++) {
		uint64_t power_2_i = ONE << (i+26);
		if ((input->GetKeyState(numberKeys[i]) == KEY_PUSH) || (input->GetKeyState(numberKeys[i]) == KEY_HOLD)) {
			if ((keyboardButtonValue & power_2_i) == 0) { //bitwise arithmetic still needed to check if it was pressed last frame
				charCode = i + L'0';
				keyboardButtonValue = (keyboardButtonValue | power_2_i);
			}
		}
		else {
			keyboardButtonValue = (keyboardButtonValue & ~(power_2_i));
		}
	}

	if (input->GetKeyState(DIK_BACKSPACE) == KEY_PUSH) {
		if (keyboardInput.length() > 0) {
			keyboardInput.pop_back();
		}
	}
	else if (input->GetKeyState(DIK_RETURN) == KEY_PUSH) {
		flags.actionT = true;
	}
	else if (timer > 5) {
		std::wstring newChar = L"";
		newChar.push_back(charCode);
		if (charCode != 0x00) {
			keyboardInput = keyboardInput + newChar;
		}
	}
}

// DxMenuObjectManger

DxMenuObjectManager::DxMenuObjectManager() {
}

// No need for this!  Handled in DxScript!
//void DxMenuObjectManager::ForceCloseAllMenus() {}