#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include "sc2utils/sc2_manage_process.h"
#include "BuildingManager.h"
#include "Bot.h"

using namespace sc2;

Building::Building() {
	this->id_ = UNIT_TYPEID::INVALID;
	this->construction_id_ = ABILITY_ID::INVALID;
	this->minerals_to_build_ = 0;
	this->width_ = 0;
	this->height_ = 0;
	this->construction_duration_ = 0;
}

Building::Building(UNIT_TYPEID id, ABILITY_ID construction_id, int minerals_to_build, int width, int height, int construction_duration) {
	this->id_ = id;
	this->construction_id_ = construction_id;
	this->minerals_to_build_ = minerals_to_build;
	this->width_ = width;
	this->height_ = height;
	this->construction_duration_ = construction_duration;
}

int Building::GetCostToBuildInMinerals() {
	return minerals_to_build_;
}

int Building::GetWidth() {
	return this->width_;
}

ABILITY_ID Building::GetIdOfActionToBuild() {
	return construction_id_;
}

int Building::GetNumberOfBuildingOfThisType(const ObservationInterface *observation) {
	Units units_of_this_type = observation->GetUnits(Unit::Alliance::Self, IsUnit(id_));
	return units_of_this_type.size() - GetNumberOfBuildingOfThisTypeInConstruction(observation);
}

int Building::GetNumberOfBuildingOfThisTypeInConstruction(const ObservationInterface *observation) {
	int number_of_building_of_this_type_in_construction = 0;
	Units units = observation->GetUnits(Unit::Alliance::Self);

	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == construction_id_) {
				number_of_building_of_this_type_in_construction++;
			}
		}
	}

	return number_of_building_of_this_type_in_construction;
}

bool Building::DependenciesCompleted(const ObservationInterface *observation) {
	for (const auto& dependency : dependencies_) {
		if (dependency->GetNumberOfBuildingOfThisType(observation) == 0) {
			return false;
		}
	}
	return true;
}

bool Building::DependenciesInConstruction(const ObservationInterface *observation) {
	for (const auto& dependency : dependencies_) {
		if (dependency->GetNumberOfBuildingOfThisTypeInConstruction(observation) == 0) {
			return false;
		}
	}
	return true;
}

CONSTRUCTION_STATE Building::GetDependenciesConstructionState(const ObservationInterface *observation) {
	if (DependenciesCompleted(observation)) {
		return CONSTRUCTION_STATE::BUILT;
	}
	if (DependenciesInConstruction(observation)) {
		return CONSTRUCTION_STATE::IN_CONSTRUCTION;
	}
	return CONSTRUCTION_STATE::INEXISTANT;
}

std::vector<Building*> Building::GetUnbuiltDependencies(const ObservationInterface *observation) {
	//TODO :: GET A BETTER NAME : it's not unbuilt dependencies but inexistants ones : unbuilt + not in construction.
	std::vector<Building*> unbuiltDependencies;

	for (Building* dependency : dependencies_) {
		if (dependency->GetNumberOfBuildingOfThisType(observation) == 0 && dependency->GetNumberOfBuildingOfThisTypeInConstruction(observation) == 0) {
			//TODO :: LINE TOO LONG, SHORTEN IT.
			unbuiltDependencies.push_back(dependency);
		}
	}

	return unbuiltDependencies;
}

Build::Build() {
	this->building_ = new Building();
	this->position_ = Point2D();
	this->core_building_ = false;
	this->rebuild_if_destroyed_ = false;
}

Build::Build(Building* building, Point2D position, bool coreBuilding, bool rebuildIfDestroyed) {
	this->building_ = building;
	this->position_ = position;
	this->core_building_ = coreBuilding;
	this->rebuild_if_destroyed_ = rebuildIfDestroyed;
}

const Point2D Build::GetPosition() const{
	return position_;
}


/////// BUILD RAFINERY /////

BuildRafinery::BuildRafinery() : Build() {

	this->vespene_geyser_ = nullptr;
}

BuildRafinery::BuildRafinery(Building* building, const Unit* vespene_geyser, bool coreBuilding,	bool rebuildIfDestroyed) :
															Build(building, Point2D(), coreBuilding, rebuildIfDestroyed) {

	this->vespene_geyser_ = vespene_geyser;
}

const Unit* BuildRafinery::GetGeyser() {
	return vespene_geyser_;
}

/////// BUILD ORDER  /////

BuildOrder::BuildOrder() {}

BuildOrder::BuildOrder(std::vector<Build*> builds_pile) {
	this->builds_pile_ = builds_pile;
}

bool BuildOrder::is_empty() {
	return this->builds_pile_.size() == 0;
}

void BuildOrder::DetermineNextBuilding(Bot *bot) {
	const ObservationInterface* observation = bot->Observation();
	const int COST_OF_COMMAND_CENTER = 400;
	if (number_of_bases_ == 1 && observation->GetMinerals() >= COST_OF_COMMAND_CENTER){
		Building* terranCommandCenter = new Building(UNIT_TYPEID::TERRAN_COMMANDCENTER, ABILITY_ID::BUILD_COMMANDCENTER, 400, 3, 3, 71);
		this->builds_pile_.insert(this->builds_pile_.begin(), 1, new Build(terranCommandCenter, Point2D(98.5, 138.5)));
	}
}

