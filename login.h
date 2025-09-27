#pragma once
#include <Windows.h>

// Shows the login window. Returns true if login + dummy injection succeeded,
// false if the user closed the window or cancelled.
bool ShowLoginWindow();
