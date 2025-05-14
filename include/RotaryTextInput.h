#pragma once
#include <Arduino.h>

void initTextInput();
void initNumberInput(int maxLen = 99);
void initIPInput();
void initPrefilledInput(const String& initialText, bool numberOnly = false, bool ipOnly = false, int maxLen = 16);
bool updateTextInput(); 
bool didUserCancel();
String getFinalInput();

