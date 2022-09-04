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
        static const uint8_t nUserNameMaxLength = 64;
        static const uint8_t nTrollfaceTexMaxLength = 64;

        static bool initPkt(
            pge_network::PgePacket& pkt,
            const pge_network::PgeNetworkConnectionHandle& connHandleServerSide,
            bool bCurrentClient,
            const std::string& sUserName,
            const std::string& sTrollFaceTex,
            const std::string& sIpAddress)
        {
            memset(&pkt, 0, sizeof(pkt));
            pkt.m_connHandleServerSide = connHandleServerSide;
            pkt.pktId = pge_network::PgePktId::APP;
            pkt.msg.app.msgId = static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserSetup::id);

            ElteFailMsg::MsgUserSetup& msgUserSetup = reinterpret_cast<ElteFailMsg::MsgUserSetup&>(pkt.msg.app.cData);
            msgUserSetup.m_bCurrentClient = bCurrentClient;
            strncpy_s(msgUserSetup.m_szUserName, nUserNameMaxLength, sUserName.c_str(), sUserName.length());
            strncpy_s(msgUserSetup.m_szTrollfaceTex, nTrollfaceTexMaxLength, sTrollFaceTex.c_str(), sTrollFaceTex.length());
            strncpy_s(msgUserSetup.m_szIpAddress, sizeof(msgUserSetup.m_szIpAddress), sIpAddress.c_str(), sIpAddress.length());

            return true;
        }

        bool m_bCurrentClient;
        char m_szUserName[nUserNameMaxLength];
        char m_szTrollfaceTex[nTrollfaceTexMaxLength];
        char m_szIpAddress[pge_network::PgeMsgUserConnected::nIpAddressMaxLength];
    };

    // clients -> server
    // MsgUserCmdMove messages are sent from clients to server, so server will do sg and then update all the clients with MsgUserUpdate
    struct MsgUserCmdMove
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_CMD_MOVE;

        static bool initPkt(
            pge_network::PgePacket& pkt,
            const HorizontalDirection& dirHorizontal,
            const VerticalDirection& dirVertical)
        {
            memset(&pkt, 0, sizeof(pkt));
            // m_connHandleServerSide is ignored in this message
            //pkt.m_connHandleServerSide = connHandleServerSide;
            pkt.pktId = pge_network::PgePktId::APP;
            pkt.msg.app.msgId = static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserCmdMove::id);

            ElteFailMsg::MsgUserCmdMove& msgUserCmdMove = reinterpret_cast<ElteFailMsg::MsgUserCmdMove&>(pkt.msg.app.cData);
            msgUserCmdMove.m_dirHorizontal = dirHorizontal;
            msgUserCmdMove.m_dirVertical = dirVertical;

            return true;
        }

        HorizontalDirection m_dirHorizontal;
        VerticalDirection m_dirVertical;
    };

    // server -> self and clients
    struct MsgUserUpdate
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_UPDATE;

        char m_szUserName[MsgUserSetup::nUserNameMaxLength];
        TXYZ m_pos;
    };

} // namespace ElteFailMsg