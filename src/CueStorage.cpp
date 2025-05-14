#include <Preferences.h>
#include "CueStorage.h"

namespace osc_controller {
#define CUE_NAMESPACE "cues"
#define MAX_CUES 32

static Preferences cuePrefs;
static Cue cueList[MAX_CUES];
static int cueCount = 0;

void loadCues() {
  cuePrefs.begin(CUE_NAMESPACE, true);
  cueCount = cuePrefs.getUInt("count", 0);

  for (int i = 0; i < cueCount && i < MAX_CUES; ++i) {
    Cue& c = cueList[i];
    String key = "cue" + String(i);

    c.index = i;
    c.type = (CueType)cuePrefs.getUChar((key + "_type").c_str(), CUE_TARGET);
    c.oscCommand = cuePrefs.getString((key + "_osc").c_str(), "");
    c.cueID = cuePrefs.getString((key + "_id").c_str(), "");
    c.enabled = cuePrefs.getBool((key + "_on").c_str(), true);
  }

  cuePrefs.end();
}

void saveCues() {
  cuePrefs.begin(CUE_NAMESPACE, false);
  cuePrefs.putUInt("count", cueCount);

  for (int i = 0; i < cueCount && i < MAX_CUES; ++i) {
    Cue& c = cueList[i];
    String key = "cue" + String(i);

    cuePrefs.putUChar((key + "_type").c_str(), c.type);
    cuePrefs.putString((key + "_osc").c_str(), c.oscCommand);
    cuePrefs.putString((key + "_id").c_str(), c.cueID);
    cuePrefs.putBool((key + "_on").c_str(), c.enabled);
  }

  cuePrefs.end();
}

bool addCue(const Cue& cue) {
  if (cueCount >= MAX_CUES) return false;
  cueList[cueCount] = cue;
  cueList[cueCount].index = cueCount;
  cueCount++;
  saveCues();
  return true;
}

bool editCue(uint8_t index, const Cue& updated) {
  if (index >= cueCount) return false;
  cueList[index] = updated;
  cueList[index].index = index;
  saveCues();
  return true;
}

bool deleteCue(uint8_t index) {
  if (index >= cueCount) return false;
  for (int i = index; i < cueCount - 1; ++i) {
    cueList[i] = cueList[i + 1];
    cueList[i].index = i;
  }
  cueCount--;
  saveCues();
  return true;
}

void reorderCues(uint8_t fromIndex, uint8_t toIndex) {
  if (fromIndex >= cueCount || toIndex >= cueCount) return;
  Cue moving = cueList[fromIndex];

  if (fromIndex < toIndex) {
    for (int i = fromIndex; i < toIndex; ++i) {
      cueList[i] = cueList[i + 1];
      cueList[i].index = i;
    }
  } else {
    for (int i = fromIndex; i > toIndex; --i) {
      cueList[i] = cueList[i - 1];
      cueList[i].index = i;
    }
  }

  cueList[toIndex] = moving;
  cueList[toIndex].index = toIndex;
  saveCues();
}

Cue* getCue(uint8_t index) {
  if (index >= cueCount) return nullptr;
  return &cueList[index];
}

int getCueCount() {
  return cueCount;
}

void resetCues() {
  cueCount = 0;
  cuePrefs.begin(CUE_NAMESPACE, false);
  cuePrefs.clear();
  cuePrefs.end();
}

} // namespace osc_controller
