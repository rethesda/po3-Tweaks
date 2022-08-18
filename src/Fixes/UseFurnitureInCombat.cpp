#include "Fixes.h"
#include "Settings.h"

//use furniture in combat
//credit to KernalsEgg for implementation
namespace Fixes::UseFurnitureInCombat
{
	//replace IsInCombat with unused? vfunc that returns false
	namespace UseInCombat
	{
		void Install()
		{
			struct Patch : Xbyak::CodeGenerator
			{
				Patch()
				{
					call(qword[rax + 0x830]);
				}
			};

			Patch patch;
			patch.ready();

			static REL::Relocation<std::uintptr_t> target{ REL_ID(17034, 17420) };  // TESFurniture::Activate

			REL::safe_write(target.address() + 0x81, std::span{ patch.getCode(), patch.getSize() });
			REL::safe_write(target.address() + OFFSET(0x1B1, 0x1B2), std::span{ patch.getCode(), patch.getSize() });
		}
	}

	namespace PreventKickOut
	{
		struct StopInteractingQuick
		{
			static void thunk(RE::Actor* a_actor, bool a_arg2)
			{
				static auto setting = Settings::GetSingleton()->fixes.useFurnitureInCombat;
				if (setting == 1 && a_actor->IsPlayerRef() || setting == 2) {
					return;
				}

				return func(a_actor, a_arg2);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL_ID(37672, 38626), OFFSET(0x485, 0x480) };
			stl::write_thunk_call<StopInteractingQuick>(target.address());

			logger::info("Installed use furniture in combat tweak"sv);
		}
	}

	void Install()
	{
		UseInCombat::Install();
		PreventKickOut::Install();
	}
}
