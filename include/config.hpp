#pragma once

struct config_t {
    double power = 1.0;
    bool enabled = false;
};

extern config_t config;

void SaveConfig();
bool LoadConfig();