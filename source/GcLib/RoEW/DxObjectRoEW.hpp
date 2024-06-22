#include "../pch.h"

#include "../directx/DxObject.hpp"
#include "../directx/DxScript.hpp"
#include "../directx/DirectInput.hpp"
#include "../../TouhouDanmakufu/Common/DnhGcLibImpl.hpp"

namespace directx {

	//****************************************************************************
	//DxMenuObject
	//****************************************************************************

	class DxMenuObject : public DxScriptObjectBase {
		friend DxScript;
		friend DxMenuObjectManager; 

		protected:
			//gstd::script_block* func_store = nullptr;
			//std::vector<int> attached_objIDs;
			//gstd::script_machine* machine;
			//gstd::value idObjectValue_;
			// Rest in peace, the Jank-o-tron 9000.  We hardly knew ye.
			struct flag_bitfield {
				bool disable : 1;
				bool error : 1;
				bool close : 1;
				bool verify : 1;
				bool popup : 1;
				bool canAct : 1;
				bool rotate : 1;
				bool actionT : 1;
			};
			// actionT - action trigger
			// if a menu option was selected
			struct input_bitfield {
				bool left : 1;
				bool right : 1;
				bool up : 1;
				bool down : 1;
				bool shot : 1;
				bool bomb : 1;
				bool user1 : 1;
				bool user2 : 1;
			};
			// This syntax for flag storage is actually absurdly helpful.
			// Vastly prefer it to normal bitfields with bitwise arithmetic...
			flag_bitfield flags;
			int timer;
			enum t_option {
				TYPE_INVALID = 0,
				TYPE_KEYBOARD = 1,
				TYPE_XAXIS = 2,
				TYPE_SLIDER = 3,
				TYPE_MAIN = 4,
				TYPE_NORMAL = 5
			};
			gstd::ref_count_weak_ptr<DxScriptObjectBase, false> parent;
			unsigned int optionIndex;
			unsigned int maxIndex;
			std::vector<gstd::ref_count_weak_ptr<DxScriptObjectBase, false>> relatedIDs;
			std::map<unsigned int, unsigned int> optionIndexX;
			std::map<unsigned int, int> maxIndexX;
			std::map<unsigned int, float> sliderValue;
			std::map<unsigned int, int> sliderMin;
			std::map<unsigned int, int> sliderMax;
			std::map<unsigned int, float> sliderIncr;
			std::vector<t_option> optionType;
			std::map<int16_t, int> buttonTimer;
			static input_bitfield lastKey;
			std::wstring keyboardInput;
			uint64_t keyboardButtonValue = std::numeric_limits<uint64_t>::max() - 1;

			const static int buttonDelay = 30;
			const static int scrollInterval = 10;
			const static int scrollInterval_slider = 3;

			void ProcessMenuInputs();
			void OptionHandler();
			void OptionHandler_Keyboard();

		public:
			DxMenuObject();
			~DxMenuObject();
			virtual void Activate();
			virtual void Work();

			int GetParent() { return parent->GetObjectID(); }
			bool GetDisabled() { return flags.disable; }
			int GetRelatedObject(int index) { return relatedIDs[index] != nullptr ? relatedIDs[index]->GetObjectID() : DxScript::ID_INVALID; }
			int GetOptionIndex() { return optionIndex; }
			int GetOptionIndexX(int index) { return optionIndexX[index]; }
			int GetMaxIndex() { return maxIndex; }
			int GetMaxIndexX(int index) { return maxIndexX[index]; }
			float GetSliderValue(int index) { return sliderValue[index]; }
			int GetSliderMin(int index) { return sliderMin[index]; }
			int GetSliderMax(int index) { return sliderMax[index]; }
			float GetSliderIncr(int index) { return sliderIncr[index]; }
			int GetOptionType(int index) { return (int)optionType[index]; }
			int GetActionFlag() { return flags.actionT; }

			void SetParent(DxScriptObjectBase* _p) { parent = _p; }
			void AddRelatedObject(DxScriptObjectBase* _obj) { 
				gstd::ref_count_weak_ptr<DxScriptObjectBase, false> _o; 
				_o = _obj; 
				relatedIDs.push_back(_o); 
			}
			void SetOptionIndex(unsigned int _arg) { optionIndex = _arg; }
			void SetMaxIndex(unsigned int _arg) { maxIndex = _arg; }
			void SetOptionIndexX(unsigned int index, unsigned int _a2) { optionIndexX[index] = _a2; }
			void SetMaxIndexX(unsigned int index, unsigned int _a2) { maxIndexX[index] = _a2; }
			void SetSliderValue(unsigned int index, float f) { sliderValue[index] = f; }
			void SetSliderMax(unsigned int index, int val) { sliderMax[index] = val; }
			void SetSliderMin(unsigned int index, int val) { sliderMin[index] = val; }
			void SetSliderIncr(unsigned int index, float f) { sliderIncr[index] = f; }

	};

	class DxMenuObjectManager : public DxScriptObjectBase {
		friend DxScript;

	protected:
		std::vector<int> menuIDs;
		std::map<int, gstd::value> menuResult;

	public:
		DxMenuObjectManager();
		void ForceCloseAllMenus();
		gstd::value GetReturnValue(int oid) { return menuResult[oid]; }
		void SetReturnValue(int oid, gstd::value val) { menuResult[oid] = val; }
	};

}