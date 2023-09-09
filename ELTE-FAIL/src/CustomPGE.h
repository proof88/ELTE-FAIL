#pragma once

/*
    ###################################################################################
    CustomPGE.h
    Customized PGE.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

// Since PGE includes soloud.h that includes winproof88.h, we need to define affecting macro here because
// later we would be late (for example, we are late in CustomPGE.cpp).
// The only solution for this would be if PGE did NOT include soloud.h but instead it would use a wrapper
// Audio class to hide it.
#ifndef WINPROOF88_ALLOW_VIRTUALKEYCODES
#define WINPROOF88_ALLOW_VIRTUALKEYCODES
#endif
#include "../../../PGE/PGE/PGE.h" // Using PGE.

#include "../../../PGE/PGE/Pure/include/external/Object3D/PureObject3DManager.h"

#include "BaseConsts.h"    // Constants, macros.
#include "ElteFailPacket.h"


struct Player_t
{
    pge_network::PgeNetworkConnectionHandle m_connHandleServerSide;   /**< Used by both server and clients to identify the connection.
                                                                           Clients don't use it for direct communication.
                                                                           Note: this is the client's handle on server side!
                                                                           This is not the same handle as client have for the connection
                                                                           towards the server! Those connection handles are not related
                                                                           to each other! */
    std::string m_sTrollface;
    PureObject3D* m_pObject3D;
    std::string m_sIpAddress;
};


/**
    The customized game engine class. This handles the game logic. Singleton.
*/
class CustomPGE :
    public PGE
{

public:

    static CustomPGE* createAndGetCustomPGEinstance();  /**< Creates and gets the only instance. */

    // ---------------------------------------------------------------------------

    CConsole& getConsole() const;
    static const char* getLoggerModuleName();
   
protected:

    CustomPGE()
    {}

    CustomPGE(const CustomPGE&)
    {}

    CustomPGE& operator=(const CustomPGE&)
    {
        return *this;
    }

    explicit CustomPGE(const char* gametitle);  /**< This is the only usable ctor, this is used by the static createAndGet(). */
    virtual ~CustomPGE();

    virtual bool onGameInitializing() override;  /**< Must-have minimal stuff before loading anything. */
    virtual bool onGameInitialized() override;   /**< Loading game content here. */
    virtual void onGameRunning() override;       /**< Game logic here. */
    virtual bool onPacketReceived(
        const pge_network::PgePacket& pkt) override;  /**< Called when a new network packet is received. */
    virtual void onGameDestroying() override;    /**< Freeing up game content here. */

private:
    PureObject3D* m_box1;
    PureObject3D* m_box2;
    std::string m_sUserName;   /**< User name received from server in PgePktUserConnected (server instance also receives this from itself). */
    std::map<std::string, Player_t> m_mapPlayers;  /**< Connected players. Used by both server and clients. Key is user name. */
    // TODO: originally username was planned to be the key for above map, however if we see that we can always use connHandleServerSide to
    // find proper player, then let's change the key to that instead of user name!
    std::set<std::string> m_trollFaces;              /**< Trollface texture file names. Used by server only. */
    bool m_bBackSpaceReleased;
    bool m_bShowGuiDemo;

    // ---------------------------------------------------------------------------

    void genUniqueUserName(char szNewUserName[elte_fail::MsgUserSetup::nUserNameMaxLength]) const;
    void WritePlayerList();
    bool handleUserSetup(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const elte_fail::MsgUserSetup& msg);
    bool handleUserConnected(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const pge_network::MsgUserConnectedServerSelf& msg);
    bool handleUserDisconnected(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const pge_network::MsgUserDisconnectedFromServer& msg);
    bool handleUserCmdMove(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const elte_fail::MsgUserCmdMove& msg);
    bool handleUserUpdate(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const elte_fail::MsgUserUpdate& msg);
}; // class CustomPGE