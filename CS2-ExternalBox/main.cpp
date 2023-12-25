#include <iostream>
#include <Windows.h>
#include <thread>
#include "Memory.h"
#include "ExWindow.h"
#include "Offsets.h"

Offsets offsets;
Config config;

void Init() {
    mem.Setup();
    mem.GetMoudles();
}

int main() {
    Init();
    CreateExternalWindow();
}
