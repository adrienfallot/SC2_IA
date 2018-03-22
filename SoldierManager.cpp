#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"
#include "SoldierManager.h"
#include "Bot.h"

using namespace sc2;

SoldierManager::SoldierManager() {}

void SoldierManager::OnUnitIdle(Bot* bot, const Unit* unit_idle)
{
	idle_soldiers_.push_back(unit_idle);

	if (unit_idle->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARINE) {
		number_of_idle_marines++;
	}
}

void SoldierManager::Update(Bot* bot) {
	// MAYBE A GOOD IMPROVMENT IN THE FUTURE :: DetermineProductionForIdle, new function : recolt need for future
	// Then make some scoring between proposed actions, and then choose.
	DetermineActionForIdleSoldiers(bot);
}

void SoldierManager::DetermineActionForIdleSoldiers(Bot* bot)
{
	bool action_found_for_soldier = false;

	bool send_marines = (number_of_idle_marines >= 20);

	for (int i = 0; i < idle_soldiers_.size(); i++) {
		//std::cout << i << " < " << idle_soldiers_.size() << std::endl;
		const Unit* soldier_idle = idle_soldiers_[i];
		switch (soldier_idle->unit_type.ToType())
		{
		case UNIT_TYPEID::TERRAN_MARINE:
			action_found_for_soldier = DetermineActionForMarine(bot, soldier_idle, send_marines);
			break;
		default:
			//If unit isn't managed by this module, delete it from vector.
			idle_soldiers_.erase(idle_soldiers_.begin() + i);
			i = (i == 0 ? 0 : i - 1);
			break;
		}

		if (action_found_for_soldier) {
			std::cout << "HERE MIGHT UNIT ERROR" << std::endl;
			idle_soldiers_.erase(idle_soldiers_.begin() + i);
			i = (i == 0 ? 0 : i - 1);
			std::cout << "BUT LOOKS LIKE NONE OCCURED" << std::endl;
		}
	}
}

bool SoldierManager::DetermineActionForMarine(Bot* bot, const Unit* marine_idle, bool send_marines)
{
	if (send_marines)
	{
		const GameInfo& game_info = bot->Observation()->GetGameInfo();
		bot->Actions()->UnitCommand(marine_idle, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		number_of_idle_marines--;
		return true;
	}
	return false;
}