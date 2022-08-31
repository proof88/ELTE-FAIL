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

namespace ElteFailMsg
{

    enum class ElteFailMsgId : pge_network::TPgeMsgAppMsgId  /* underlying type should be same as type of PgeMsgApp::msgId */
    {
        USER_CMD_MOVE = 0,
        USER_UPDATE
    };

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
    // MsgUserCmdMove messages are sent from clients to server, so server will do sg and then update all the clients with MsgUserUpdate
    struct MsgUserCmdMove
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_CMD_MOVE;

        HorizontalDirection horDirection;
        VerticalDirection verDirection;
    };

    // server -> clients
    struct MsgUserUpdate
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_UPDATE;

        char szUserName[64];
        TXYZ pos;
    };

} // namespace ElteFailMsg