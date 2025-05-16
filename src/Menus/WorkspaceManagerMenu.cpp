#include <Arduino.h>
#include "Globals.h"
#include "Menus/MainMenu.h"
#include "Menus/WorkspaceManagerMenu.h"
#include "Settings/AppState.h"
#include "Settings/WorkspaceManager.h"
#include "Settings/LanguageManager.h"
#include "Utilities/RotaryTextInput.h"

namespace osc_controller::menus {
using namespace osc_controller;

void handleWorkspaceManagerMenu() {
    const char* items[] = {
        settings::t("add_workspace_item"),
        settings::t("edit_workspace_item"),
        settings::t("delete_workspace_item"),
        settings::t("set_primary_workspace_item")
    };
    int itemCount = sizeof(items) / sizeof(items[0]);

    static int lastDrawnIndex = -1;
    long newPos = encoder.getPosition();
    selectedIndex = (newPos % itemCount + itemCount) % itemCount;

    if (forceRedraw || selectedIndex != lastDrawnIndex) {
        drawMenu(items, itemCount);
        lastDrawnIndex = selectedIndex;
        forceRedraw = false;
    }

    if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
        lastPressTime = millis();
        switch (selectedIndex) {
            case 0: handleAddWorkspaceMenu(); break;
            case 1: handleEditWorkspaceMenu(); break;
            case 2: handleDeleteWorkspaceMenu(); break;
            case 3: handleSetPrimaryWorkspaceMenu(); break;
        }
        resetMenuState();
        lastDrawnIndex = -1;
    }

    if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
        currentState = settings::SETTINGS_MENU;
        resetMenuState();
        lastDrawnIndex = -1;
        lcd.clear();
    }
}

void handleAddWorkspaceMenu() {
    lcd.clear();
    lcd.print(settings::t("enter_workspace_name"));
    delay(500);

    utilities::initTextInput();
    while (!utilities::updateTextInput()) encoder.tick();

    if (!utilities::didUserCancel()) {
        String name = utilities::getFinalInput();
        settings::addWorkspace(name);
        settings::saveWorkspaces();
        lcd.clear(); lcd.print(settings::t("workspace_added"));
        delay(800);
    } else {
        lcd.clear(); lcd.print(settings::t("cancelled_label"));
        delay(800);
    }
}

void handleEditWorkspaceMenu() {
    int count = settings::getWorkspaceCount();
    if (count == 0) {
        lcd.clear(); lcd.print(settings::t("no_workspaces"));
        delay(1000); return;
    }

    long newPos = encoder.getPosition();
    int selected = (newPos % count + count) % count;

    settings::Workspace ws = settings::getWorkspace(selected);
    lcd.clear(); lcd.print(settings::t("edit_workspace"));

    utilities::initPrefilledInput(ws.name);
    while (!utilities::updateTextInput()) encoder.tick();

    if (!utilities::didUserCancel()) {
        String newName = utilities::getFinalInput();
        settings::editWorkspace(selected, newName);
        settings::saveWorkspaces();
        lcd.clear(); lcd.print(settings::t("workspace_updated"));
        delay(800);
    } else {
        lcd.clear(); lcd.print(settings::t("cancelled_label"));
        delay(800);
    }
}

void handleDeleteWorkspaceMenu() {
    int count = settings::getWorkspaceCount();
    if (count == 0) {
        lcd.clear(); 
        lcd.print(settings::t("no_workspaces"));
        delay(1000); 
        return;
    }

    long lastPos = -999;
    int selected = 0;

    while (true) {
        encoder.tick();
        long newPos = encoder.getPosition();
        selected = (newPos % count + count) % count;

        if (newPos != lastPos) {
            lastPos = newPos;
            settings::Workspace ws = settings::getWorkspace(selected);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(settings::t("delete_q"));
            lcd.setCursor(0, 1);
            lcd.print(ws.name.substring(0, 16));
        }

        if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            settings::deleteWorkspace(selected);
            settings::saveWorkspaces();
            lcd.clear(); 
            lcd.print(settings::t("workspace_deleted"));
            delay(800);
            return;
        }

        if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            lcd.clear(); 
            lcd.print(settings::t("cancelled_label"));
            delay(800);
            return;
        }
    }
}

void handleSetPrimaryWorkspaceMenu() {
    int count = settings::getWorkspaceCount();
    if (count == 0) {
        lcd.clear(); 
        lcd.print(settings::t("no_workspaces"));
        delay(1000); 
        return;
    }

    int primaryIndex = -1;
    for (int i = 0; i < count; ++i) {
        if (settings::getWorkspace(i).isPrimary) {
            primaryIndex = i;
            break;
        }
    }

    long lastPos = -999;
    int selected = 0;

    lcd.clear();
    lcd.print(settings::t("select_primary"));
    delay(500);

    while (true) {
        encoder.tick();
        long newPos = encoder.getPosition();
        selected = (newPos % count + count) % count;

        if (newPos != lastPos) {
            lastPos = newPos;
            settings::Workspace ws = settings::getWorkspace(selected);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(settings::t("select_primary"));
            lcd.setCursor(0, 1);
            if (selected == primaryIndex) {
                lcd.print("*");
            } else {
                lcd.print(" ");
            }
            lcd.print(ws.name.substring(0, 15));
        }

        if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            settings::setPrimaryWorkspace(selected);
            settings::saveWorkspaces();
            lcd.clear();
            lcd.print(settings::t("primary_set"));
            lcd.setCursor(0, 1);
            lcd.print(settings::getWorkspace(selected).name.substring(0, 16));
            delay(1000);
            return;
        }

        if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            lcd.clear();
            lcd.print(settings::t("cancelled_label"));
            delay(800);
            return;
        }
    }
}

} // namespace osc_controller::menus

