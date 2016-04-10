#pragma once

#include <SDL.h>
#include <stdio.h>
#include <string>
#include <vector>

const std::string g_assetsFolder = "assets/";
const std::string g_greenColor = "Green";

const unsigned SCREEN_FPS = 60;
const unsigned SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

class GameObject;

#include "video/Timer.hpp"
#include "video/Renderer.hpp"
#include "video/ImageLoader.hpp"
#include "video/SDLSetup.hpp"

#include "objects/GameObject.hpp"
#include "objects/Paddle.hpp"
#include "objects/Ball.hpp"

