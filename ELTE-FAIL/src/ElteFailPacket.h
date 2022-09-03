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
        USER_SETUP = 0,
        USER_CMD_MOVE,
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

    // server -> clients
    struct MsgUserSetup
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_SETUP;
        static const uint8_t nUserNameLength = 64;
        static const uint8_t nTrollfaceTexLength = 64;

        bool bCurrentClient;
        char szUserName[nUserNameLength];
        char szTrollfaceTex[nTrollfaceTexLength];
    };

    // clients -> server
    // MsgUserCmdMove messages are sent from clients to server, so server will do sg and then update all the clients with MsgUserUpdate
    struct MsgUserCmdMove
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_CMD_MOVE;

        HorizontalDirection horDirection;
        VerticalDirection verDirection;
    };

    // server -> self and clients
    struct MsgUserUpdate
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_UPDATE;

        char szUserName[MsgUserSetup::nUserNameLength];
        TXYZ pos;
    };

} // namespace ElteFailMsg