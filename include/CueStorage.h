#pragma once
#include <Arduino.h>

enum CueType {
  CUE_TARGET,      // e.g. /cue/1/start
  CUE_WORKSPACE    // e.g. /go or /panic
};

struct Cue {
  uint8_t index;         // Order in our show list
  CueType type;
  String oscCommand;     // Full OSC string to send
  String cueID;          // Optional cue ID or number
  bool enabled;          // Whether it is part of the run show
};

void loadCues();
void saveCues();
void resetCues();

bool addCue(const Cue& cue);
bool editCue(uint8_t index, const Cue& updated);
bool deleteCue(uint8_t index);
void reorderCues(uint8_t fromIndex, uint8_t toIndex);

Cue* getCue(uint8_t index);
int getCueCount();
