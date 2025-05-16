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
    wsPrefs.begin(WS_NAMESPACE, true);
    workspaceCount = wsPrefs.getUInt("count", 0);
    primaryIndex = wsPrefs.getInt("primary", -1);

    for (int i = 0; i < workspaceCount && i < MAX_WORKSPACES; ++i) {
        String key = "ws" + String(i);
        workspaceList[i].name = wsPrefs.getString((key + "_name").c_str(), "");
        workspaceList[i].isPrimary = (i == primaryIndex);
    }

    wsPrefs.end();
}

void saveWorkspaces() {
    wsPrefs.begin(WS_NAMESPACE, false);
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
    if (index < 0 || index >= workspaceCount) return {"", false};
    return workspaceList[index];
}

void addWorkspace(const String& name) {
    if (workspaceCount >= MAX_WORKSPACES) return;
    workspaceList[workspaceCount] = {name, false};
    workspaceCount++;
    saveWorkspaces();
}

void deleteWorkspace(int index) {
    if (index < 0 || index >= workspaceCount) return;

    for (int i = index; i < workspaceCount - 1; ++i) {
        workspaceList[i] = workspaceList[i + 1];
    }
    workspaceCount--;

    if (primaryIndex == index) {
        primaryIndex = -1;
    } else if (primaryIndex > index) {
        primaryIndex--;
    }

    saveWorkspaces();
}

void editWorkspace(int index, const String& newName) {
    if (index < 0 || index >= workspaceCount) return;
    workspaceList[index].name = newName;
    saveWorkspaces();
}

void setPrimaryWorkspace(int index) {
    if (index < 0 || index >= workspaceCount) return;

    primaryIndex = index;
    for (int i = 0; i < workspaceCount; ++i) {
        workspaceList[i].isPrimary = (i == index);
    }
    saveWorkspaces();
}

Workspace getPrimaryWorkspace() {
    if (primaryIndex < 0 || primaryIndex >= workspaceCount) return {"", false};
    return workspaceList[primaryIndex];
}

} // namespace osc_controller::settings

