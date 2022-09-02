/*
    ###################################################################################
    CustomPGE.cpp
    Customized PGE.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

#include "CustomPGE.h"

#include <cassert>
#include <filesystem>  // requires cpp17
#include <set>
#include <sstream>

#include "../../../PGE/PGE/PRRE/include/external/PRREuiManager.h"
#include "../../../PGE/PGE/PRRE/include/external/Display/PRREWindow.h"
#include "../../../PGE/PGE/PRRE/include/external/PRRECamera.h"
#include "../../../CConsole/CConsole/src/CConsole.h"



// ############################### PUBLIC ################################

const ElteFailMsg::ElteFailMsgId ElteFailMsg::MsgUserCmdMove::id;
const ElteFailMsg::ElteFailMsgId ElteFailMsg::MsgUserUpdate::id;

/**
    Creates and gets the only instance.
*/
CustomPGE* CustomPGE::createAndGetCustomPGEinstance()
{
    static CustomPGE pgeInstance(ELTEFAIL_NAME);
    return &pgeInstance;
} // createAndGetCustomPGEinstance()


// ############################## PROTECTED ##############################


/**
    This is the only usable ctor, this is used by the static createAndGet().
*/
CustomPGE::CustomPGE(const char* gameTitle) :
    PGE(gameTitle)
{

} // CustomPGE(...)


CustomPGE::~CustomPGE()
{

} // ~CustomPGE()


CConsole& CustomPGE::getConsole() const
{
    return CConsole::getConsoleInstance(getLoggerModuleName());
} // getConsole()


const char* CustomPGE::getLoggerModuleName()
{
    return "CustomPGE";
} // getLoggerModuleName()


/**
    Must-have minimal stuff before loading anything.
    Game engine calls this before even finishing its own initialization.
*/
void CustomPGE::onGameInitializing()
{
    // Earliest we can enable our own logging
    getConsole().Initialize("ELTE-FAIL log", true);
    getConsole().SetFGColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, "999999" );
    getConsole().SetIntsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "FFFF00" );
    getConsole().SetStringsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "FFFFFF" );
    getConsole().SetFloatsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "FFFF00" );
    getConsole().SetBoolsColor( FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "00FFFF" );

    // CustomPGE (game) logs
    getConsole().SetLoggingState(getLoggerModuleName(), true);
    getConsole().SetLoggingState("PGESysNET", true);
    getConsole().SetLoggingState("PgeNetwork", true);
    getConsole().SetLoggingState("PgeServer", true);
    getConsole().SetLoggingState("PgeClient", true);

    // Turn everything on for development only
    getConsole().SetLoggingState("4LLM0DUL3S", true);
}


