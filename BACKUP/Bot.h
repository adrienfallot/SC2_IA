#pragma once

#ifndef BOT_H
#define BOT_H

#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <vector>

#include "BuildingManager.h"

using namespace sc2;

class Bot : public Agent {
	BuildingManager buildingManager;

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

	Point2D transformPoint2D(Point2D point, int x_shift, int y_shift);
};

#endif // !BOT_H