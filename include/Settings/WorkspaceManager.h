#pragma once
#include <Arduino.h>

namespace osc_controller::settings {

struct Workspace {
    String name;
    bool isPrimary;
};

void loadWorkspaces();
void saveWorkspaces();
int getWorkspaceCount();
Workspace getWorkspace(int index);
void addWorkspace(const String& name);
void deleteWorkspace(int index);
void setPrimaryWorkspace(int index);
Workspace getPrimaryWorkspace();
void editWorkspace(int index, const String& newName);

} // namespace osc_controller::settings