/** 
    Loading game content here.
*/
void CustomPGE::onGameInitialized()
{
    getConsole().OLnOI("CustomPGE::onGameInitialized()");

    // Dont want to see logs of loading of resources cause I'm debugging network now
    getConsole().SetLoggingState("4LLM0DUL3S", false);

    getPRRE().getCamera().SetNearPlane(0.1f);
    getPRRE().getCamera().SetFarPlane(100.0f);

    PRRETexture* const tex1 = getPRRE().getTextureManager().createFromFile("gamedata\\proba128x128x24.bmp");

    /*  */
    box1 = getPRRE().getObject3DManager().createBox(1, 1, 1);
    box1->getPosVec().SetZ(2.0f);
    box1->getPosVec().SetX(1.5f);
    box1->SetOccluder(true);
    box1->SetOcclusionTested(false);
    
    // since a while this vertex coloring is not working
    box1->getMaterial().getColors()[0].red    = 1.0f;
    box1->getMaterial().getColors()[0].green  = 0.0f;
    box1->getMaterial().getColors()[0].blue   = 0.0f;
    box1->getMaterial().getColors()[0].alpha  = 0.0f;
    box1->getMaterial().getColors()[9].red    = 1.0f;
    box1->getMaterial().getColors()[9].green  = 0.0f;
    box1->getMaterial().getColors()[9].blue   = 0.0f;
    box1->getMaterial().getColors()[9].alpha  = 0.0f;
    
    box1->getMaterial().setTexture(tex1);
    box1->setVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);  
    
    /*    */
    box2 = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\cube.obj");
    box2->setVertexTransferMode(PRRE_VT_DYN_DIR_1_BY_1);
    box2->getPosVec().SetZ(4); 
    
    /*       
    PRREObject3D* const plane1 = getPRRE().getObject3DManager().createPlane(2, 2);
    plane1->getPosVec().SetX(0);
    plane1->getPosVec().SetZ(2);
    plane1->getMaterial().setTexture(tex1);
    plane1->setVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);
    */

    PRREObject3D* const snail = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail.obj");
    snail->SetScaling(0.02f);
    snail->getPosVec().SetX(-1.5f);
    snail->getPosVec().SetZ(2.7f);  
    
    /* */   
    PRREObject3D* snail_lm = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail_lm.obj");
    snail_lm->SetScaling(0.02f);
    snail_lm->Hide();
     
    // dealing with lightmaps ...
    
    if ( snail->getCount() == snail_lm->getCount() ) 
    {
        for (TPRREint i = 0; i < snail->getCount(); i++)
        {
            PRREObject3D* const snailSub = (PRREObject3D*) snail->getAttachedAt(i);
            // assuming that snail_lm has the same subobjects and vertex count as snail
            PRREObject3D* const snailLMSub = (PRREObject3D*) snail_lm->getAttachedAt(i);
            if ( snailSub && snailLMSub )
            {
                // copying lightmap data into snail material's 2nd layer
                snailSub->getMaterial(false).copyFromMaterial(snailLMSub->getMaterial(false), 1, 0);
                snailSub->getMaterial(false).setBlendFuncs(PRRE_SRC_ALPHA, PRRE_ONE_MINUS_SRC_ALPHA, 1);
            }
        }
    }
    else
    {
        getConsole().EOLn("snail->getCount() != snail_lm->getCount(): %d != %d", snail->getCount(), snail_lm->getCount());
    }    
    
    snail->setVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);
    snail->SetDoubleSided(true);

    // at this point, we should be safe to delete snail_lm since object's dtor calls material's dtor which doesn't free up the textures
    // however, a mechanism is needed to be implemented to correctly handle this situation.
    // WA1: CopyFromMaterial() should hardcopy the textures also; deleting material should delete its textures too;
    // WA2: (better) textures should maintain refcount. Material deletion would decrement refcount and would effectively delete textures when refcount reaches 0.
    delete snail_lm;
    snail_lm = NULL; 

    /*
    PRREObject3D* snail_clone = getPRRE().getObject3DManager().createCloned(*snail);
    snail_clone->getPosVec().SetX(-1);
    snail->SetOcclusionTested(false);
    */

    /*         
    getPRRE().getTextureManager().setDefaultIsoFilteringMode(PRRE_ISO_LINEAR_MIPMAP_LINEAR, PRRE_ISO_LINEAR);

    PRREObject3D* const arena = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\arena\\arena.obj");
    arena->SetScaling(0.005f);
    arena->getPosVec().SetZ(2.f); 
    arena->getPosVec().SetY(-1.5f);

    PRREObject3D* arena_lm = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\arena\\arena_lm.obj");
    arena_lm->SetScaling(0.02f);
    arena_lm->Hide();

    // dealing with lightmaps ...
    if ( arena->getCount() == arena_lm->getCount() ) 
    {
        for (TPRREuint i = 0; i < arena->getCount(); i++)
        {
            PRREObject3D* const arenaSub = (PRREObject3D*) arena->getAttachedAt(i);
            // assuming that arena_lm has the same subobjects and vertex count as arena
            PRREObject3D* const arenaLMSub = (PRREObject3D*) arena_lm->getAttachedAt(i);
            if ( arenaSub && arenaLMSub )
            {
                // copying lightmap data into snail material's 2nd layer
                arenaSub->getMaterial(false).copyFromMaterial(arenaLMSub->getMaterial(false), 1, 0);
                arenaSub->getMaterial(false).setBlendFuncs(PRRE_SRC_ALPHA, PRRE_ONE_MINUS_SRC_ALPHA, 1);
            }
        }
    }
    else
    {
        getConsole().EOLn("arena->getCount() != arenaLMSub->getCount(): %d != %d", arena->getCount(), arena_lm->getCount());
    }

    arena->setVertexTransferMode(PRRE_VT_DYN_DIR_SVA_GEN);

    delete arena_lm;
    arena_lm = NULL;
    */

    // Gather some trollface pictures for the players
    // Building this set up initially, each face is removed from the set when assigned to a player, so
    // all players will have unique face texture assigned.
    for (const auto& entry : std::filesystem::directory_iterator("gamedata/trollfaces/"))
    {
        if ((entry.path().extension().string() == ".bmp"))
        {
            trollFaces.insert(entry.path().string());
        }
    }
    getConsole().OLn("%s() Server parsed %d trollfaces", __func__, trollFaces.size());
    
    getPRRE().getUImanager().addText("almafaALMAFA012345Ûˆ¸”’€_+", 10, 10);

    getConsole().OO();
    getConsole().OLn("");

    getPRRE().WriteList();

    if (getNetwork().isServer())
    {
        getNetwork().getServer().getBlackListedMessages().insert(static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserSetup::id));

        // MsgUserUpdate is also processed by server, but it injects this pkt into its own queue when needed.
        // MsgUserUpdate MUST NOT be received by server over network!
        // MsgUserUpdate is received only by clients over network!
        getNetwork().getServer().getBlackListedMessages().insert(static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserUpdate::id));

        if (!getNetwork().getServer().startListening())
        {
            PGE::showErrorDialog("Server has FAILED to start listening!");
            assert(false);
        }
    }
    else
    {
        getNetwork().getClient().getBlackListedMessages().insert(static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserCmdMove::id));

        if (!getNetwork().getClient().connectToServer("127.0.0.1"))
        {
            PGE::showErrorDialog("Client has FAILED to establish connection to the server!");
            assert(false);
        }
    }

} // onGameInitialized()


