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
	std::vector<const Unit*> idle_buildings_;

	void DetermineProductionForIdleBuildings(Bot* bot);

	bool DetermineProductionForCommandCenter(Bot* bot, const Unit* command_center_idle);

	bool DetermineProductionForBarracks(Bot* bot, const Unit* barracks_idle);

	bool UnitCanBeConstructed(const ObservationInterface* observation, const int cost_of_unit, const int space_taken_by_unit);

 public:
	ProductionManager();

	void Update(Bot* bot);

	void OnUnitIdle(Bot* bot, const Unit* buidling_idle);

};

#endif  //!PRODUCTION_MANAGER_H