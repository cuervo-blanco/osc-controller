#include <Preferences.h>
#include "Settings/LanguageManager.h"

namespace osc_controller::settings {
static Language currentLang = LANG_EN;
Preferences langPrefs;

void setLanguage(Language lang) {
  currentLang = lang;
  saveLanguageSetting();
}

Language getLanguage() {
  return currentLang;
}

void saveLanguageSetting() {
  langPrefs.begin("lang", false);
  langPrefs.putUChar("lang", (uint8_t)currentLang);
  langPrefs.end();
}

void loadLanguageSetting() {
  langPrefs.begin("lang", true);
  currentLang = (Language)langPrefs.getUChar("lang", LANG_EN);
  langPrefs.end();
}

const char* t(const char* key) {
  if (currentLang == LANG_EN) {
    if (strcmp(key, "menu_network") == 0) return "Network";
    if (strcmp(key, "menu_cues") == 0) return "Cues";
    if (strcmp(key, "menu_run_show") == 0) return "Show Mode";
    if (strcmp(key, "menu_settings") == 0) return "Settings";
    if (strcmp(key, "update_done") == 0) return "Updated!";
    if (strcmp(key, "no_wifi") == 0) return "No WiFi!";
    if (strcmp(key, "pinging") == 0) return "Pinging...";
    if (strcmp(key, "invalid_ip") == 0) return "Invalid IP";
    if (strcmp(key, "invalid_cue") == 0) return "Invalid cue.";
    if (strcmp(key, "fail") == 0) return "Fail";
    if (strcmp(key, "success") == 0) return "Success: ";
    if (strcmp(key, "scan_item") == 0) return "Scan";
    if (strcmp(key, "port_item") == 0) return "Port";
    if (strcmp(key, "passcode_item") == 0) return "Passcode";
    if (strcmp(key, "qlab_ip_item") == 0) return "Qlab IP";
    if (strcmp(key, "info_item") == 0) return "Info";
    if (strcmp(key, "reset_item") == 0) return "Reset";
    if (strcmp(key, "test_item") == 0) return "Test";
    if (strcmp(key, "reset_notify") == 0) return "Resetting...";
    if (strcmp(key, "reset_done_notify") == 0) return "Reset Done.";
    if (strcmp(key, "done_notify") == 0) return "Done.";
    if (strcmp(key, "add_cue_item") == 0) return "Add Cue";
    if (strcmp(key, "edit_cue_item") == 0) return "Edit Cue";
    if (strcmp(key, "delete_cue_item") == 0) return "Delete Cue";
    if (strcmp(key, "preview_item") == 0) return "Preview";
    if (strcmp(key, "reorder_cues_item") == 0) return "Reorder Cues";
    if (strcmp(key, "start_cue_item") == 0) return "Start Cue";
    if (strcmp(key, "stop_cue_item") == 0) return "Stop Cue";
    if (strcmp(key, "pause_cue_item") == 0) return "Pause Cue";
    if (strcmp(key, "load_cue_item") == 0) return "Load Cue";
    if (strcmp(key, "custom_path_item") == 0) return "Custom Cue";
    if (strcmp(key, "select_type") == 0) return "Select type";
    if (strcmp(key, "type") == 0) return "Type";
    if (strcmp(key, "enter_osc_path") == 0) return "Enter OSC Path:";
    if (strcmp(key, "enter_cue_id") == 0) return "Enter Cue ID:";
    if (strcmp(key, "cue_added") == 0) return "Cue added.";
    if (strcmp(key, "add_failed") == 0) return "Add failed.";
    if (strcmp(key, "no_cues_edit") == 0) return "No cues to edit.";
    if (strcmp(key, "edit_cue_label") == 0) return "Edit Cue #";
    if (strcmp(key, "updated_label") == 0) return "Updated.";
    if (strcmp(key, "cancelled_label") == 0) return "Cancelled.";
    if (strcmp(key, "no_cues_del") == 0) return "No cues to del.";
    if (strcmp(key, "delete_all") == 0) return "!! Delete All !!";
    if (strcmp(key, "press_confirm") == 0) return "Press to confirm";
    if (strcmp(key, "all_cues_del_label") == 0) return "All cues deleted";
    if (strcmp(key, "del_cue_label") == 0) return "Del Cue #";
    if (strcmp(key, "deleted_label") == 0) return "Deleted Cue #";
    if (strcmp(key, "delete_failed") == 0) return "Delete failed.";
    if (strcmp(key, "not_enough_cues") == 0) return "Not enough cues";
    if (strcmp(key, "move_cue_label") == 0) return "Move Cue #";
    if (strcmp(key, "to_position") == 0) return "To position: ";
    if (strcmp(key, "reordered_label") == 0) return "Reordered.";
    if (strcmp(key, "fired_label") == 0) return "Fired:";
    if (strcmp(key, "fired_cue_label") == 0) return "Fired Cue #";
    if (strcmp(key, "no_cues_label") == 0) return "No cues.";
    if (strcmp(key, "action_none") == 0) return "None";
    if (strcmp(key, "action_panic_all") == 0) return "Panic All";
    if (strcmp(key, "action_custom_osc") == 0) return "Custom OSC";
    if (strcmp(key, "action_goto_cue") == 0) return "Go to Cue";
    if (strcmp(key, "action_stop_all") == 0) return "Stop All";
    if (strcmp(key, "action_resume_all") == 0) return "Resume All";
    if (strcmp(key, "action_go") == 0) return "Go";
    if (strcmp(key, "action_panic") == 0) return "Panic";
    if (strcmp(key, "action_save") == 0) return "Save";
    if (strcmp(key, "select_action") == 0) return "Action:";
    if (strcmp(key, "checking_update") == 0) return "Checking update...";
    if (strcmp(key, "saved_notify") == 0) return "Saved.";
    if (strcmp(key, "update_failed") == 0) return "Update Failed";
    if (strcmp(key, "no_updates") == 0) return "No Updates.";
    if (strcmp(key, "scroll_lock_item") == 0) return "Scroll Lock";
    if (strcmp(key, "scroll_locked") == 0) return "Scroll Locked";
    if (strcmp(key, "show_complete") == 0) return "Show Complete!";
    if (strcmp(key, "press_back") == 0) return "Press Back!";
    if (strcmp(key, "third_button_item") == 0) return "Third Button";
    if (strcmp(key, "workspace_manager_item") == 0) return "Workspaces";
    if (strcmp(key, "network_item") == 0) return "Network";
    if (strcmp(key, "factory_reset_item") == 0) return "Factory Reset";
    if (strcmp(key, "device_info_item") == 0) return "Device Info";
    if (strcmp(key, "language_item") == 0) return "Language";
    if (strcmp(key, "update_item") == 0) return "Update Software";
    if (strcmp(key, "enabled") == 0) return "Enabled";
    if (strcmp(key, "disabled") == 0) return "Disabled";
    if (strcmp(key, "workspace_id_prompt") == 0) return "Workspace ID:";
    if (strcmp(key, "confirm_reset") == 0) return "Confirm Reset?";
    if (strcmp(key, "click_yes") == 0) return "Click = Yes";
    if (strcmp(key, "cancelled") == 0) return "Cancelled";
    if (strcmp(key, "connecting") == 0) return "Connecting...";
    if (strcmp(key, "connected") == 0) return "Connected";
    if (strcmp(key, "connection_failed") == 0) return "Connection failed.";
    if (strcmp(key, "try_again") == 0) return "Try again.";
    if (strcmp(key, "language_label") == 0) return "Idioma/Language";
    if (strcmp(key, "version_label") == 0) return "Version:";
    if (strcmp(key, "workspace_id_label") == 0) return "Workspace ID:";
    if (strcmp(key, "qlab_ip_label") == 0) return "QLab IP:";
    if (strcmp(key, "port_label") == 0) return "Port:";
    if (strcmp(key, "scroll_label") == 0) return "Scroll:";
    if (strcmp(key, "third_btn_action_label") == 0) return "3rd Btn Action:";
    if (strcmp(key, "no_workspace_id") == 0) return "No workspace ID";
    if (strcmp(key, "no_workspaces") == 0) return "No workspaces";
    if (strcmp(key, "no_cues_to_run") == 0) return "No cues to run";
    if (strcmp(key, "unset") == 0) return "<unset>";
    if (strcmp(key, "none") == 0) return "<none>";
    if (strcmp(key, "OFF") == 0) return "OFF";
    if (strcmp(key, "ON") == 0) return "ON";
    if (strcmp(key, "scanning_wifi") == 0) return "Scanning WiFi...";
    if (strcmp(key, "no_networks") == 0) return "No networks!";
    if (strcmp(key, "scan_failed") == 0) return "Scan Failed";
    if (strcmp(key, "ssid_error") == 0) return "SSID Error.";
    if (strcmp(key, "enter_password") == 0) return "Enter password:";
    if (strcmp(key, "select_network") == 0) return "Select Network:";
    if (strcmp(key, "custom_item") == 0) return "Custom";
    if (strcmp(key, "select_path_type") == 0) return "Select Path Type";
    if (strcmp(key, "add_workspace_item") == 0) return "Add Workspace";
    if (strcmp(key, "edit_workspace_item") == 0) return "Edit Workspace";
    if (strcmp(key, "delete_workspace_item") == 0) return "Delete Workspace";
    if (strcmp(key, "set_primary_workspace_item") == 0) return "Set Primary";
    if (strcmp(key, "enter_workspace_name") == 0) return "Enter Workspace name";
    if (strcmp(key, "workspace_added") == 0) return "Workspace added.";
    if (strcmp(key, "select_primary") == 0) return "Set primary:";
    if (strcmp(key, "primary_set") == 0) return "Primary Selected";

  }

  if (currentLang == LANG_ES) {
    if (strcmp(key, "menu_network") == 0) return "Red";
    if (strcmp(key, "menu_cues") == 0) return "Cues";
    if (strcmp(key, "menu_run_show") == 0) return "Modo Show";
    if (strcmp(key, "menu_settings") == 0) return "Ajustes";
    if (strcmp(key, "update_done") == 0) return "Actualizado!";
    if (strcmp(key, "no_wifi") == 0) return "Sin WiFi!";
    if (strcmp(key, "pinging") == 0) return "Testeando...";
    if (strcmp(key, "invalid_ip") == 0) return "IP Invalido";
    if (strcmp(key, "invalid_cue") == 0) return "Cue Invalido";
    if (strcmp(key, "fail") == 0) return "Fallido";
    if (strcmp(key, "success") == 0) return "Exitoso: ";
    if (strcmp(key, "scan_item") == 0) return "Scanear";
    if (strcmp(key, "port_item") == 0) return "Puerto";
    if (strcmp(key, "passcode_item") == 0) return "Codigo";
    if (strcmp(key, "qlab_ip_item") == 0) return "Qlab IP";
    if (strcmp(key, "info_item") == 0) return "Info";
    if (strcmp(key, "reset_item") == 0) return "Resetear";
    if (strcmp(key, "test_item") == 0) return "Prueba";
    if (strcmp(key, "reset_notify") == 0) return "Reseteando...";
    if (strcmp(key, "reset_done_notify") == 0) return "Reseteo Listo";
    if (strcmp(key, "done_notify") == 0) return "Terminado";
    if (strcmp(key, "add_cue_item") == 0) return "Anadir Cue";
    if (strcmp(key, "edit_cue_item") == 0) return "Editar Cue";
    if (strcmp(key, "delete_cue_item") == 0) return "Eliminar Cue";
    if (strcmp(key, "preview_item") == 0) return "Vista Previa";
    if (strcmp(key, "reorder_cues_item") == 0) return "Reordenar Cues";
    if (strcmp(key, "start_cue_item") == 0) return "Empezar Cue";
    if (strcmp(key, "stop_cue_item") == 0) return "Detener Cue";
    if (strcmp(key, "pause_cue_item") == 0) return "Pausar Cue";
    if (strcmp(key, "load_cue_item") == 0) return "Cargar Cue";
    if (strcmp(key, "custom_path_item") == 0) return "Personalizar";
    if (strcmp(key, "select_type") == 0) return "Escoger tipo";
    if (strcmp(key, "type") == 0) return "Tipo";
    if (strcmp(key, "enter_osc_path") == 0) return "Ingresar OSC:";
    if (strcmp(key, "enter_cue_id") == 0) return "Ingresar ID Cue:";
    if (strcmp(key, "cue_added") == 0) return "Cue anadida.";
    if (strcmp(key, "add_failed") == 0) return "Fallo al anadir.";
    if (strcmp(key, "no_cues_edit") == 0) return "Sin cues a editar.";
    if (strcmp(key, "edit_cue_label") == 0) return "Editar Cue #";
    if (strcmp(key, "updated_label") == 0) return "Actualizado.";
    if (strcmp(key, "cancelled_label") == 0) return "Cancelado.";
    if (strcmp(key, "no_cues_del") == 0) return "Sin cues a borrar.";
    if (strcmp(key, "delete_all") == 0) return "!! Borrar Todo !!";
    if (strcmp(key, "press_confirm") == 0) return "Presione para confirmar";
    if (strcmp(key, "all_cues_del_label") == 0) return "Todos cues borrados";
    if (strcmp(key, "del_cue_label") == 0) return "Borrar Cue #";
    if (strcmp(key, "deleted_label") == 0) return "Cue borrada #";
    if (strcmp(key, "delete_failed") == 0) return "Fallo al borrar.";
    if (strcmp(key, "not_enough_cues") == 0) return "No hay suficientes cues";
    if (strcmp(key, "move_cue_label") == 0) return "Mover Cue #";
    if (strcmp(key, "to_position") == 0) return "A posicion: ";
    if (strcmp(key, "reordered_label") == 0) return "Reordenado.";
    if (strcmp(key, "fired_label") == 0) return "Ejecutado:";
    if (strcmp(key, "fired_cue_label") == 0) return "Ejecutado Cue #";
    if (strcmp(key, "no_cues_label") == 0) return "Sin cues.";
    if (strcmp(key, "action_none") == 0) return "Ninguno";
    if (strcmp(key, "action_panic_all") == 0) return "Panico Total";
    if (strcmp(key, "action_custom_osc") == 0) return "OSC Personal.";
    if (strcmp(key, "action_goto_cue") == 0) return "Ir a Cue";
    if (strcmp(key, "action_stop_all") == 0) return "Detener Todo";
    if (strcmp(key, "action_resume_all") == 0) return "Reanudar Todo";
    if (strcmp(key, "action_go") == 0) return "Ir";
    if (strcmp(key, "action_panic") == 0) return "Panico";
    if (strcmp(key, "action_save") == 0) return "Guardar";
    if (strcmp(key, "select_action") == 0) return "Accion:"; 
    if (strcmp(key, "checking_update") == 0) return "Buscando actualiz..."; 
    if (strcmp(key, "saved_notify") == 0) return "Guardado.";
    if (strcmp(key, "update_failed") == 0) return "Fallido.";
    if (strcmp(key, "no_updates") == 0) return "No Actualizacion";
    if (strcmp(key, "scroll_lock_item") == 0) return "Scroll Lock";
    if (strcmp(key, "scroll_locked") == 0) return "Scroll Locked";
    if (strcmp(key, "show_complete") == 0) return "Show Completado!";
    if (strcmp(key, "press_back") == 0) return "Pressionar Atras!";
    if (strcmp(key, "third_button_item") == 0) return "3er Boton";
    if (strcmp(key, "workspace_manager_item") == 0) return "Proyectos";
    if (strcmp(key, "network_item") == 0) return "Red";
    if (strcmp(key, "factory_reset_item") == 0) return "Resetear";
    if (strcmp(key, "device_info_item") == 0) return "Info Dispositivo";
    if (strcmp(key, "language_item") == 0) return "Idioma";
    if (strcmp(key, "update_item") == 0) return "Actualizar";
    if (strcmp(key, "enabled") == 0) return "Activado";
    if (strcmp(key, "disabled") == 0) return "Desactivado";
    if (strcmp(key, "workspace_id_prompt") == 0) return "Workspace ID:";
    if (strcmp(key, "confirm_reset") == 0) return "¿Confirmar Reset?";
    if (strcmp(key, "click_yes") == 0) return "Click = Si";
    if (strcmp(key, "cancelled") == 0) return "Cancelado";
    if (strcmp(key, "connecting") == 0) return "Conectando...";
    if (strcmp(key, "connected") == 0) return "Conectado!";
    if (strcmp(key, "connection_failed") == 0) return "Conexion fallada.";
    if (strcmp(key, "try_again") == 0) return "Vuelve a intentar.";
    if (strcmp(key, "language_label") == 0) return "Idioma/Language";
    if (strcmp(key, "version_label") == 0) return "Version:";
    if (strcmp(key, "workspace_id_label") == 0) return "ID Workspace:";
    if (strcmp(key, "qlab_ip_label") == 0) return "IP QLab:";
    if (strcmp(key, "port_label") == 0) return "Puerto:";
    if (strcmp(key, "scroll_label") == 0) return "Scroll:";
    if (strcmp(key, "third_btn_action_label") == 0) return "Accion boton 3:";
    if (strcmp(key, "no_workspace_id") == 0) return "Sin ID espacio";
    if (strcmp(key, "no_cues_to_run") == 0) return "No hay cues";
    if (strcmp(key, "unset") == 0) return "<no_definido>";
    if (strcmp(key, "none") == 0) return "<no_definido>";
    if (strcmp(key, "OFF") == 0) return "APAGADO";
    if (strcmp(key, "ON") == 0) return "ENCENDIDO";
    if (strcmp(key, "scanning_wifi") == 0) return "Escaneando Wifi...";
    if (strcmp(key, "no_networks") == 0) return "No hay redes!";
    if (strcmp(key, "scan_failed") == 0) return "Escaneo fallido";
    if (strcmp(key, "ssid_error") == 0) return "SSID Error.";
    if (strcmp(key, "enter_password") == 0) return "Ingresar contrasena";
    if (strcmp(key, "select_network") == 0) return "Escoger Red:";
    if (strcmp(key, "custom_item") == 0) return "Personalizado";
    if (strcmp(key, "select_path_type") == 0) return "Escoger tipo:";
    if (strcmp(key, "add_workspace_item") == 0) return "Anadir Workspace";
    if (strcmp(key, "edit_workspace_item") == 0) return "Editar Workspace";
    if (strcmp(key, "delete_workspace_item") == 0) return "Borrar Workspace";
    if (strcmp(key, "set_primary_workspace_item") == 0) return "Escoger Primario";
    if (strcmp(key, "no_workspaces") == 0) return "No hay workspaces";
    if (strcmp(key, "enter_workspace_name") == 0) return "Escribir nombre";
    if (strcmp(key, "workspace_added") == 0) return "Workspace anadido";
    if (strcmp(key, "select_primary") == 0) return "Escoger primario";
    if (strcmp(key, "primary_set") == 0) return "Primario seleccionado";
  }

  return key;
}

} // namespace osc_controller::settings