//TODO add function to determine what to build next in function of game observation and current buildings_pile. 
// FOR EXAMPLE : if supply_max < 200 and buildings_pile doesn't contain 2 supply_max per barracks + command center : add one.

BuildingManager::BuildingManager() {
	this->build_order_ = new BuildOrder();
}

BuildingManager::BuildingManager(BuildOrder* opening_build_order) {
	this->build_order_ = opening_build_order;
}

BuildingManager::BuildingManager(std::vector<Build*> builds_pile) {
	this->build_order_ = new BuildOrder(builds_pile);
}

void BuildingManager::Update(Bot *bot) {
	TryBuilding(bot);  // / ! \ Please note that this can change the build order.
}

void BuildingManager::TryBuilding(Bot *bot) {
	//Try building the top of the buildOrder pile if their is enought minerals for it.
	//If a building has to be constructed to build it, it will add it to the pile.
	const ObservationInterface *observation = bot->Observation();

	if (!build_order_->is_empty()) {
		const Build* building_to_build = build_order_->builds_pile_.back();
		CONSTRUCTION_STATE dependenciesState = building_to_build->building_->GetDependenciesConstructionState(observation);
		if (dependenciesState == CONSTRUCTION_STATE::BUILT){
			if (observation->GetMinerals() > building_to_build->building_->GetCostToBuildInMinerals()) {

				if (building_to_build->building_->id_ == UNIT_TYPEID::TERRAN_REFINERY) {
					BuildBuilding(bot, building_to_build, ((BuildRafinery*)building_to_build)->GetGeyser());
				}
				else {
					BuildBuilding(bot, building_to_build, building_to_build->GetPosition());
				}

			}
		}
		else if (dependenciesState == CONSTRUCTION_STATE::INEXISTANT) {
			AddInexistantDependenciesToPile(observation, building_to_build->building_);
		}
		else {
		}
	}
}

void BuildingManager::AddInexistantDependenciesToPile(const ObservationInterface *observation, Building* building_to_build) {
	//TODO :: Find a better name : InexistantDependencies do not reflect well we are talking about unbuilt + not in construction.
	//TODO :: Find why we can't have a const Building* as argument for this function (doesn't work for getUnbuiltDependencies call.
	//const Unit* commandCenter = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
	//Point2D commandCenterPosition = commandCenter->pos;

	for (Building* unbuilt_dependency : building_to_build->GetUnbuiltDependencies(observation)) {
		const Unit* command_center = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
		Point2D commandCenterPosition = command_center->pos; //114.5 25.5 ;; 29.5 134.5
		build_order_->builds_pile_.push_back(new Build(unbuilt_dependency, TransformPoint2D(commandCenterPosition, 4, 0), false, false));  //TODO FIND A METHOD FOR POSITION DYNAMIC
	}
}

void BuildingManager::BuildBuilding(Bot *bot, const Build* building_to_build, Vector2D target_position) {
	const Unit* unitBuilder = GetBuilder(bot->Observation(), building_to_build);

	bot->Actions()->UnitCommand(unitBuilder, building_to_build->building_->GetIdOfActionToBuild(), target_position);
	if (building_to_build->building_->GetIdOfActionToBuild() == ABILITY_ID::BUILD_BARRACKS && building_to_build->building_->GetWidth() == 4) {
		bot->barrackWithTechLab_.push_back(target_position);
		bot->barrackWithTechLab_.push_back(target_position);
	}
	build_order_->builds_pile_.pop_back();
}

void BuildingManager::BuildBuilding(Bot *bot, const Build* building_to_build, const Unit* target_geyser) {
	const Unit* unitBuilder = GetBuilder(bot->Observation(), building_to_build);

	const Unit* target = target_geyser;
	if (target == nullptr) {
		target = FindNearestGeyser(bot, unitBuilder->pos);
	}

	bot->Actions()->UnitCommand(unitBuilder, building_to_build->building_->GetIdOfActionToBuild(), target);

	build_order_->builds_pile_.pop_back();
}


const Unit* BuildingManager::FindNearestGeyser(Bot *bot, const Point2D& start) {
	Units units = bot->Observation()->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}

const Unit* BuildingManager::GetBuilder(const ObservationInterface *observation, const Build* building_to_build) {
	const Unit* unit_to_build = nullptr;
	Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
	
	if (building_to_build->GetPosition().x > 80 && building_to_build->GetPosition().y > 120) {
		for (const auto& unit : units) {
			if (unit->orders.size() == 0) {
				if (unit->pos.x > 80 || unit->pos.y > 120) {
					return unit;
				}
			}
		}
	}

	for (const auto& unit : units) {
		if (unit->orders.size() == 0) {
			if (unit->pos.x <= 80 || unit->pos.y <= 120) {
				return unit;
			}
		}
	}

	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == ABILITY_ID::HARVEST_GATHER) {
				if (unit->pos.x <= 80 || unit->pos.y <= 120) {
					return unit;
				}
			}
		}
	}

	return units[0];
}

Point2D BuildingManager::TransformPoint2D(Point2D point, int x_shift, int y_shift) {
	return Point2D(point.x + x_shift, point.y + y_shift);
}