/** 
    Game logic here.
    DO NOT make any unnecessary operations here, as this function must complete below 16 msecs to keep 60 fps!
    Avoid dynamic memory allocations as well! Make sure you use a preallocated memory pool!
*/
void CustomPGE::onGameRunning()
{
    const PRREWindow& window = getPRRE().getWindow();
    const PGEInputHandler& input = PGEInputHandler::createAndGet();

    static bool bCameraLocked = true;

    input.getMouse().SetCursorPos(window.getX() + window.getWidth()/2,
                                  window.getY() + window.getHeight()/2);

    if (!getNetwork().isServer())
    {
        getPRRE().getUImanager().text(
            "Ping: " + std::to_string(getNetwork().getClient().getPing(true)) + " ms",
            10, 50);
        getPRRE().getUImanager().text(
            "Quality: local: " + std::to_string(getNetwork().getClient().getQualityLocal(false)) +
            "; remote: " + std::to_string(getNetwork().getClient().getQualityRemote(false)),
            10, 70);
        getPRRE().getUImanager().text(
            "Tx Speed: " + std::to_string(getNetwork().getClient().getTxByteRate(false)) +
            " Bps; Rx Speed: " + std::to_string(getNetwork().getClient().getRxByteRate(false)) + " Bps",
            10, 90);
        getPRRE().getUImanager().text(
            "Internal Queue Time: " + std::to_string(getNetwork().getClient().getInternalQueueTimeUSecs(false)) + " us",
            10, 110);
    }

    if ( box1 != NULL )
    {
       box1->getAngleVec().SetY( box1->getAngleVec().getY() + 0.2f );
    }

    if ( input.getKeyboard().isKeyPressed(VK_UP) )
    {
        getPRRE().getCamera().Move(0.01f);
    }
    if ( input.getKeyboard().isKeyPressed(VK_DOWN) )
    {
        getPRRE().getCamera().Move(-0.01f);
    }
    if ( input.getKeyboard().isKeyPressed(VK_LEFT) )
    {
        getPRRE().getCamera().Strafe(-0.01f);
    }
    if ( input.getKeyboard().isKeyPressed(VK_RIGHT) )
    {
        getPRRE().getCamera().Strafe(0.01f);
    }
    if ( input.getKeyboard().isKeyPressed(VK_SPACE) )
    {
        getPRRE().getCamera().Elevate(0.01f);
    }
    if ( input.getKeyboard().isKeyPressed(VK_CONTROL) )
    {
        getPRRE().getCamera().Elevate(-0.01f);
    }

    if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('1')) )
    {
        if ( box1 != NULL )
        {
            box1->SetRenderingAllowed( !box1->isRenderingAllowed() );
            Sleep(200); /* to make sure key is released, avoid bouncing */
        }
    }

    if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('2')) )
    {
        PRREObject3D* snailobj = (PRREObject3D*) getPRRE().getObject3DManager().getByFilename("gamedata\\models\\snail_proofps\\snail.obj");
        if ( snailobj != NULL )
        {
            snailobj->SetRenderingAllowed( !snailobj->isRenderingAllowed() );
            Sleep(200);
        }
    }

    if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('3')) )
    {
        PRREObject3D* arenaobj = (PRREObject3D*) getPRRE().getObject3DManager().getByFilename("gamedata\\models\\arena\\arena.obj");
        if ( arenaobj != NULL )
        {
            arenaobj->SetRenderingAllowed( !arenaobj->isRenderingAllowed() );
            Sleep(200);
        }
    }

    ElteFailMsg::HorizontalDirection horDir = ElteFailMsg::HorizontalDirection::NONE;
    ElteFailMsg::VerticalDirection verDir = ElteFailMsg::VerticalDirection::NONE;
    if (input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('w')))
    {
        verDir = ElteFailMsg::VerticalDirection::UP;
    }
    if (input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('s')))
    {
        verDir = ElteFailMsg::VerticalDirection::DOWN;
    }
    if (input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('a')))
    {
        horDir = ElteFailMsg::HorizontalDirection::LEFT;
    }
    if (input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('d')))
    {
        horDir = ElteFailMsg::HorizontalDirection::RIGHT;
    }

    if ((horDir != ElteFailMsg::HorizontalDirection::NONE) || (verDir != ElteFailMsg::VerticalDirection::NONE))
    {
        pge_network::PgePacket pkt;
        memset(&pkt, 0, sizeof(pkt));
        pkt.pktId = pge_network::PgePktId::APP;
        pkt.msg.app.msgId = static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserCmdMove::id);
        ElteFailMsg::MsgUserCmdMove& msgCmdMove = reinterpret_cast<ElteFailMsg::MsgUserCmdMove&>(pkt.msg.app.cData);
        msgCmdMove.horDirection = horDir;
        msgCmdMove.verDirection = verDir;

        if (getNetwork().isServer())
        {
            // inject this packet to server's queue
            // server will properly update its own position and send update to all clients too based on current state of HandleUserCmdMove()
            // TODO: for sure an inject function would be nice which automatically fills in server username!
            getNetwork().getServer().getPacketQueue().push_back(pkt);
        }
        else
        {
            // here username is not needed since this is sent by client, and server will identify the client anyway based on connection id
            getNetwork().getClient().SendToServer(pkt);
        }
    }

    // L for camera Lock
    if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('l')) )
    {
        bCameraLocked = !bCameraLocked;
        Sleep(200);
    }

    if ( bCameraLocked )
    {
        //getPRRE().getCamera().getTargetVec().Set( box1->getPosVec().getX(), box1->getPosVec().getY(), box1->getPosVec().getZ() );

        const auto it = m_mapPlayers.find(sUserName);
        if (it != m_mapPlayers.end())
        {
            PRREObject3D* pPlayerObj = it->second.pObject3D;
            if (pPlayerObj)
            {
                getPRRE().getCamera().getTargetVec().Set(pPlayerObj->getPosVec().getX(), pPlayerObj->getPosVec().getY(), pPlayerObj->getPosVec().getZ());
            }
        }
    }

    std::stringstream str;
    //str << "MX1: " << changeX << "   MY1: " << changeY;
    //str << "key: " << input.getKeyboard().isKeyPressed(VK_RETURN);

    //window.SetCaption(str.str());
} // onGameRunning()


