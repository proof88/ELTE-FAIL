#pragma once

/*
    ###################################################################################
    CustomPGE.h
    Customized PGE.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

#include "../../../PGE/PGE/PGE.h" // Using PGE.
#include "../../../PGE/PGE/PRRE/include/external/Object3D/PRREObject3DManager.h"

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
    PRREObject3D* pObject3D;
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

    virtual void onGameInitializing() override;  /**< Must-have minimal stuff before loading anything. */
    virtual void onGameInitialized() override;   /**< Loading game content here. */
    virtual void onGameRunning() override;       /**< Game logic here. */
    virtual void onPacketReceived(
        pge_network::PgeNetworkConnectionHandle connHandle,
        const pge_network::PgePacket& pkt) override;  /**< Called when a new network packet is received. */
    virtual void onGameDestroying() override;    /**< Freeing up game content here. */

private:
    PRREObject3D* box1;
    PRREObject3D* box2;
    std::string sUserName;   /**< User name received from server in PgePktUserConnected (server instance also receives this from itself). */
    std::map<std::string, Player_t> m_mapPlayers;  /**< Connected players. Used by both server and clients. */
    std::set<std::string> trollFaces;              /**< Trollface texture file names. Used by server only. */

    // ---------------------------------------------------------------------------

    void genUniqueUserName(char szNewUserName[ElteFailMsg::MsgUserSetup::nUserNameMaxLength]) const;
    void WritePlayerList();
    void HandleUserSetup(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const ElteFailMsg::MsgUserSetup& msg);
    void HandleUserConnected(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const pge_network::PgeMsgUserConnected& msg);
    void HandleUserDisconnected(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const pge_network::PgeMsgUserDisconnected& msg);
    void HandleUserCmdMove(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const ElteFailMsg::MsgUserCmdMove& msg);
    void HandleUserUpdate(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const ElteFailMsg::MsgUserUpdate& msg);
}; // class CustomPGE