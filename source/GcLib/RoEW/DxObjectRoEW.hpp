#include "../pch.h"

#include "../directx/DxObject.hpp"
#include "../directx/DxScript.hpp"
#include "../directx/DirectInput.hpp"

namespace directx {

	//****************************************************************************
	//DxMenuObject
	//****************************************************************************

	class DxMenuObject : public DxScriptObjectBase {
		friend DxScript;

		protected:
			//gstd::script_block* func_store = nullptr;
			//std::vector<int> attached_objIDs;
			//gstd::script_machine* machine;
			//gstd::value idObjectValue_;
			// Rest in peace, the Jank-o-tron 9000.  We hardly knew ye.

			struct flags {
				bool disable		: 1;
				bool error			: 1;
				bool close			: 1;
				bool verify			: 1;
				bool popup			: 1;
				bool canAct			: 1;
				bool rotate			: 1;
				unsigned remainder 	: 1;
			};
			// This syntax for flag storage is actually absurdly helpful.
			// Vastly prefer it to normal bitfields with bitwise arithmetic...
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
			std::vector<gstd::ref_count_weak_ptr<DxScriptObjectBase>> relatedIDs;
			std::map<unsigned int, unsigned int> optionIndexX;
			std::map<unsigned int, int> maxIndex;
			std::map<unsigned int, float> sliderValue;
			std::map<unsigned int, int> sliderMin;
			std::map<unsigned int, int> sliderMax;
			std::map<int16_t, int> buttonTimer;
			int16_t lastKey;

		public:
			DxMenuObject();
			virtual void Activate();
			virtual void Work();
			void SetParent(DxScriptObjectBase* _p) { parent = _p; }
	};

	class DxMenuObjectManager : public DxScriptObjectBase {
		friend DxScript;

	protected:
		std::map<int, gstd::value> menuResult;

	public:
		DxMenuObjectManager();
		void ForceCloseAllMenus();
	};

}