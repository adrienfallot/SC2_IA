#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"
#include "ProductionManager.h"
#include "Bot.h"

using namespace sc2;

ProductionManager::ProductionManager() {}

void ProductionManager::OnUnitIdle(Bot* bot, const Unit* building_idle) {
	idle_buildings_.push_back(building_idle);
}


void ProductionManager::Update(Bot* bot) {
	// MAYBE A GOOD IMPROVMENT IN THE FUTURE :: DetermineProductionForIdle, new function : recolt need for future
	// Then make some scoring between proposed actions, and then choose.
	DetermineProductionForIdleBuildings(bot);
}

void ProductionManager::DetermineProductionForIdleBuildings(Bot* bot) {
	// The determined production for a building is either what they produce, either if they stay idle.
	// When they're not idle anymore, they're removed from idle_buildings_.
	bool action_found_for_building = false;

	for (int i = 0; i < idle_buildings_.size(); i++) {
		action_found_for_building = false;
		const Unit* building_idle = idle_buildings_[i];
		switch (building_idle->unit_type.ToType())
		{
		case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
			action_found_for_building = DetermineProductionForCommandCenter(bot, building_idle);
			break;
		}
		case UNIT_TYPEID::TERRAN_BARRACKS: {
			action_found_for_building = DetermineProductionForBarracks(bot, building_idle);
			break;
		}
		case UNIT_TYPEID::TERRAN_ENGINEERINGBAY: {
			action_found_for_building = DetermineProductionForEngineeringBay(bot, building_idle);
			break;
		}
		default:
			//If unit isn't managed by this module, delete it from vector.
			idle_buildings_.erase(idle_buildings_.begin() + i);
			i--;
			break;
		}

		if (action_found_for_building) {
			idle_buildings_.erase(idle_buildings_.begin() + i);
			i--;
		}
	}
}

bool ProductionManager::DetermineProductionForCommandCenter(Bot* bot, const Unit* command_center_idle) {
	// Returns true if action got assigned to the building.
	const int COST_OF_SCV = 50;  // TODO(team) :: Find a correct architecture to store Unit_Infos.
	const int SPACE_TAKEN_BY_SCV = 1;  // TODO(team) :: Find a correct architecture to store Unit_Infos.
	const ObservationInterface* observation = bot->Observation();
	if (command_center_idle->assigned_harvesters < command_center_idle->ideal_harvesters && UnitCanBeConstructed(observation, COST_OF_SCV, 0, SPACE_TAKEN_BY_SCV)) {
		bot->Actions()->UnitCommand(command_center_idle, ABILITY_ID::TRAIN_SCV);
		return true;
	}
	return false;
	// TODO(nathan) :: Add the refinery logic to the condition of TRAIN_SCV.
}

bool ProductionManager::DetermineProductionForBarracks(Bot* bot, const Unit* barracks_idle) {
	// Returns true if action got assigned to the building.
	const int COST_OF_MARINE = 50;
	const int SPACE_TAKEN_BY_MARINE = 1;
	const int COST_OF_MARAUDER = 100;
	const int COST_VESPENE_OF_MARAUDER = 25;
	const int SPACE_TAKEN_BY_MARAUDER = 2;
	const int COST_OF_TECH_LAB = 50;
	const int COST_VESPENE_OF_TECH_LAB = 25;
	const ObservationInterface* observation = bot->Observation();
	if (UnitCanBeConstructed(observation, COST_OF_TECH_LAB, COST_VESPENE_OF_TECH_LAB, 0)) {
		if (GetIfNeedATechLab(bot, barracks_idle)) {
			std::cout << "BIDIDI" << std::endl;
			bot->Actions()->UnitCommand(barracks_idle, ABILITY_ID::BUILD_TECHLAB_BARRACKS);
			bot->barrackWithTechLabBuilt_.push_back(barracks_idle->pos);
			return true;
		}
	}
	if (GetIfHasATechLab(bot, barracks_idle)) {
		if (UnitCanBeConstructed(observation, COST_OF_MARAUDER, COST_VESPENE_OF_MARAUDER, SPACE_TAKEN_BY_MARAUDER)) {
			bot->Actions()->UnitCommand(barracks_idle, ABILITY_ID::TRAIN_MARAUDER);
			return true;
		}
	}
	if (UnitCanBeConstructed(observation, COST_OF_MARINE, 0, SPACE_TAKEN_BY_MARINE)) {
		bot->Actions()->UnitCommand(barracks_idle, ABILITY_ID::TRAIN_MARINE);
		return true;
	}
	return false;
}

