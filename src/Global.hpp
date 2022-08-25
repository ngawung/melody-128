#pragma once

#include <raylib.h>

static const Vector2 FONT_SIZE = {9.0f, 16.0f};
static const Vector2 SCREEN_SIZE = {72.0f, 37.0f};

Vector2 GetTermToScreen(Vector2 pos);
Vector2 GetScreenToTerm(Vector2 pos);