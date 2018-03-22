#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <vector>

#include "BuildingManager.h"
#include "Bot.h"

using namespace sc2;

void Bot::OnGameFullStart() {

	}

void Bot::OnGameStart() {
	const Unit* commandCenter = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
	Point2D commandCenterPosition = commandCenter->pos;
	std::vector<Building> buildingOrder;
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, 3, 3), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, 5, 3), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, 7, 3), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, 9, 3), true, false));

	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, 9, 3), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, -3, -3), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, -5, -3), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, -7, -3), true, false));
	buildingOrder.push_back(TerranBarracks(transformPoint2D(commandCenterPosition, -9, -3), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, -9, -6), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, -12, -9), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, 12, -9), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, 12, 9), true, false));
	buildingOrder.push_back(TerranMissileTurret(transformPoint2D(commandCenterPosition, -12, 9), true, false));

	buildingManager = BuildingManager(buildingOrder);
}

void Bot::OnGameEnd() {

}

void Bot::OnStep() {
	buildingManager.Update(this);
}

void Bot::OnUnitCreated(const Unit* unit) {

}

void Bot::OnUnitDestroyed(const Unit* unit) {

}

void Bot::OnUnitIdle(const Unit* unit) {

}

void Bot::OnBuildingConstructionComplete(const Unit* unit) {

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

Point2D Bot::transformPoint2D(Point2D point, int x_shift, int y_shift) {
	return Point2D(point.x + x_shift, point.y + y_shift);
}