bool ProductionManager::GetIfNeedATechLab(Bot *bot, const Unit* barrack_idle) {
	int i = 0;
	for (i = 0; i < bot->barrackWithTechLab_.size(); i++) {
		if (bot->barrackWithTechLab_[i].x == barrack_idle->pos.x && bot->barrackWithTechLab_[i].y == barrack_idle->pos.y) {
			bot->barrackWithTechLab_.erase(bot->barrackWithTechLab_.begin() + i);
			return true;
		}
	}
	return false;
}

bool ProductionManager::GetIfHasATechLab(Bot *bot, const Unit* barrack_idle) {
	int i = 0;
	for (i = 0; i < bot->barrackWithTechLabBuilt_.size(); i++) {
		if (bot->barrackWithTechLabBuilt_[i].x == barrack_idle->pos.x && bot->barrackWithTechLabBuilt_[i].y == barrack_idle->pos.y) {
			return true;
		}
	}
	return false;
}

bool ProductionManager::DetermineProductionForEngineeringBay(Bot* bot, const Unit* engineeringBay_idle) {
	const int COST_OF_INFANTERY_WEAPON_1 = 100;
	const int COST_OF_INFANTERY_WEAPON_2 = 175;
	const int COST_OF_INFANTERY_WEAPON_3 = 250;
	const int COST_OF_INFANTERY_ARMOR_1 = 100;
	const int COST_OF_INFANTERY_ARMOR_2 = 175;
	const int COST_OF_INFANTERY_ARMOR_3 = 250;
	const ObservationInterface* observation = bot->Observation();

	if (current_infantery_weapon_level >= 3 && current_infantery_armor_level >= 3) {
		return true;
	}

	if (current_infantery_weapon_level <= current_infantery_armor_level) {
		if (current_infantery_weapon_level < 1) {
			if (UnitCanBeConstructed(observation, COST_OF_INFANTERY_WEAPON_1, COST_OF_INFANTERY_WEAPON_1, 0)) {
				bot->Actions()->UnitCommand(engineeringBay_idle, ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1);
				current_infantery_weapon_level++;
				return true;
			}
		}
		else if (current_infantery_weapon_level < 2) {  // TODO :: Add dependency to armurerie
			if (UnitCanBeConstructed(observation, COST_OF_INFANTERY_WEAPON_2, COST_OF_INFANTERY_WEAPON_2, 0)) {
				bot->Actions()->UnitCommand(engineeringBay_idle, ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2);
				current_infantery_weapon_level++;
				return true;
			}
		}
		else if (current_infantery_weapon_level < 3) {
			if (UnitCanBeConstructed(observation, COST_OF_INFANTERY_WEAPON_3, COST_OF_INFANTERY_WEAPON_3, 0)) {
				bot->Actions()->UnitCommand(engineeringBay_idle, ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3);
				current_infantery_weapon_level++;
				return true;
			}
		}
	}
	else {
		if (current_infantery_armor_level < 1) {
			if (UnitCanBeConstructed(observation, COST_OF_INFANTERY_ARMOR_1, COST_OF_INFANTERY_ARMOR_1, 0)) {
				bot->Actions()->UnitCommand(engineeringBay_idle, ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1);
				current_infantery_armor_level++;
				return true;
			}
		}
		else if (current_infantery_armor_level < 2) {  // TODO :: Add dependency to armurerie
			if (UnitCanBeConstructed(observation, COST_OF_INFANTERY_ARMOR_2, COST_OF_INFANTERY_ARMOR_2, 0)) {
				bot->Actions()->UnitCommand(engineeringBay_idle, ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2);
				current_infantery_armor_level++;
				return true;
			}
		}
		else if (current_infantery_armor_level < 3) {
			if (UnitCanBeConstructed(observation, COST_OF_INFANTERY_ARMOR_3, COST_OF_INFANTERY_ARMOR_3, 0)) {
				bot->Actions()->UnitCommand(engineeringBay_idle, ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3);
				current_infantery_armor_level++;
				return true;
			}
		}
	}
	return false;
}

bool ProductionManager::UnitCanBeConstructed(const ObservationInterface* observation, const int mineral_cost_of_unit, const int vespene_cost_of_unit, const int space_taken_by_unit) {
	// TODO (team) :: this shit violate SRP.
	int space_left = observation->GetFoodCap() - observation->GetFoodUsed();
	if (observation->GetMinerals() < mineral_cost_of_unit || observation->GetVespene() < vespene_cost_of_unit) {
		return false;
	}
	if (space_left < space_taken_by_unit) {
		return false;
	}
	return true;
}
