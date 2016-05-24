#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <mutex>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

const std::string g_assetsFolder = "assets/";
const std::string g_greenColor = "Green";
const std::string g_blueColor = "Blue";
const std::string g_redColor = "Red";
const std::string g_yellowColor = "Yellow";

const char PLAYER_DOWN_NUMBER = 0;
const char PLAYER_UP_NUMBER = 1;
const char PLAYER_LEFT_NUMBER = 2;
const char PLAYER_RIGHT_NUMBER = 3;

enum PROGRAM_STATE {PROGRAM_EXIT, PROGRAM_SERVER, PROGRAM_CLIENT};
enum GAME_STATE {GAME_LOBBY, GAME_PLAY};

const int DEFAULT_SERVER_PORT_TCP_IN = 27015;
const int DEFAULT_SERVER_PORT_UDP_IN = 27019;
const int DEFAULT_SERVER_PORT_TCP_OUT = 27017;
const int DEFAULT_SERVER_PORT_UDP_OUT = 27018;
const int BUFFER_SIZE = 64;
const int MAX_PLAYERS = 4;

const SDL_Color whiteColorSDL = {127, 127, 127, 0 };
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
extern std::string charToOneCharString(const char& arg);
extern std::string intToStr(const int& arg);



#include "collections/ThreadSafeStack.hpp"

#include "exceptions/NetworkException.hpp"

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
#include "gui/PlayersTextBox.hpp"

#include "network/GameContext.hpp"

class NetworkMessageHandler;

extern std::unordered_map<char, NetworkMessageHandler*> messagesHandlers;

#include "network/messages/messages_ids.hpp"
#include "network/messages/NetworkMessageHandler.hpp"
#include "network/messages/MessageReceiverSender.hpp"

class ServerNetworkTask;
class ClientNetworkTask;

#include "network/Server.hpp"
#include "network/Client.hpp"

#include "network/ServerNetworkTask.hpp"
#include "network/ClientNetworkTask.hpp"




