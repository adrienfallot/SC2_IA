#pragma once

#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"

class Bot;

using namespace sc2;

class SoldierManager {

private:
	std::vector<const Unit*> idle_soldiers_;

	int number_of_idle_marines = 0;

	std::vector<Point2D> DetermineDefensePoint();  //TODO : Determine which point on the map should be defended.

	void DetermineActionForIdleSoldiers(Bot* bot);

	bool DetermineActionForMarine(Bot* bot, const Unit* marine_idle, bool send_marines);

public:
	SoldierManager();

	void Update(Bot* bot);

	void OnUnitIdle(Bot* bot, const Unit* buidling_idle);
};

#endif  //!UNIT_MANAGER_H