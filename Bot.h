#pragma once

#ifndef BOT_H
#define BOT_H

#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <vector>

#include "ProductionManager.h"
#include "BuildingManager.h"
#include "SoldierManager.h"

using namespace sc2;

class Bot : public Agent {
	BuildingManager* building_manager_;

	ProductionManager* production_manager_;

	SoldierManager* soldier_manager_;



	virtual void OnGameFullStart() final;

	virtual void OnGameStart() final;

	virtual void OnGameEnd() final;

	virtual void OnStep() final;

	virtual void OnUnitCreated(const Unit* unit) final;

	virtual void OnUnitDestroyed(const Unit* unit) final;

	virtual void OnUnitIdle(const Unit* unit) final;

	virtual void OnBuildingConstructionComplete(const Unit* unit) final;

	virtual void OnUpgradeCompleted(const Unit* unit) final;

	virtual void OnUnitEnterVision(const Unit* unit) final;

	virtual void OnNydusDetected() final;

	virtual void OnNuclearLaunchDetected() final;

	virtual void OnError(const std::vector<ClientError>& client_errors, const std::vector<std::string>& protocol_errors = {});

public:
	std::vector<Point2D> barrackWithTechLab_;

	Point2D TransformPoint2D(Point2D point, int x_shift, int y_shift);

	const Unit* FindNearestMineralPatch(const Point2D& start);
};

#endif // !BOT_H