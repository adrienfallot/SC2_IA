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

	int number_of_idle_marines_ = 0;
	int number_of_sneeky_idle_marines_ = 0;
	int number_min_of_marines_to_attack_ = 20;
	int number_min_of_sneeky_marines_to_attack_ = 35;

	std::vector<Point2D> DetermineDefensePoint();  //TODO : Determine which point on the map should be defended.

	void DetermineActionForIdleSoldiers(Bot* bot);

	bool SoldierManager::DetermineActionForGhost(Bot* bot, const Unit* marauder_idle, bool enought_marines_for_attack, bool enought_sneeky_marines_for_attack);

	bool DetermineActionForMarauder(Bot* bot, const Unit* marauder_idle, bool enought_marines_for_attack, bool enought_sneeky_marines_for_attack);

	bool DetermineActionForMarine(Bot* bot, const Unit* marine_idle, bool enought_marines_for_attack, bool enought_sneeky_marines_for_attack);
	
	bool DetermineActionForSCV(Bot* bot, const Unit* scv_idle);

	void GetIdleSneekySCVToHarvest(Bot* bot);

	void GetNewSCVToWork(Bot* bot, const Unit* unit);
	
	const Unit* FindNearestMineralPatch(const Point2D& start, Bot *bot);

	Point2D FindRandomLocationInArea(Point2D center, float radius);

	Point2D GetRandomPositionInArea(Point2D center, float radius);

public:
	SoldierManager();

	void Update(Bot* bot);

	void OnBuildingConstructionComplete(Bot* bot, const Unit* unit);

	void OnUnitIdle(Bot* bot, const Unit* buidling_idle);

};

#endif  //!UNIT_MANAGER_H