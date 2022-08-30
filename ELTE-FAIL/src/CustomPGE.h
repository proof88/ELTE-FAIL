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
    uint32_t m_connHandle;     /**< Used by both server and clients to identify the connection.
                                    Clients don't use it for direct communication.*/
    std::string m_sTrollface;
    PRREObject3D* pObject3D;
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

    virtual void onGameInitializing();  /**< Must-have minimal stuff before loading anything. */
    virtual void onGameInitialized();   /**< Loading game content here. */
    virtual void onGameRunning();       /**< Game logic here. */
    virtual void onPacketReceived(
        uint32_t connHandle,
        const PgePacket& pkt);          /**< Called when a new network packet is received. */
    virtual void onGameDestroying();    /**< Freeing up game content here. */

private:
    PRREObject3D* box1;
    PRREObject3D* box2;
    std::string sUserName;   /**< User name received from server in PgePktUserConnected (server instance also receives this from itself). */
    std::map<std::string, Player_t> m_mapPlayers;  /**< Connected players. Used by both server and clients. */
    std::set<std::string> trollFaces;              /**< Trollface texture file names. Used by server only. */

    // ---------------------------------------------------------------------------

    void genUniqueUserName(char sNewUserName[64]) const;
    void HandleUserConnected(uint32_t connHandle, const PgePktUserConnected& pkt);
    void HandleUserDisconnected(uint32_t connHandle, const PgePktUserDisconnected& pkt);
    void HandleUserCmdMove(uint32_t connHandle, const PgePktUserCmdMove& pkt);
    void HandleUserUpdate(uint32_t connHandle, const PgePktUserUpdate& pkt);
}; // class CustomPGE