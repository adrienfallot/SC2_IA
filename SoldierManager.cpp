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
	if (std::find(idle_soldiers_.begin(), idle_soldiers_.end(), unit_idle) == idle_soldiers_.end()) {
		idle_soldiers_.push_back(unit_idle);
		if (unit_idle->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARINE) {
			number_of_idle_marines_++;
		}
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

	bool enought_marines_for_attack = (number_of_idle_marines_ >= number_min_of_marines_to_attack_);
	std::cout << enought_marines_for_attack << std::endl;
	for (int i = 0; i < idle_soldiers_.size(); i++) {
		action_found_for_soldier = false;
		const Unit* soldier_idle = idle_soldiers_[i];
		switch (soldier_idle->unit_type.ToType())
		{
		case UNIT_TYPEID::TERRAN_MARINE:
			std::cout << "ZOUP" << std::endl;
			action_found_for_soldier = DetermineActionForMarine(bot, soldier_idle, enought_marines_for_attack);
			break;
		default:
			//If unit isn't managed by this module, delete it from vector.
			std::cout << "ZOP" << std::endl;
			idle_soldiers_.erase(idle_soldiers_.begin() + i);
			i--;
			break;
		}

		if (action_found_for_soldier) {
			idle_soldiers_.erase(idle_soldiers_.begin() + i);
			i--;
			std::cout << "BOUP" << std::endl;
		}
	}
}

bool SoldierManager::DetermineActionForMarine(Bot* bot, const Unit* marine_idle, bool enought_marines_for_attack)
{
	if (enought_marines_for_attack)
	{
		const GameInfo& game_info = bot->Observation()->GetGameInfo();
		bot->Actions()->UnitCommand(marine_idle, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		number_of_idle_marines_--;
		std::cout << "BIP" << std::endl;
		return true;
	}
	return false;
}