/**
    Called when a new network packet is received.
*/
void CustomPGE::onPacketReceived(pge_network::PgeNetworkConnectionHandle connHandle, const pge_network::PgePacket& pkt)
{
    switch (pkt.pktId)
    {
    case pge_network::PgeMsgUserConnected::id:
        HandleUserConnected(connHandle, pkt.msg.userConnected);
        break;
    case pge_network::PgeMsgUserDisconnected::id:
        HandleUserDisconnected(connHandle, pkt.msg.userDisconnected);
        break;
    case pge_network::PgeMsgApp::id:
    {
        switch (static_cast<ElteFailMsg::ElteFailMsgId>(pkt.msg.app.msgId))
        {
        case ElteFailMsg::MsgUserSetup::id:
            HandleUserSetup(connHandle, reinterpret_cast<const ElteFailMsg::MsgUserSetup&>(pkt.msg.app.cData));
            break;
        case ElteFailMsg::MsgUserCmdMove::id:
            HandleUserCmdMove(connHandle, reinterpret_cast<const ElteFailMsg::MsgUserCmdMove&>(pkt.msg.app.cData));
            break;
        case ElteFailMsg::MsgUserUpdate::id:
            HandleUserUpdate(connHandle, reinterpret_cast<const ElteFailMsg::MsgUserUpdate&>(pkt.msg.app.cData));
            break;
        default:
            getConsole().EOLn("CustomPGE::%s(): unknown msgId %u in PgeMsgApp!", __func__, pkt.pktId);
        }
        break;
    }
    default:
        getConsole().EOLn("CustomPGE::%s(): unknown pktId %u!", __func__, pkt.pktId);
    }
}


