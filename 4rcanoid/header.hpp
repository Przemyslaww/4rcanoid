#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

const std::string g_assetsFolder = "assets/";
const std::string g_greenColor = "Green";
const std::string g_blueColor = "Blue";
const std::string g_redColor = "Red";
const std::string g_yellowColor = "Yellow";

enum PROGRAM_STATE {PROGRAM_EXIT, PROGRAM_SERVER, PROGRAM_CLIENT};

const SDL_Color whiteColorSDL = {255, 255, 255, 0 };
const SDL_Color blackColorSDL = {0,0,0,0};

const unsigned SCREEN_FPS = 60;
const unsigned SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

const unsigned SCREEN_WIDTH = 700;
const unsigned SCREEN_HEIGHT = 700;

const unsigned paddleWidth = 100;
const unsigned paddleHeight = 20;

const double NINETY_DEGREES = M_PI * 0.5;

const double ballSpeed = 6.0;
const int paddleSpeed = 6;

const unsigned wallWidth = 100;
const unsigned paddleBoundary = SCREEN_WIDTH / 2 - paddleWidth / 2 - wallWidth;



class GameObject;
class Block;

extern std::vector<std::string> split(const std::string& astr, char c);


#include "video/Timer.hpp"
#include "video/Renderer.hpp"
#include "video/ImageLoader.hpp"
#include "video/SDLSetup.hpp"

#include "objects/GameObject.hpp"
#include "objects/Paddle.hpp"
#include "objects/Block.hpp"
#include "objects/BlocksGrid.hpp"
#include "objects/Ball.hpp"

#include "gui/TextInputDialog.hpp"
#include "gui/TextBox.hpp"

#include "network/NetworkConnection.hpp"


