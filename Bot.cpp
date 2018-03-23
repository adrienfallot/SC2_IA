#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <vector>

#include "Bot.h"

using namespace sc2;

void Bot::OnGameFullStart() {
	}

void Bot::OnGameStart() {
	Building* terranRafinery = new Building(UNIT_TYPEID::TERRAN_REFINERY, ABILITY_ID::BUILD_REFINERY, 75, 3, 3, 21);
	Building* terranCommandCenter = new Building(UNIT_TYPEID::TERRAN_COMMANDCENTER, ABILITY_ID::BUILD_COMMANDCENTER, 400, 3, 3, 71);
	Building* terranSupplyDepot = new Building(UNIT_TYPEID::TERRAN_SUPPLYDEPOT, ABILITY_ID::BUILD_SUPPLYDEPOT, 100, 2, 2, 21);
	Building* terranBarracks = new Building(UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::BUILD_BARRACKS, 150, 3, 3, 46);
	Building* terranAutoTurret = new Building(UNIT_TYPEID::TERRAN_AUTOTURRET, ABILITY_ID::EFFECT_AUTOTURRET, 125, 3, 3, 18);
	Building* terranMissileTurret = new Building(UNIT_TYPEID::TERRAN_MISSILETURRET, ABILITY_ID::BUILD_MISSILETURRET, 100, 3, 3, 18);
	Building* terranEngineeringBay = new Building(UNIT_TYPEID::TERRAN_ENGINEERINGBAY, ABILITY_ID::BUILD_ENGINEERINGBAY, 125, 3, 3, 25);
	terranMissileTurret->dependencies_.push_back(terranEngineeringBay);
	terranAutoTurret->dependencies_.push_back(terranEngineeringBay);
	terranBarracks->dependencies_.push_back(terranSupplyDepot);

	const Unit* commandCenter = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
	Point2D commandCenterPosition = commandCenter->pos; //114.5 25.5
	Point2D secondCommandCenterPosition = Point2D(98.5, 138.5);
	std::vector<Build*> buildsPile;

	/**/

	buildsPile.push_back(new Build(terranMissileTurret, TransformPoint2D(commandCenterPosition, 11, -3)));
	buildsPile.push_back(new Build(terranMissileTurret, TransformPoint2D(commandCenterPosition, 13, -3)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(commandCenterPosition, 13, 0)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, 13, 3)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, -3, -3)));

	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(secondCommandCenterPosition, 2, -7)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(secondCommandCenterPosition, 2, -10)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(secondCommandCenterPosition, -2, -7)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(secondCommandCenterPosition, -2, -10)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(secondCommandCenterPosition, 6, -4)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(secondCommandCenterPosition, 9, -4)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(secondCommandCenterPosition, 8, -7)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(secondCommandCenterPosition, 8, -10)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(secondCommandCenterPosition, 6, -7)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(secondCommandCenterPosition, 6, -10)));
	buildsPile.push_back(new Build(terranCommandCenter, secondCommandCenterPosition));

	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, 3, 3)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, 5, 3)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, 7, 3)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, 9, 3)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, 11, 3)));
	buildsPile.push_back(new BuildRafinery(terranRafinery));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, -5, -3)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, -7, -3)));
	buildsPile.push_back(new Build(terranBarracks, TransformPoint2D(commandCenterPosition, -9, -3)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(commandCenterPosition, 3, 0)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(commandCenterPosition, 9, 0)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(commandCenterPosition, 11, 0)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(commandCenterPosition, -3, 0)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(commandCenterPosition, -9, 0)));
	buildsPile.push_back(new BuildRafinery(terranRafinery));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(commandCenterPosition, -11, 0)));
	buildsPile.push_back(new Build(terranSupplyDepot, TransformPoint2D(commandCenterPosition, -13, 0)));
	buildsPile.push_back(new Build(terranMissileTurret, TransformPoint2D(commandCenterPosition, -11, -3)));
	buildsPile.push_back(new Build(terranMissileTurret, TransformPoint2D(commandCenterPosition, -13, -3)));
	//45.5, 20.5
	//98.5, 138.5
	building_manager_ = new BuildingManager(buildsPile);
	production_manager_ = new ProductionManager();
	soldier_manager_ = new SoldierManager();
}

void Bot::OnGameEnd() {

}

void Bot::OnStep() {
	building_manager_->Update(this);
	production_manager_->Update(this);
	soldier_manager_->Update(this);
}

void Bot::OnUnitCreated(const Unit* unit) {

}

void Bot::OnUnitDestroyed(const Unit* unit) {

}

void Bot::OnUnitIdle(const Unit* unit) {
	production_manager_->OnUnitIdle(this, unit);
	soldier_manager_->OnUnitIdle(this, unit);
}

void Bot::OnBuildingConstructionComplete(const Unit* unit) {
	soldier_manager_->OnBuildingConstructionComplete(this, unit);
}

void Bot::OnUpgradeCompleted(const Unit* unit) {

}

void Bot::OnUnitEnterVision(const Unit* unit) {

}

void Bot::OnNydusDetected() {

}

void Bot::OnNuclearLaunchDetected() {

}

void Bot::OnError(const std::vector<ClientError>& client_errors, const std::vector<std::string>& protocol_errors) {

}

Point2D Bot::TransformPoint2D(Point2D point, int x_shift, int y_shift) {
	return Point2D(point.x + x_shift, point.y + y_shift);
}

const Unit* Bot::FindNearestMineralPatch(const Point2D& start) {
	Units units = this->Observation()->GetUnits(Unit::Alliance::Neutral);
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