/** 
    Freeing up game content here.
    Free up everything that has been allocated in onGameInitialized() and onGameRunning().
*/
void CustomPGE::onGameDestroying()
{
    m_mapPlayers.clear();

    delete box1;
    box1 = NULL;
    getPRRE().getObject3DManager().DeleteAll();

    getConsole().Deinitialize();
} // onGameRunning()


// ############################### PRIVATE ###############################


void CustomPGE::genUniqueUserName(char sNewUserName[64]) const
{
    bool found = false;
    do
    {
        sprintf_s(sNewUserName, 64, "User%d", 10000 + (rand() % 100000));
        for (const auto& client : m_mapPlayers)
        {
            found = (client.first == sNewUserName);
            if (found)
            {
                break;
            }
        }
    } while (found);
}

void CustomPGE::HandleUserSetup(pge_network::PgeNetworkConnectionHandle connHandle, const ElteFailMsg::MsgUserSetup& msg)
{
    if (getNetwork().isServer())
    {
        getConsole().EOLn("CustomPGE::%s(): server received MsgUserSetup, CANNOT HAPPEN!", __func__);
        assert(false);
        return;
    }

    if ((strnlen(msg.szUserName, 64) > 0) && (m_mapPlayers.end() != m_mapPlayers.find(msg.szUserName)))
    {
        getConsole().EOLn("CustomPGE::%s(): cannot happen: user %s (connHandle: %u) is already present in players list!",
            __func__, msg.szUserName, connHandle);
        assert(false);
        return;
    }

    if (msg.bCurrentClient)
    {
        getConsole().OLn("CustomPGE::%s(): this is me, my name is %s, connHandle: %u", __func__, msg.szUserName, connHandle);
        // store our username so we can refer to it anytime later
        sUserName = msg.szUserName;
        getPRRE().getUImanager().addText("Client, User name: " + sUserName, 10, 30);
    }
    else
    {
        getConsole().OLn("CustomPGE::%s(): new remote user %s (connHandle: %u) connected and I'm client", __func__, msg.szUserName, connHandle);
    }

    // insert user into map using wacky syntax
    m_mapPlayers[msg.szUserName];
    m_mapPlayers[msg.szUserName].m_sTrollface = msg.szTrollfaceTex;
    m_mapPlayers[msg.szUserName].m_connHandle = connHandle;

    PRREObject3D* const plane = getPRRE().getObject3DManager().createPlane(0.5f, 0.5f);
    if (!plane)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to create object for user %s!", __func__, msg.szUserName);
        // TODO: should exit or sg
        return;
    }

    plane->SetDoubleSided(true);
    plane->getPosVec().SetX(0);
    plane->getPosVec().SetZ(2);

    if (!m_mapPlayers[msg.szUserName].m_sTrollface.empty())
    {
        PRRETexture* const tex = getPRRE().getTextureManager().createFromFile(m_mapPlayers[msg.szUserName].m_sTrollface.c_str());
        if (tex)
        {
            plane->getMaterial().setTexture(tex);
        }
        else
        {
            getConsole().EOLn("CustomPGE::%s(): failed to load trollface texture %s for user %s!",
                __func__, m_mapPlayers[msg.szUserName].m_sTrollface.c_str(), msg.szUserName);
        }
    }
    else
    {
        getConsole().EOLn("CustomPGE::%s(): trollface texture name empty for user %s!",
            __func__, msg.szUserName);
    }

    plane->setVertexModifyingHabit(PRRE_VMOD_STATIC);
    plane->setVertexReferencingMode(PRRE_VREF_INDEXED);

    m_mapPlayers[msg.szUserName].pObject3D = plane;
}

