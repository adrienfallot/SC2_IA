#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <vector>

#include "BuildingManager.h"
#include "Bot.h"

using namespace sc2;

Building::Building() {
	this->id = UNIT_TYPEID::INVALID;
	this->constructionId = ABILITY_ID::INVALID;
	this->position = Point2D();
	this->mineralsToBuild = 0;
	this->width = 0;
	this->height = 0;
	this->buildingDuration = 0;
	this->coreBuilding = false;
	this->rebuildIfDestroyed = false;
}

Building::Building(UNIT_TYPEID id, ABILITY_ID constructionId, Point2D position, int mineralsToBuild, int width, int height,
															int buildingDuration, bool coreBuilding, bool rebuildIfDestroyed) {
	this->id = id;
	this->constructionId = constructionId;
	this->position = position;
	this->mineralsToBuild = mineralsToBuild;
	this->width = width;
	this->height = height;
	this->buildingDuration = buildingDuration;
	this->coreBuilding = coreBuilding;
	this->rebuildIfDestroyed = rebuildIfDestroyed;
}

CONSTRUCTION_STATE Building::getDependenciesConstructionState(const ObservationInterface *observation) {
	if (dependenciesCompleted(observation)) {
		std::cout << "RETURN BUILT" << std::endl;
		return CONSTRUCTION_STATE::BUILT;
	}
	if (dependenciesInConstruction(observation)) {
		return CONSTRUCTION_STATE::IN_CONSTRUCTION;
	}
	return CONSTRUCTION_STATE::INEXISTANT;
}

bool Building::dependenciesCompleted(const ObservationInterface *observation) {
	for (const auto& dependency : dependencies) {
		std::cout << "BUILDING :: " << observation->GetUnits(Unit::Alliance::Self, IsUnit(dependency)).size() << std::endl;
		if (observation->GetUnits(Unit::Alliance::Self, IsUnit(dependency)).size() - getNumberOfBuildingInConstruction(observation, dependenciesAbility[0] == 0) {
			return false;
		}
	}
	std::cout << "RETURN TRUE" << std::endl;
	return true;
}

bool Building::dependenciesInConstruction(const ObservationInterface *observation) {
	Units units = observation->GetUnits(Unit::Alliance::Self);
	for (const auto& dependencyAbility : dependenciesAbilityId) {
		for (const auto& unit : units) {
			for (const auto& order : unit->orders) {
				if (order.ability_id == dependencyAbility) {
					return true;
				}
			}
		}
		return false;
	}
}

//TODO :: SINGLE RESPONSABILITY PRINCIPLE : change this function of class or interface.
int Building::getNumberOfBuildingInConstruction(const ObservationInterface *observation, ABILITY_ID buildingAbility) {
	int numberOfBuildingInConstruction = 0;
	Units units = observation->GetUnits(Unit::Alliance::Self);
	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == buildingAbility) {
				numberOfBuildingInConstruction;
			}
		}
	}
	return numberOfBuildingInConstruction;
}

//TODO DEPENDENCY YET ONLY WORKS FOR ONE DEPENDENCY, BUT THEY CAN BE MULTIPLE.

TerranSupplyDepot::TerranSupplyDepot(Point2D position, bool coreBuilding, bool rebuildIfDestroyed) : Building(
	UNIT_TYPEID::TERRAN_SUPPLYDEPOT, ABILITY_ID::BUILD_SUPPLYDEPOT, position, 100, 2, 2, 21, coreBuilding, rebuildIfDestroyed) {}

TerranBarracks::TerranBarracks(Point2D position, bool coreBuilding, bool rebuildIfDestroyed) : Building(
	UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::BUILD_BARRACKS, position, 150, 3, 3, 46, coreBuilding, rebuildIfDestroyed) {}

TerranMissileTurret::TerranMissileTurret(Point2D position, bool coreBuilding, bool rebuildIfDestroyed) : Building(
	UNIT_TYPEID::TERRAN_MISSILETURRET, ABILITY_ID::BUILD_MISSILETURRET, position, 100, 3, 3, 18, coreBuilding, rebuildIfDestroyed) {
	this->dependencies.push_back(UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
	this->dependenciesAbilityId.push_back(ABILITY_ID::BUILD_ENGINEERINGBAY);
}

TerranEngineeringBay::TerranEngineeringBay(Point2D position, bool coreBuilding, bool rebuildIfDestroyed) : Building(
	UNIT_TYPEID::TERRAN_ENGINEERINGBAY, ABILITY_ID::BUILD_ENGINEERINGBAY, position, 125, 3, 3, 25, coreBuilding, rebuildIfDestroyed) {}


BuildOrder::BuildOrder() {}

BuildOrder::BuildOrder(std::vector<Building> buildingsPile) {
	this->buildingsPile = buildingsPile;
}

bool BuildOrder::isEmpty() {
	return this->buildingsPile.size() == 0;
}
//TODO add function to determine what to build next in function of game observation and current buildings_pile. 
// FOR EXAMPLE : if supply_max < 200 and buildings_pile doesn't contain 2 supply_max per barracks + command center : add one.

BuildingManager::BuildingManager() {
	this->openingBuildOrder = BuildOrder();
}

BuildingManager::BuildingManager(BuildOrder openingBuildOrder) {
	this->openingBuildOrder = openingBuildOrder;
}

BuildingManager::BuildingManager(std::vector<Building> buildingsPile) {
	this->openingBuildOrder = BuildOrder(buildingsPile);
}

void BuildingManager::Update(Agent *bot) {
	TryBuilding(bot);  // / ! \ Please note that this can change the build order.
}

void BuildingManager::TryBuilding(Agent *bot) {
	//Try building the top of the buildOrder pile if their is enought minerals for it.
	//If a building has to be constructed to build it, it will add it to the pile.
	const ObservationInterface *observation = bot->Observation();
	if (!openingBuildOrder.isEmpty()) {
		Building buildingToBuild = openingBuildOrder.buildingsPile.back();
		CONSTRUCTION_STATE dependenciesState = buildingToBuild.getDependenciesConstructionState(observation);
		if (dependenciesState == CONSTRUCTION_STATE::BUILT){
			std::cout << "BUILT" << std::endl;
			if (observation->GetMinerals() > buildingToBuild.mineralsToBuild) {
				std::cout << "ENOUGHT MINERALS" << std::endl;
				const Unit* unitBuilder = getBuilder(observation);
				bot->Actions()->UnitCommand(unitBuilder, buildingToBuild.constructionId, buildingToBuild.position);
				openingBuildOrder.buildingsPile.pop_back();
			}
		}
		else if (dependenciesState == CONSTRUCTION_STATE::INEXISTANT) {
			std::cout << "DOES NOT EXIST" << std::endl;
			const Unit* commandCenter = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
			Point2D commandCenterPosition = commandCenter->pos;
			openingBuildOrder.buildingsPile.push_back(TerranEngineeringBay(transformPoint2D(commandCenterPosition, -7, -7), true, false));
		}
		else {
			std::cout << "SHIT" << std::endl;
		}
	}
}

const Unit* BuildingManager::getBuilder(const ObservationInterface *observation) {
	const Unit* unit_to_build = nullptr;
	Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
	for (const auto& unit : units) {
		if (unit->orders.size() == 0) {
			return unit;
		}
	}

	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == ABILITY_ID::HARVEST_GATHER) {
				return unit;
			}
		}
	}

	return units[0];
}

Point2D BuildingManager::transformPoint2D(Point2D point, int x_shift, int y_shift) {
	return Point2D(point.x + x_shift, point.y + y_shift);
}