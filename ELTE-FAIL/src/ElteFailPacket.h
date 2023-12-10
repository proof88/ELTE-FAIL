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

#include <array>

namespace elte_fail
{

    enum class ElteFailMsgId : pge_network::MsgApp::TMsgId  /* underlying type should be same as type of MsgAppArea::msgId */
    {
        UserSetupFromServer = 0,
        UserCmdMoveFromClient,
        UserUpdateFromServer,
        LastMsgId
    };

    struct ElteFailMsgId2ZStringPair
    {
        ElteFailMsgId msgId;
        const char* const zstring;
    };

    // this way of defining std::array makes sure code cannot compile if we forget to align the array after changing ElteFailMsgId
    constexpr std::array<ElteFailMsgId2ZStringPair, static_cast<size_t>(ElteFailMsgId::LastMsgId)> MapMsgAppId2String
    { {
         {ElteFailMsgId::UserSetupFromServer,    "MsgUserSetupFromServer"},
         {ElteFailMsgId::UserCmdMoveFromClient,  "MsgUserCmdFromClient"},
         {ElteFailMsgId::UserUpdateFromServer,   "MsgUserUpdateFromServer"}
    } };

    // server -> self (inject) and clients
    struct MsgUserSetupFromServer
    {
        static const ElteFailMsgId id = ElteFailMsgId::UserSetupFromServer;
        static const uint8_t nUserNameBufferLength = 64;
        static const uint8_t nTrollfaceTexMaxLength = 64;

        static bool initPkt(
            pge_network::PgePacket& pkt,
            const pge_network::PgeNetworkConnectionHandle& connHandleServerSide,
            bool bCurrentClient,
            const std::string& sUserName,
            const std::string& sTrollFaceTex,
            const std::string& sIpAddress)
        {
            // although preparePktMsgAppFill() does runtime check, we should fail already at compile-time if msg is too big!
            static_assert(sizeof(MsgUserSetupFromServer) <= pge_network::MsgAppArea::nMaxMessagesAreaLengthBytes, "msg size");

            pge_network::PgePacket::initPktMsgApp(pkt, connHandleServerSide);

            pge_network::TByte* const pMsgAppData = pge_network::PgePacket::preparePktMsgAppFill(
                pkt, static_cast<pge_network::MsgApp::TMsgId>(id), sizeof(MsgUserSetupFromServer));
            if (!pMsgAppData)
            {
                return false;
            }

            elte_fail::MsgUserSetupFromServer& msgUserSetup = reinterpret_cast<elte_fail::MsgUserSetupFromServer&>(*pMsgAppData);
            msgUserSetup.m_bCurrentClient = bCurrentClient;
            strncpy_s(msgUserSetup.m_szUserName, nUserNameBufferLength, sUserName.c_str(), sUserName.length());
            strncpy_s(msgUserSetup.m_szTrollfaceTex, nTrollfaceTexMaxLength, sTrollFaceTex.c_str(), sTrollFaceTex.length());
            strncpy_s(msgUserSetup.m_szIpAddress, sizeof(msgUserSetup.m_szIpAddress), sIpAddress.c_str(), sIpAddress.length());

            return true;
        }

        bool m_bCurrentClient;
        char m_szUserName[nUserNameBufferLength];
        char m_szTrollfaceTex[nTrollfaceTexMaxLength];
        char m_szIpAddress[pge_network::MsgUserConnectedServerSelf::nIpAddressMaxLength];
    };
    static_assert(std::is_trivial_v<MsgUserSetupFromServer>);
    static_assert(std::is_trivially_copyable_v<MsgUserSetupFromServer>);
    static_assert(std::is_standard_layout_v<MsgUserSetupFromServer>);

    enum class VerticalDirection : uint8_t
    {
        NONE = 0,
        UP,
        DOWN
    };

    enum class HorizontalDirection : uint8_t
    {
        NONE = 0,
        LEFT,
        RIGHT
    };

    // clients -> server
    // MsgUserCmdMoveFromClient messages are sent from clients to server, so server will do sg and then update all the clients with MsgUserUpdateFromServer
    struct MsgUserCmdMoveFromClient
    {
        static const ElteFailMsgId id = ElteFailMsgId::UserCmdMoveFromClient;

        static bool initPkt(
            pge_network::PgePacket& pkt,
            const HorizontalDirection& dirHorizontal,
            const VerticalDirection& dirVertical)
        {
            // although preparePktMsgAppFill() does runtime check, we should fail already at compile-time if msg is too big!
            static_assert(sizeof(MsgUserCmdMoveFromClient) <= pge_network::MsgAppArea::nMaxMessagesAreaLengthBytes, "msg size");

            pge_network::PgePacket::initPktMsgApp(pkt, 0 /* m_connHandleServerSide is ignored in this message */);

            pge_network::TByte* const pMsgAppData = pge_network::PgePacket::preparePktMsgAppFill(
                pkt, static_cast<pge_network::MsgApp::TMsgId>(id), sizeof(MsgUserCmdMoveFromClient));
            if (!pMsgAppData)
            {
                return false;
            }

            elte_fail::MsgUserCmdMoveFromClient& msgUserCmdMove = reinterpret_cast<elte_fail::MsgUserCmdMoveFromClient&>(*pMsgAppData);
            msgUserCmdMove.m_dirHorizontal = dirHorizontal;
            msgUserCmdMove.m_dirVertical = dirVertical;

            return true;
        }

        HorizontalDirection m_dirHorizontal;
        VerticalDirection m_dirVertical;
    };
    static_assert(std::is_trivial_v<MsgUserCmdMoveFromClient>);
    static_assert(std::is_trivially_copyable_v<MsgUserCmdMoveFromClient>);
    static_assert(std::is_standard_layout_v<MsgUserCmdMoveFromClient>);

    // server -> self (inject) and clients
    struct MsgUserUpdateFromServer
    {
        static const ElteFailMsgId id = ElteFailMsgId::UserUpdateFromServer;

        static bool initPkt(
            pge_network::PgePacket& pkt,
            const pge_network::PgeNetworkConnectionHandle& connHandleServerSide,
            const TPureFloat x,
            const TPureFloat y, 
            const TPureFloat z)
        {
            // although preparePktMsgAppFill() does runtime check, we should fail already at compile-time if msg is too big!
            static_assert(sizeof(MsgUserUpdateFromServer) <= pge_network::MsgAppArea::nMaxMessagesAreaLengthBytes, "msg size");
            
            pge_network::PgePacket::initPktMsgApp(pkt, connHandleServerSide);

            pge_network::TByte* const pMsgAppData = pge_network::PgePacket::preparePktMsgAppFill(
                pkt, static_cast<pge_network::MsgApp::TMsgId>(id), sizeof(MsgUserUpdateFromServer));
            if (!pMsgAppData)
            {
                return false;
            }

            elte_fail::MsgUserUpdateFromServer& msgUserCmdUpdate = reinterpret_cast<elte_fail::MsgUserUpdateFromServer&>(*pMsgAppData);
            msgUserCmdUpdate.m_pos.x = x;
            msgUserCmdUpdate.m_pos.y = y;
            msgUserCmdUpdate.m_pos.z = z;

            return true;
        }

        TXYZ m_pos;  // Z-coord is actually unused because it never gets changed during the whole gameplay ...
    };
    static_assert(std::is_trivial_v<MsgUserUpdateFromServer>);
    static_assert(std::is_trivially_copyable_v<MsgUserUpdateFromServer>);
    static_assert(std::is_standard_layout_v<MsgUserUpdateFromServer>);

} // namespace elte_fail