void CustomPGE::HandleUserConnected(pge_network::PgeNetworkConnectionHandle connHandle, const pge_network::PgeMsgUserConnected& msg)
{
    const char* szConnectedUserName = nullptr;
    std::string sTrollface;

    if (getNetwork().isServer())
    {
        // server doesn't care about msg.sTrollfaceTex because that is empty
        if (trollFaces.size() > 0)
        {
            sTrollface = *trollFaces.begin();
            trollFaces.erase(trollFaces.begin());
        }
        else
        {
            CConsole::getConsoleInstance("PGESysNET").EOLn("%s: SERVER No more trollfaces left for user %s", __func__, szConnectedUserName);
        }

        if (msg.bCurrentClient)
        {
            // server is processing its own birth
            if (m_mapPlayers.size() == 0)
            {
                char szNewUserName[64];
                genUniqueUserName(szNewUserName);
                getConsole().OLn("CustomPGE::%s(): first (local) user %s connected and I'm server, so this is me (connHandle: %u)",
                    __func__, szNewUserName, connHandle);
                // store our username so we can refer to it anytime later
                sUserName = szNewUserName;
                szConnectedUserName = szNewUserName;
                getPRRE().getUImanager().addText("Server, User name: " + sUserName, 10, 30);
            }
            else
            {
                // cannot happen
                getConsole().EOLn("CustomPGE::%s(): user (connHandle: %u) connected with bCurrentClient as true but it is not me, CANNOT HAPPEN!", 
                   __func__, connHandle);
                assert(false);
                return;
            }
        }
        else
        {
            // server is processing another user's birth

            if (m_mapPlayers.empty())
            {
                // cannot happen because at least the user of the server should be in the map!
                // this should happen only if we are dedicated server but currently only listen-server is supported!
                getConsole().EOLn("CustomPGE::%s(): non-server user (connHandle: %u) connected but map of players is still empty, CANNOT HAPPEN!",
                    __func__, connHandle);
                assert(false);
                return;
            }

            char szNewUserName[64];
            genUniqueUserName(szNewUserName);
            szConnectedUserName = szNewUserName;
            getConsole().OLn("CustomPGE::%s(): new remote user %s (connHandle: %u) connected and I'm server",
                __func__, szConnectedUserName, connHandle);

            pge_network::PgePacket newPktSetup;
            memset(&newPktSetup, 0, sizeof(newPktSetup));
            newPktSetup.connHandle = connHandle;
            newPktSetup.pktId = pge_network::PgePktId::APP;
            newPktSetup.msg.app.msgId = static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserSetup::id);
            ElteFailMsg::MsgUserSetup& msgUserSetup = reinterpret_cast<ElteFailMsg::MsgUserSetup&>(newPktSetup.msg.app.cData);
            msgUserSetup.bCurrentClient = false;
            strncpy_s(msgUserSetup.szUserName, 64, szConnectedUserName, 64);
            strncpy_s(msgUserSetup.szTrollfaceTex, 64, sTrollface.c_str(), 64);

            // inform all other clients about this new user
            getNetwork().getServer().SendPacketToAllClients(newPktSetup, connHandle);

            // now we send this msg to the client with this bool flag set so client will know it is their connect
            msgUserSetup.bCurrentClient = true;
            getNetwork().getServer().SendPacketToClient(connHandle, newPktSetup);

            // we also send as many PgeMsgUserConnected pkts to the client as the number of already connected players,
            // otherwise client won't know about them, so the client will detect them as newly connected users
            msgUserSetup.bCurrentClient = false;
            for (const auto& it : m_mapPlayers)
            {
                newPktSetup.connHandle = it.second.m_connHandle;
                strncpy_s(msgUserSetup.szUserName, 64, it.first.c_str(), 64);
                strncpy_s(msgUserSetup.szTrollfaceTex, 64, it.second.m_sTrollface.c_str(), 64);
                getNetwork().getServer().SendPacketToClient(connHandle, newPktSetup);
            }
        }
    }
    else
    {
        getConsole().EOLn("CustomPGE::%s(): client received PktUserConnected, CANNOT HAPPEN!", __func__);
        assert(false);
        return;
    }

    // insert user into map using wacky syntax
    m_mapPlayers[szConnectedUserName];
    m_mapPlayers[szConnectedUserName].m_sTrollface = sTrollface;
    m_mapPlayers[szConnectedUserName].m_connHandle = connHandle;

    PRREObject3D* const plane = getPRRE().getObject3DManager().createPlane(0.5f, 0.5f);
    if (!plane)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to create object for user %s!", __func__, szConnectedUserName);
        // TODO: should exit or sg
        return;
    }

    plane->SetDoubleSided(true);
    plane->getPosVec().SetX(0);
    plane->getPosVec().SetZ(2);

    if (!sTrollface.empty())
    {
        PRRETexture* const tex = getPRRE().getTextureManager().createFromFile(sTrollface.c_str());
        if (tex)
        {
            plane->getMaterial().setTexture(tex);
        }
        else
        {
            getConsole().EOLn("CustomPGE::%s(): failed to load trollface texture %s for user %s!",
                __func__, sTrollface.c_str(), szConnectedUserName);
        }
    }
    else
    {
        getConsole().EOLn("CustomPGE::%s(): trollface texture name empty for user %s!",
            __func__, szConnectedUserName);
    }
    
    plane->setVertexModifyingHabit(PRRE_VMOD_STATIC);
    plane->setVertexReferencingMode(PRRE_VREF_INDEXED);

    m_mapPlayers[szConnectedUserName].pObject3D = plane;
}

