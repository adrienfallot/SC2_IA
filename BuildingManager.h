#pragma once

#ifndef BUILDING_MANAGER_H
#define BUILDING_MANAGER_H

#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"
class Bot;

using namespace sc2;

enum CONSTRUCTION_STATE {INEXISTANT, IN_CONSTRUCTION, BUILT};

class Building {

	UNIT_TYPEID id_;
	ABILITY_ID construction_id_;  // Id of the action to construct.
	int minerals_to_build_;
	int width_;
	int height_;
	int construction_duration_;  // Time of construction in second.

	int GetNumberOfBuildingOfThisType(const ObservationInterface *observation);

	int GetNumberOfBuildingOfThisTypeInConstruction(const ObservationInterface *observation);

	bool DependenciesCompleted(const ObservationInterface *observation);

	bool DependenciesInConstruction(const ObservationInterface *observation);

 public:

	std::vector<Building*> dependencies_;

	Building();

	Building(UNIT_TYPEID id, ABILITY_ID constructionId, int mineralsToBuild, int width, int height, int constructionDuration);
	
	//<Getter>
	int GetCostToBuildInMinerals();

	ABILITY_ID GetIdOfActionToBuild();
	//</Getter>

	CONSTRUCTION_STATE GetDependenciesConstructionState(const ObservationInterface *observation);

	std::vector<Building*> GetUnbuiltDependencies(const ObservationInterface *observation);
};

class Build {

	Point2D position_;
	bool core_building_;  // Is part of the preselected strat's build order.
	bool rebuild_if_destroyed_;

 public:

	Building *building_;  // Do not modify it except if you want to modify this for every other class using it.

	Build();

	Build(Building *building, Point2D position, bool core_building = false, bool rebuild_if_destroyed = false);

	//<Getter>
	Point2D GetPosition();

	const Point2D GetPosition() const;
	//</Getter>
};

// TODO, The differents buildings should be interface for the parameters not changings.

class BuildOrder {

	// Determine which building to build, and when. Also has a build order at start.
 public:

	std::vector<Build*> builds_pile_;

	BuildOrder();

	BuildOrder(std::vector<Build*> builds_pile);

	bool is_empty();
	// TODO add function to determine what to build next in function of game observation and current buildings_pile. 
	// FOR EXAMPLE : if supply_max < 200 and buildings_pile doesn't contain 2 supply_max per barracks + command center : add one.
};

class BuildingManager {

	// Tells the building what to produce, and when.
	BuildOrder *build_order_;

	void TryBuilding(Bot *bot);

	void BuildBuilding(Bot *bot, const Build* building_to_build);

	const Unit* GetBuilder(const ObservationInterface *observation);

	// TODO :: Find a better name : InexistantDependencies do not reflect well we are talking about unbuilt + not in construction.
	// TODO :: Find why we can't have a const Building* as argument for this function (doesn't work for getUnbuiltDependencies call
	void AddInexistantDependenciesToPile(const ObservationInterface *observation, Building* building_to_build);

	// TODO :: EXIST IN MULTIPLE CLASS, SRP BROKEN!
	Point2D BuildingManager::TransformPoint2D(Point2D point, int x_shift, int y_shift);

 public:
	BuildingManager();
	BuildingManager(BuildOrder* openingBuildOrder);
	BuildingManager(std::vector<Build*> buildings_pile);

	void Update(Bot *bot);

};

#endif // !BUILDING_MANAGER_H

// TODO If dependency not complete, build it.

// TODO Ajouter "if destroyed -> rebuild if rebuildIfDestroyed".
