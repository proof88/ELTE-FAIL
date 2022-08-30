#pragma once

/*
    ###################################################################################
    ElteFailPacket.h
    Network packets defined for ELTE-FAIL.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

#include "../../../PGE/PGE/Network/PgePacket.h"

enum class VerticalDirection : std::uint8_t
{
    NONE = 0,
    UP,
    DOWN
};

enum class HorizontalDirection : std::uint8_t
{
    NONE = 0,
    LEFT,
    RIGHT
};

// clients -> server
// PgePktUserCmd messages are sent from clients to server, so server will do sg and then update all the clients with PgePktUserUpdate
struct PgePktUserCmdMove
{
    static const uint32_t id = 3;
    HorizontalDirection horDirection;
    VerticalDirection verDirection;
};

// server -> clients
struct PgePktUserUpdate
{
    static const uint32_t id = 4;
    char szUserName[64];
    TXYZ pos;
};