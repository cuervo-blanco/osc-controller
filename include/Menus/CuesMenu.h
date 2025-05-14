#pragma once
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

#include "Settings/AppState.h"
#include "CueStorage.h"

namespace osc_controller::menus {
void handleCuesMenu();
void handleAddCueMenu();
void handleEditCueMenu();
void handleDeleteCueMenu();
void handleCueControlMenu();
void handleReorderCueMenu();
void handleEnterQLabIP();
} // namespace Menus
