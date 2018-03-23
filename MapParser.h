#pragma once

#ifndef MAP_PARSER_H
#define MAP_PARSER_H

#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"

using namespace sc2;

class MapParser {

private:
	std::vector<Point2D> defense_point_;

public:
	MapParser();

	void OnGameStart(); //TODO : Set first value of defense_point_.

	void Update();  //TODO : --low-priority -> Analyse part of map destroyed and adapt anything that should change after that.

	void OnBuildingConstructionCompleted(const Unit* unit);  //TODO : When CommandCenter built, add new defense point.

	void OnUnitDestroyed(const Unit* unit);  //TODO : When ComandCenter Destroyed, remove defense point

	std::vector<Point2D> GetDefensePoint();
};

#endif  //!MAP_PARSER_H