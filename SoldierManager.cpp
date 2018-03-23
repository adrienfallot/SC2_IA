#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"
#include "SoldierManager.h"
#include "Bot.h"

using namespace sc2;

SoldierManager::SoldierManager() {}

void SoldierManager::OnBuildingConstructionComplete(Bot* bot, const Unit* unit) {
	GetNewSCVToWork(bot, unit);
	GetIdleSneekySCVToHarvest(bot);
}

void SoldierManager::GetNewSCVToWork(Bot* bot, const Unit* unit) {
	const Unit* nearest_mineral_patch = bot->FindNearestMineralPatch(unit->pos);
	if (unit->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER) {
		bot->Actions()->UnitCommand(unit, ABILITY_ID::SMART, nearest_mineral_patch);
	}
}

void SoldierManager::GetIdleSneekySCVToHarvest(Bot* bot) {
	Units units = bot->Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
	bool action_found;

	for (const auto& unit : units) {
		if (unit->orders.size() == 0) {
			if (unit->pos.x > 80 || unit->pos.y > 120) {
				action_found = DetermineActionForSCV(bot, unit);
			}
		}
	}
}

void SoldierManager::OnUnitIdle(Bot* bot, const Unit* unit_idle)
{
	if (std::find(idle_soldiers_.begin(), idle_soldiers_.end(), unit_idle) == idle_soldiers_.end()) {
		if (unit_idle->unit_type.ToType() != UNIT_TYPEID::TERRAN_SCV) {
			idle_soldiers_.push_back(unit_idle);
		}
		if (unit_idle->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARINE) {
			if (unit_idle->pos.x > 80 && unit_idle->pos.y > 120) {
				number_of_sneeky_idle_marines_++;
			}
			else {
				number_of_idle_marines_++;
			}
		}
		else if (unit_idle->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARAUDER  || unit_idle->unit_type.ToType() == UNIT_TYPEID::TERRAN_GHOST) {
			if (unit_idle->pos.x > 80 && unit_idle->pos.y > 120) {
				number_of_sneeky_idle_marines_ += 2;
			}
			else {
				number_of_idle_marines_ += 2;
			}
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
	bool enought_sneeky_marines_for_attack = (number_of_sneeky_idle_marines_ >= number_min_of_sneeky_marines_to_attack_);
	for (int i = 0; i < idle_soldiers_.size(); i++) {
		action_found_for_soldier = false;
		const Unit* soldier_idle = idle_soldiers_[i];
		switch (soldier_idle->unit_type.ToType())
		{
		case UNIT_TYPEID::TERRAN_MARINE:
			action_found_for_soldier = DetermineActionForMarine(bot, soldier_idle, enought_marines_for_attack, enought_sneeky_marines_for_attack);
			break;
		case UNIT_TYPEID::TERRAN_MARAUDER:
				action_found_for_soldier = DetermineActionForMarauder(bot, soldier_idle, enought_marines_for_attack, enought_sneeky_marines_for_attack);
				break;
		case UNIT_TYPEID::TERRAN_GHOST:
			action_found_for_soldier = DetermineActionForMarauder(bot, soldier_idle, enought_marines_for_attack, enought_sneeky_marines_for_attack);
			break;
		case UNIT_TYPEID::TERRAN_SCV:
			action_found_for_soldier = DetermineActionForSCV(bot, soldier_idle);
			break;
		default:
			//If unit isn't managed by this module, delete it from vector.
			idle_soldiers_.erase(idle_soldiers_.begin() + i);
			i--;
			break;
		}

		if (action_found_for_soldier) {
			idle_soldiers_.erase(idle_soldiers_.begin() + i);
			i--;
		}
	}
}

bool SoldierManager::DetermineActionForGhost(Bot* bot, const Unit* marauder_idle, bool enought_marines_for_attack, bool enought_sneeky_marines_for_attack)
{
	return DetermineActionForGhost(bot, marauder_idle, enought_marines_for_attack, enought_sneeky_marines_for_attack);
}

bool SoldierManager::DetermineActionForMarauder(Bot* bot, const Unit* marauder_idle, bool enought_marines_for_attack, bool enought_sneeky_marines_for_attack)
{
	return DetermineActionForMarine(bot, marauder_idle, enought_marines_for_attack, enought_sneeky_marines_for_attack);
}

bool SoldierManager::DetermineActionForMarine(Bot* bot, const Unit* marine_idle, bool enought_marines_for_attack, bool enought_sneeky_marines_for_attack)
{
	if (enought_marines_for_attack && (marine_idle->pos.x <= 80 || marine_idle->pos.y <= 120))
	{
		const GameInfo& game_info = bot->Observation()->GetGameInfo();
		bot->Actions()->UnitCommand(marine_idle, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		number_of_idle_marines_--;
		return true;
	}
	else if (enought_sneeky_marines_for_attack && marine_idle->pos.x > 80 && marine_idle->pos.y > 120)
	{
		const GameInfo& game_info = bot->Observation()->GetGameInfo();
		bot->Actions()->UnitCommand(marine_idle, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		number_of_sneeky_idle_marines_--;
		return true;
	}
	return false;
}

bool SoldierManager::DetermineActionForSCV(Bot* bot, const Unit* scv_idle){
	
	if (scv_idle->pos.x <= 80 || scv_idle->pos.y <= 120)
	{
		Units refineries = bot->Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
		std::cout << refineries.size() << std::endl;
		
		for (const auto& refinery : refineries) {

			if (refinery->assigned_harvesters < refinery->ideal_harvesters) {
				bot->Actions()->UnitCommand(scv_idle, ABILITY_ID::HARVEST_GATHER, refinery);
			}
		}

	}
	else if (scv_idle->pos.x > 85 && scv_idle->pos.y > 120)
	{
		const Unit* nearest_mineral_patch = FindNearestMineralPatch(scv_idle->pos, bot);
		bot->Actions()->UnitCommand(scv_idle, ABILITY_ID::HARVEST_GATHER, nearest_mineral_patch);
		return true;
	}
	return false;
}

const Unit* SoldierManager::FindNearestMineralPatch(const Point2D& start, Bot *bot) {
	Units units = bot->Observation()->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}