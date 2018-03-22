#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <vector>

#include "BuildingManager.h"
#include "Bot.h"

using namespace sc2;
/*
class Building {
public:
	UNIT_TYPEID id;
	ABILITY_ID constructionId;  //Id of the action to construct.
	Point2D position;
	int mineralsToBuild;
	bool coreBuilding;  //Is part of the preselected strat's build order.
	bool rebuildIfDestroyed;

	Building() {
		this->id = UNIT_TYPEID::INVALID;
		this->constructionId = ABILITY_ID::INVALID;
		this->position = Point2D();
		this->mineralsToBuild = 1000;
		this->coreBuilding = false;
		this->rebuildIfDestroyed = false;
	}

	Building(UNIT_TYPEID id, ABILITY_ID constructionId, Point2D position, int mineralsToBuild,
								bool coreBuilding = false, bool rebuildIfDestroyed = false) {
		this->id = id;
		this->constructionId = constructionId;
		this->position = position;
		this->mineralsToBuild = mineralsToBuild;
		this->coreBuilding = coreBuilding;
		this->rebuildIfDestroyed = rebuildIfDestroyed;
	}
};

class TerranSupplyDepot : public Building {
public:
	TerranSupplyDepot(Point2D position, bool coreBuilding, bool rebuildIfDestroyed) : Building(UNIT_TYPEID::TERRAN_SUPPLYDEPOT, ABILITY_ID::BUILD_SUPPLYDEPOT, position, 100, coreBuilding, rebuildIfDestroyed) {
	}
};

class BuildOrder {
	//Determine which building to build, and when. Also has a build order at start.
public:
	std::vector<Building> buildingsPile;

	BuildOrder() {
		//this->buildingsPile.push_back(Building());
	}

	BuildOrder(std::vector<Building> buildingsPile) {
		this->buildingsPile = buildingsPile;
	}

	bool isEmpty() {
		return this->buildingsPile.size() == 0;
	}
	//TODO add function to determine what to build next in function of game observation and current buildings_pile. 
	// FOR EXAMPLE : if supply_max < 200 and buildings_pile doesn't contain 2 supply_max per barracks + command center : add one.
};

class BuildingManager {
public:
	//Tells the building what to produce, and when.
	BuildOrder openingBuildOrder;

	BuildingManager() {
		this->openingBuildOrder = BuildOrder();
	}

	BuildingManager(BuildOrder openingBuildOrder) {
		this->openingBuildOrder = openingBuildOrder;
	}

	BuildingManager(std::vector<Building> buildingsPile) {
		this->openingBuildOrder = BuildOrder(buildingsPile);
	}

	void Update(Agent *bot) {
		const ObservationInterface *observation = bot->Observation();
		if (!openingBuildOrder.isEmpty()) {
			Building buildingToBuild = openingBuildOrder.buildingsPile.back();
			if (observation->GetMinerals() > buildingToBuild.mineralsToBuild) {
				const Unit* unitBuilder = getBuilder(observation);
				bot->Actions()->UnitCommand(unitBuilder, buildingToBuild.constructionId, buildingToBuild.position);
				openingBuildOrder.buildingsPile.pop_back();
			}
		}
	}

	const Unit* getBuilder(const ObservationInterface *observation) {
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
};

class Bot : public Agent {
	BuildingManager buildingManager;
	virtual void OnGameFullStart() final {
	
	}

	virtual void OnGameStart() final {
		const Unit* commandCenter = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
		Point2D commandCenterPosition = commandCenter->pos;
		std::vector<Building> buildingOrder;
		buildingOrder.push_back(TerranSupplyDepot(transformPoint2D(commandCenterPosition, 3, 3), true, false));
		buildingOrder.push_back(TerranSupplyDepot(transformPoint2D(commandCenterPosition, 5, 3), true, false));
		buildingOrder.push_back(TerranSupplyDepot(transformPoint2D(commandCenterPosition, 7, 3), true, false));
		buildingOrder.push_back(TerranSupplyDepot(transformPoint2D(commandCenterPosition, 9, 3), true, false));

		buildingOrder.push_back(TerranSupplyDepot(transformPoint2D(commandCenterPosition, -3, -3), true, false));
		buildingOrder.push_back(TerranSupplyDepot(transformPoint2D(commandCenterPosition, -5, -3), true, false));
		buildingOrder.push_back(TerranSupplyDepot(transformPoint2D(commandCenterPosition, -7, -3), true, false));
		buildingOrder.push_back(TerranSupplyDepot(transformPoint2D(commandCenterPosition, -9, -3), true, false));
		buildingManager = BuildingManager(buildingOrder);
	}

	virtual void OnGameEnd() final {
	
	}

	virtual void OnStep() final {
		buildingManager.Update(this);
	}

	virtual void OnUnitCreated(const Unit* unit) final {

	}

	virtual void OnUnitDestroyed(const Unit* unit) final {

	}

	virtual void OnUnitIdle(const Unit* unit) final {
	
	}

	virtual void OnBuildingConstructionComplete(const Unit* unit) final {

	}

	virtual void OnUpgradeCompleted(const Unit* unit) final {

	}

	virtual void OnUnitEnterVision(const Unit* unit) final {

	}

	virtual void OnNydusDetected() final {

	}

	virtual void OnNuclearLaunchDetected() final {

	}

	virtual void OnError(const std::vector<ClientError>& client_errors, const std::vector<std::string>& protocol_errors = {}) final {

	}

	Point2D transformPoint2D(Point2D point, int x_shift, int y_shift) {
		return Point2D(point.x + x_shift, point.y + y_shift);
	}
};
*/
/*
class Bot : public Agent {
	int numberOfVespeneGeyser;
	public:
		virtual void OnGameStart() final {
			numberOfVespeneGeyser = 0;
		}

		virtual void OnUnitIdle(const Unit* unit) final {
			switch (unit->unit_type.ToType()) {
			case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
				break;
			}
			case UNIT_TYPEID::TERRAN_SCV: {
				const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
				if (!mineral_target) {
					break;
				}
				Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
				break;
			}
			case UNIT_TYPEID::TERRAN_BARRACKS: {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
				break;
			}
			case UNIT_TYPEID::TERRAN_MARINE: {
				if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 10) {
					const GameInfo& game_info = Observation()->GetGameInfo();
					Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
				}
				break;
			}
			default: {
				break;
			}
			}
		}

		const Unit* FindNearestMineralPatch(const Point2D& start) {
			Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
			float distance = std::numeric_limits<float>::max();
			const Unit* target = nullptr;
			for (const auto& u : units) {
				if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
					float d = DistanceSquared2D(u->pos, start);
					if (d < distance) {
						distance = d;
						target = u;
					}
				}
			}
			return target;
		}

		const Unit* FindNearestGeyser(const Point2D& start) {
			Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
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

		virtual void OnStep() {
			const ObservationInterface* observation = Observation();
			TryBuildSupplyDepot();
			if(observation->GetMinerals() > 500)
				TryBuildVespeneGeyser();
			TryBuildBarracks();
		}

		size_t CountUnitType(UNIT_TYPEID unit_type) {
			return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
		}

		bool TryBuildBarracks() {
			const ObservationInterface* observation = Observation();

			if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
				return false;
			}

			if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 2) {
				return false;
			}

			return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
		}

		bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV) {
			const ObservationInterface* observation = Observation();

			// If a unit already is building a supply structure of this type, do nothing.
			// Also get an scv to build the structure.
			const Unit* unit_to_build = nullptr;
			int already_building = 0;
			Units units = observation->GetUnits(Unit::Alliance::Self);
			for (const auto& unit : units) {
				for (const auto& order : unit->orders) {
					if (order.ability_id == ability_type_for_structure) {
						already_building++;
						if (already_building >= 2)
							return false;
					}
				}

				if (unit->unit_type == unit_type) {
					unit_to_build = unit;
				}
			}

			float rx = GetRandomScalar();
			float ry = GetRandomScalar();

			Actions()->UnitCommand(unit_to_build,
				ability_type_for_structure,
				Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));

			return true;
		}

		bool TryBuildSupplyDepot() {
			const ObservationInterface* observation = Observation();

			// If we are not supply capped, don't build a supply depot.
			if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
				return false;

			// Try and build a depot. Find a random SCV and give it the order.
			return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
		}

		bool TryBuildVespeneGeyser() {
			const ObservationInterface* observation = Observation();
			if(numberOfVespeneGeyser <= 0){
				const Unit* unit_to_build = nullptr;
				Units units = observation->GetUnits(Unit::Alliance::Self);
				for (const auto& unit : units) {
					if (unit->unit_type == UNIT_TYPEID::TERRAN_SCV) {
						unit_to_build = unit;
					}
				}
				
				numberOfVespeneGeyser++;
				Actions()->UnitCommand(unit_to_build,
					ABILITY_ID::BUILD_REFINERY,
					FindNearestGeyser(unit_to_build->pos));
			}
			
			return true;
		}

		virtual void OnBuildingConstructionComplete(const Unit *unit) final {
			const ObservationInterface* observation = Observation();
			int toto = (int) (unit->unit_type.ToType());
			std::cout << toto << std::endl;
			switch (unit->unit_type.ToType()) {
			case UNIT_TYPEID::TERRAN_REFINERY: {
				Units units = observation->GetUnits(Unit::Alliance::Self);
				for (const auto& unit_ : units) {
					if (unit_->unit_type == UNIT_TYPEID::TERRAN_SCV) {
						std::cout << "ENTER IF" << std::endl;
						Actions()->UnitCommand(unit_, ABILITY_ID::HARVEST_GATHER, unit);
						if (unit->assigned_harvesters >= 3) {
							break;
						}
					}
				}
				break;
			}
			default: {
				break;
			}
			}
		}
};
*/

class Base {
	//A base is a commandCenter and its directSurroundings.
};

//TODO : Determine the repartition of minerals between BuildOrder and BuildingManager
//TODO : Parse every class in a superclass and then make it adaptable with Strategy Pattern.

int main(int argc, char* argv[]) {
	Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	Bot bot;
	coordinator.SetParticipants({
		CreateParticipant(Race::Terran, &bot),
		CreateComputer(Race::Zerg)
		});

	coordinator.LaunchStarcraft();
	coordinator.StartGame(sc2::kMapBelShirVestigeLE);
	while (coordinator.Update()) {
	}

	return 0;
}