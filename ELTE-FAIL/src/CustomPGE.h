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

    virtual void onGameInitializing(); /**< Must-have minimal stuff before loading anything. */
    virtual void onGameInitialized();  /**< Loading game content here. */
    virtual void onGameRunning();      /**< Game logic here. */
    virtual void onGameDestroying();   /**< Freeing up game content here. */


private:
    PRREObject3D* box1;
    PRREObject3D* box2;

    // ---------------------------------------------------------------------------

    void HandleUserConnected(const PgePacket& pkt);
    void HandlePackets();


}; // class CustomPGE