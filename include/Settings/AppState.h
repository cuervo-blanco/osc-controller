#pragma once

namespace osc_controller::settings {

constexpr int ENCODER_SW = 5;
constexpr int BACK_BUTTON = 13;
constexpr int FIRE_BUTTON = 4;
constexpr int THIRD_BUTTON = 14;
constexpr int ROTARY_CLK = 18;
constexpr int ROTARY_DT = 19;
constexpr int SDA_PIN = 21;
constexpr int SCL_PIN = 22;

enum MenuState {
  MAIN_MENU,

  NETWORK_MENU,
  CUES_MENU,
  RUN_SHOW_MENU,
  SETTINGS_MENU,

  ENTER_PORT,
  ENTER_PASSCODE,
  ENTER_QLAB_IP,
  SHOW_INFO,

  ADD_CUE_MENU,
  EDIT_CUE_MENU,
  DELETE_CUE_MENU,
  CUE_CONTROL_MENU,
  REORDER_CUE_MENU
};
} // namespace Settings

