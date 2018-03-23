#pragma once

#ifndef PRODUCTION_MANAGER_H
#define PRODUCTION_MANAGER_H

#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"
class Bot;

using namespace sc2;

class ProductionManager {

 private:
	int current_infantery_armor_level = 0;
	int current_infantery_weapon_level = 0;

	std::vector<const Unit*> idle_buildings_;

	void DetermineProductionForIdleBuildings(Bot* bot);

	bool DetermineProductionForCommandCenter(Bot* bot, const Unit* command_center_idle);

	bool DetermineProductionForBarracks(Bot* bot, const Unit* barracks_idle);

	bool DetermineProductionForEngineeringBay(Bot* bot, const Unit* barracks_idle);

	bool UnitCanBeConstructed(const ObservationInterface* observation, const int mineral_cost_of_unit, const int vespene_cost_of_unit, const int space_taken_by_unit);
	
	bool GetIfNeedATechLab(Bot *bot, const Unit* barrack_idle);

	bool GetIfHasATechLab(Bot *bot, const Unit* barrack_idle);

 public:
	ProductionManager();

	void Update(Bot* bot);

	void OnUnitIdle(Bot* bot, const Unit* buidling_idle);

};

#endif  //!PRODUCTION_MANAGER_H