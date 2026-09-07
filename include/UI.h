/**
 * @file UI.h
 * @brief User interface header
 */

#pragma once

#include "EvilTwin.h"
#include <vector>
#include <string>

// ============================================
// Core UI Functions
// ============================================

void UI_Init();
void UI_Cleanup();

void UI_ClearScreen();
void UI_DisplayHeader();
void UI_DisplayMenu();
void UI_DisplayNetworks();
void UI_DisplayStatus();
void UI_DisplayCredentials();
void UI_DisplayHelp();
void UI_DisplaySettings();

char UI_GetChoice();
int UI_GetNetworkSelection();
bool UI_GetConfirmation(const std::string& prompt);
std::string UI_GetInput(const std::string& prompt);
int UI_GetIntInput(const std::string& prompt);

void UI_DisplayMessage(const std::string& msg);
void UI_DisplayError(const std::string& error);
void UI_DisplaySuccess(const std::string& msg);
void UI_DisplayInfo(const std::string& info);

// ============================================
// Update Functions
// ============================================

void UI_UpdateNetworks(const std::vector<WiFiNetwork>& networks);
void UI_UpdateStatus(const AttackStatus& status);
void UI_AddCredential(const Credential& cred);
void UI_ClearCredentials();
void UI_UpdateConfig(const AppConfig& config);

// ============================================
// Getters/Setters
// ============================================

bool UI_IsRunning();
void UI_SetRunning(bool running);

bool UI_GetShowCredentials();
void UI_SetShowCredentials(bool show);

int UI_GetSelectedNetwork();
void UI_SetSelectedNetwork(int idx);

std::vector<WiFiNetwork> UI_GetNetworks();
std::vector<Credential> UI_GetCredentials();
AppConfig UI_GetConfig();

// ============================================
// Export Functions
// ============================================

bool UI_ExportCredentials(const std::string& filename = "");

// ============================================
// Progress/Spinner
// ============================================

void UI_ShowProgress(const std::string& label, int current, int total);
void UI_ShowSpinner(const std::string& label);
void UI_ClearProgress();

// ============================================
// Color Functions
// ============================================

void UI_SetColorRed();
void UI_SetColorGreen();
void UI_SetColorYellow();
void UI_SetColorCyan();
void UI_SetColorWhite();

// ============================================
// Redraw State
// ============================================

bool UI_NeedsRedraw();
void UI_ResetRedraw();

// ============================================
// Main UI Loop
// ============================================

void UI_RunLoop();
