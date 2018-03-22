#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"
#include "MapParser.h"

MapParser::MapParser() {
	;
}

std::vector<Point2D> MapParser::GetDefensePoint() {
	return defense_point_;
}