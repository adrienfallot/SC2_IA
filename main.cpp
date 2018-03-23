#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <vector>

#include "BuildingManager.h"
#include "Bot.h"

using namespace sc2;

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
		CreateComputer(Race::Terran, Difficulty::Easy)
		});

	coordinator.LaunchStarcraft();
	coordinator.StartGame(sc2::kMapBelShirVestigeLE);
	while (coordinator.Update()) {
	}

	return 0;
}