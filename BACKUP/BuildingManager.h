#pragma once

#ifndef BUILDING_MANAGER_H
#define BUILDING_MANAGER_H

#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <vector>

using namespace sc2;

enum CONSTRUCTION_STATE {INEXISTANT, IN_CONSTRUCTION, BUILT};

struct Building {
	UNIT_TYPEID id;
	ABILITY_ID constructionId;  //Id of the action to construct.
	int mineralsToBuild;
	int width;
	int height;
	int buildingDuration;
	std::vector<UNIT_TYPEID> dependencies;
};

class Building {
public:
	UNIT_TYPEID id;
	ABILITY_ID constructionId;  //Id of the action to construct.
	Point2D position;
	int mineralsToBuild;
	int width;
	int height;
	int buildingDuration;
	std::vector<UNIT_TYPEID> dependencies;
	std::vector<ABILITY_ID> dependenciesAbilityId;
	bool coreBuilding;  //Is part of the preselected strat's build order.
	bool rebuildIfDestroyed;

	Building();

	Building(UNIT_TYPEID id, ABILITY_ID constructionId, Point2D position, int mineralsToBuild, int width, int height,
										int buildingDuration, bool coreBuilding = false, bool rebuildIfDestroyed = false);

	CONSTRUCTION_STATE Building::getDependenciesConstructionState(const ObservationInterface *observation);

	bool dependenciesCompleted(const ObservationInterface *observation);

	bool dependenciesInConstruction(const ObservationInterface *observation);

	//TODO :: SINGLE RESPONSABILITY PRINCIPLE.
	int Building::getNumberOfBuildingInConstruction(const ObservationInterface *observation, ABILITY_ID buildingAbility);
};

class TerranSupplyDepot : public Building {
public:
	TerranSupplyDepot(Point2D position, bool coreBuilding, bool rebuildIfDestroyed);
};

class TerranBarracks : public Building {
public:
	TerranBarracks(Point2D position, bool coreBuilding, bool rebuildIfDestroyed);
};

class TerranMissileTurret : public Building {
public:
	TerranMissileTurret(Point2D position, bool coreBuilding, bool rebuildIfDestroyed);
};

class TerranEngineeringBay : public Building {
public:
	TerranEngineeringBay(Point2D position, bool coreBuilding, bool rebuildIfDestroyed);
};

//TODO, The differents buildings should be interface for the parameters not changings.

class BuildOrder {
	//Determine which building to build, and when. Also has a build order at start.
public:
	std::vector<Building> buildingsPile;

	BuildOrder();

	BuildOrder(std::vector<Building> buildingsPile);

	bool isEmpty();
	//TODO add function to determine what to build next in function of game observation and current buildings_pile. 
	// FOR EXAMPLE : if supply_max < 200 and buildings_pile doesn't contain 2 supply_max per barracks + command center : add one.
};

class BuildingManager {
public:
	//Tells the building what to produce, and when.
	BuildOrder openingBuildOrder;

	BuildingManager();

	BuildingManager(BuildOrder openingBuildOrder);

	BuildingManager(std::vector<Building> buildingsPile);

	void Update(Agent *bot);

	void TryBuilding(Agent *bot);

	const Unit* getBuilder(const ObservationInterface *observation);

	Point2D BuildingManager::transformPoint2D(Point2D point, int x_shift, int y_shift);
};

#endif // !BUILDING_MANAGER_H

//TODO If dependency not complete, build it.