void CustomPGE::HandleUserDisconnected(pge_network::PgeNetworkConnectionHandle connHandle, const pge_network::PgeMsgUserDisconnected&)
{
    auto it = m_mapPlayers.begin();
    while (it != m_mapPlayers.end())
    {
        if (it->second.m_connHandle == connHandle)
        {
            break;
        }
        it++;
    }

    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user with connHandle: %u!", __func__, connHandle);
        return;
    }

    const std::string& sClientUserName = it->first;

    if (getNetwork().isServer())
    {
        getConsole().OLn("CustomPGE::%s(): user %s disconnected and I'm server", __func__, sClientUserName.c_str());
        trollFaces.insert(it->second.m_sTrollface);  // re-insert the unneeded trollface texture into the set
    }
    else
    {
        getConsole().OLn("CustomPGE::%s(): user %s disconnected and I'm client", __func__, sClientUserName.c_str());
    }

    if (it->second.pObject3D)
    {
        delete it->second.pObject3D;  // yes, dtor will remove this from its Object3DManager too!
    }

    m_mapPlayers.erase(it);
}

void CustomPGE::HandleUserCmdMove(pge_network::PgeNetworkConnectionHandle connHandle, const ElteFailMsg::MsgUserCmdMove& pktUserCmdMove)
{
    auto it = m_mapPlayers.begin();
    while (it != m_mapPlayers.end())
    {
        if (it->second.m_connHandle == connHandle)
        {
            break;
        }
        it++;
    }
    
    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user with connHandle: %u!", __func__, connHandle);
        return;
    }

    const std::string& sClientUserName = it->first;

    PRREObject3D* obj = it->second.pObject3D;
    if (!obj)
    {
        getConsole().EOLn("CustomPGE::%s(): user %s doesn't have associated Object3D!", __func__, sClientUserName.c_str());
        return;
    }

    if ((pktUserCmdMove.horDirection == ElteFailMsg::HorizontalDirection::NONE) && 
        (pktUserCmdMove.verDirection == ElteFailMsg::VerticalDirection::NONE))
    {
        getConsole().EOLn("CustomPGE::%s(): user %s sent invalid cmdMove!", __func__, sClientUserName.c_str());
        return;
    }

    //getConsole().OLn("CustomPGE::%s(): user %s sent valid cmdMove", __func__, sClientUserName.c_str());
    switch (pktUserCmdMove.horDirection)
    {
    case ElteFailMsg::HorizontalDirection::LEFT:
        obj->getPosVec().SetX( obj->getPosVec().getX() - 0.01f );
        break;
    case ElteFailMsg::HorizontalDirection::RIGHT:
        obj->getPosVec().SetX(obj->getPosVec().getX() + 0.01f);
        break;
    default: /* no-op */
        break;
    }

    switch (pktUserCmdMove.verDirection)
    {
    case ElteFailMsg::VerticalDirection::DOWN:
        obj->getPosVec().SetY(obj->getPosVec().getY() - 0.01f);
        break;
    case ElteFailMsg::VerticalDirection::UP:
        obj->getPosVec().SetY(obj->getPosVec().getY() + 0.01f);
        break;
    default: /* no-op */
        break;
    }

    pge_network::PgePacket pktOut;
    memset(&pktOut, 0, sizeof(pktOut));
    pktOut.pktId = pge_network::PgePktId::APP;
    pktOut.msg.app.msgId = static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserUpdate::id);
    ElteFailMsg::MsgUserUpdate& msgUserUpdate = reinterpret_cast<ElteFailMsg::MsgUserUpdate&>(pktOut.msg.app.cData);
    strncpy_s(msgUserUpdate.szUserName, 64, sClientUserName.c_str(), 64);
    msgUserUpdate.pos.x = obj->getPosVec().getX();
    msgUserUpdate.pos.y = obj->getPosVec().getY();
    getNetwork().getServer().SendPacketToAllClients(pktOut);
    // this msgUserUpdate should be also sent to server as self
    // maybe the SendPacketToAllClients() should be enhanced to contain packet injection for server's packet queue!
    getNetwork().getServer().getPacketQueue().push_back(pktOut);
}

void CustomPGE::HandleUserUpdate(pge_network::PgeNetworkConnectionHandle, const ElteFailMsg::MsgUserUpdate& msg)
{
    auto it = m_mapPlayers.find(msg.szUserName);
    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user: %s!", __func__, msg.szUserName);
        return;
    }

    PRREObject3D* obj = it->second.pObject3D;
    if (!obj)
    {
        getConsole().EOLn("CustomPGE::%s(): user %s doesn't have associated Object3D!", __func__, msg.szUserName);
        return;
    }

    obj->getPosVec().SetX(msg.pos.x);
    obj->getPosVec().SetY(msg.pos.y);
}


