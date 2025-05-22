#include "Settings/WorkspaceManager.h"
#include <Preferences.h>

namespace osc_controller::settings {

#define WS_NAMESPACE "workspaces"
#define MAX_WORKSPACES 8

static Preferences wsPrefs;
static Workspace workspaceList[MAX_WORKSPACES];
static int workspaceCount = 0;
static int primaryIndex = -1;

void loadWorkspaces() {
  if (!wsPrefs.begin(WS_NAMESPACE, true)) {
        Serial.println("Failed to begin workspace preferences in read mode.");
        workspaceCount = 0;
        primaryIndex = -1;
        return;
    }
    workspaceCount = wsPrefs.getUInt("count", 0);
    primaryIndex = wsPrefs.getInt("primary", -1);

    if (workspaceCount > MAX_WORKSPACES) {
        Serial.println("Workspace count exceeds maximum. Resetting.");
        workspaceCount = MAX_WORKSPACES;
    }

    for (int i = 0; i < workspaceCount && i < MAX_WORKSPACES; ++i) {
        String key = "ws" + String(i);
        workspaceList[i].name = wsPrefs.getString((key + "_name").c_str(), "");
        workspaceList[i].isPrimary = (i == primaryIndex);
    }

    wsPrefs.end();
}

void saveWorkspaces() {
    if (!wsPrefs.begin(WS_NAMESPACE, false)) {
        Serial.println("Failed to begin workspace preferences in write mode.");
        return;
    }

    wsPrefs.putUInt("count", workspaceCount);
    wsPrefs.putInt("primary", primaryIndex);

    for (int i = 0; i < workspaceCount && i < MAX_WORKSPACES; ++i) {
        String key = "ws" + String(i);
        wsPrefs.putString((key + "_name").c_str(), workspaceList[i].name);
    }

    wsPrefs.end();
}

int getWorkspaceCount() {
    return workspaceCount;
}

Workspace getWorkspace(int index) {
   if (index < 0 || index >= workspaceCount) {
        Serial.printf("Invalid workspace index: %d\n", index);
        return {"", false};
    }
    return workspaceList[index];
}

void addWorkspace(const String& name) {
    if (workspaceCount >= MAX_WORKSPACES) {
        Serial.println("Cannot add workspace: maximum limit reached.");
        return;
    }
    if (name.isEmpty()) {
        Serial.println("Cannot add workspace: empty name.");
        return;
    }
    workspaceList[workspaceCount] = {name, false};
    workspaceCount++;
    saveWorkspaces();
}

void deleteWorkspace(int index) {
    if (index < 0 || index >= workspaceCount) {
        Serial.printf("Cannot delete workspace: invalid index %d\n", index);
        return;
    }

    for (int i = index; i < workspaceCount - 1; ++i) {
        workspaceList[i] = workspaceList[i + 1];
    }
    workspaceList[workspaceCount - 1] = {"", false};
    workspaceCount--;

    if (primaryIndex == index) {
        primaryIndex = -1;
    } else if (primaryIndex > index) {
        primaryIndex--;
    }

    saveWorkspaces();
}

void editWorkspace(int index, const String& newName) {
    if (index < 0 || index >= workspaceCount) {
        Serial.printf("Cannot edit workspace: invalid index %d\n", index);
        return;
    }

    if (newName.isEmpty()) {
        Serial.println("Cannot set empty workspace name.");
        return;
    }
    workspaceList[index].name = newName;
    saveWorkspaces();
}

void setPrimaryWorkspace(int index) {
    if (index < 0 || index >= workspaceCount) {
        Serial.printf("Cannot set primary workspace: invalid index %d\n", index);
        return;
    }

    primaryIndex = index;
    for (int i = 0; i < workspaceCount; ++i) {
        workspaceList[i].isPrimary = (i == index);
    }
    saveWorkspaces();
}

Workspace getPrimaryWorkspace() {
    if (primaryIndex < 0 || primaryIndex >= workspaceCount) {
        Serial.println("No valid primary workspace set.");
        return {"", false};
    }
    return workspaceList[primaryIndex];
}

} // namespace osc_controller::settings

