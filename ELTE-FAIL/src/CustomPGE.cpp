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

    {   // create box object internally
        box1 = getPRRE().getObject3DManager().createBox(1, 1, 1);
        box1->getPosVec().SetZ(2.0f);
        box1->getPosVec().SetX(1.5f);
        box1->SetOccluder(true);
        box1->SetOcclusionTested(false);

        // since a while this vertex coloring is not working
        box1->getMaterial().getColors()[0].red = 1.0f;
        box1->getMaterial().getColors()[0].green = 0.0f;
        box1->getMaterial().getColors()[0].blue = 0.0f;
        box1->getMaterial().getColors()[0].alpha = 0.0f;
        box1->getMaterial().getColors()[9].red = 1.0f;
        box1->getMaterial().getColors()[9].green = 0.0f;
        box1->getMaterial().getColors()[9].blue = 0.0f;
        box1->getMaterial().getColors()[9].alpha = 0.0f;

        box1->getMaterial().setTexture(tex1);
        box1->setVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);
    }
    
    {   // load box object from file
        box2 = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\cube.obj");
        box2->setVertexTransferMode(PRRE_VT_DYN_DIR_1_BY_1);
        box2->getPosVec().SetZ(4);
    }
    
    /*       
    PRREObject3D* const plane1 = getPRRE().getObject3DManager().createPlane(2, 2);
    plane1->getPosVec().SetX(0);
    plane1->getPosVec().SetZ(2);
    plane1->getMaterial().setTexture(tex1);
    plane1->setVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);
    */

    {   // snail
        PRREObject3D* const snail = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail.obj");
        snail->SetScaling(0.02f);
        snail->getPosVec().SetX(-1.5f);
        snail->getPosVec().SetZ(2.7f);

        PRREObject3D* snail_lm = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail_lm.obj");
        snail_lm->SetScaling(0.02f);
        snail_lm->Hide();

        // dealing with lightmaps ...
        if (snail->getCount() == snail_lm->getCount())
        {
            for (TPRREint i = 0; i < snail->getCount(); i++)
            {
                PRREObject3D* const snailSub = (PRREObject3D*)snail->getAttachedAt(i);
                // assuming that snail_lm has the same subobjects and vertex count as snail
                PRREObject3D* const snailLMSub = (PRREObject3D*)snail_lm->getAttachedAt(i);
                if (snailSub && snailLMSub)
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
    }

    /*
    PRREObject3D* snail_clone = getPRRE().getObject3DManager().createCloned(*snail);
    snail_clone->getPosVec().SetX(-1);
    snail->SetOcclusionTested(false);
    */

    {   // arena
        getPRRE().getTextureManager().setDefaultIsoFilteringMode(PRRE_ISO_LINEAR_MIPMAP_LINEAR, PRRE_ISO_LINEAR);

        PRREObject3D* const arena = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\arena\\arena.obj");
        arena->SetScaling(0.002f);
        arena->getPosVec().SetZ(2.f);
        arena->getPosVec().SetY(-1.5f);

        PRREObject3D* arena_lm = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\arena\\arena_lm.obj");
        arena_lm->SetScaling(0.02f);
        arena_lm->Hide();

        // dealing with lightmaps ...
        if (arena->getCount() == arena_lm->getCount())
        {
            for (TPRREint i = 0; i < arena->getCount(); i++)
            {
                PRREObject3D* const arenaSub = (PRREObject3D*)arena->getAttachedAt(i);
                // assuming that arena_lm has the same subobjects and vertex count as arena
                PRREObject3D* const arenaLMSub = (PRREObject3D*)arena_lm->getAttachedAt(i);
                if (arenaSub && arenaLMSub)
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
    }

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
        getNetwork().getServer().getBlackListedAppMessages().insert(static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserSetup::id));

        // MsgUserUpdate is also processed by server, but it injects this pkt into its own queue when needed.
        // MsgUserUpdate MUST NOT be received by server over network!
        // MsgUserUpdate is received only by clients over network!
        getNetwork().getServer().getBlackListedAppMessages().insert(static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserUpdate::id));

        if (!getNetwork().getServer().startListening())
        {
            PGE::showErrorDialog("Server has FAILED to start listening!");
            assert(false);
        }
    }
    else
    {
        getNetwork().getClient().getBlackListedAppMessages().insert(static_cast<pge_network::TPgeMsgAppMsgId>(ElteFailMsg::MsgUserCmdMove::id));

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
    PRREWindow& window = getPRRE().getWindow();
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

    if (input.getKeyboard().isKeyPressed(VK_ESCAPE))
    {
        window.Close();
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
        ElteFailMsg::MsgUserCmdMove::initPkt(pkt, horDir, verDir);

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
void CustomPGE::onPacketReceived(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const pge_network::PgePacket& pkt)
{
    switch (pkt.pktId)
    {
    case pge_network::MsgUserConnected::id:
        HandleUserConnected(m_connHandleServerSide, pkt.msg.userConnected);
        break;
    case pge_network::MsgUserDisconnected::id:
        HandleUserDisconnected(m_connHandleServerSide, pkt.msg.userDisconnected);
        break;
    case pge_network::MsgApp::id:
    {
        switch (static_cast<ElteFailMsg::ElteFailMsgId>(pkt.msg.app.msgId))
        {
        case ElteFailMsg::MsgUserSetup::id:
            HandleUserSetup(m_connHandleServerSide, reinterpret_cast<const ElteFailMsg::MsgUserSetup&>(pkt.msg.app.cData));
            break;
        case ElteFailMsg::MsgUserCmdMove::id:
            HandleUserCmdMove(m_connHandleServerSide, reinterpret_cast<const ElteFailMsg::MsgUserCmdMove&>(pkt.msg.app.cData));
            break;
        case ElteFailMsg::MsgUserUpdate::id:
            HandleUserUpdate(m_connHandleServerSide, reinterpret_cast<const ElteFailMsg::MsgUserUpdate&>(pkt.msg.app.cData));
            break;
        default:
            getConsole().EOLn("CustomPGE::%s(): unknown msgId %u in MsgApp!", __func__, pkt.pktId);
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


void CustomPGE::genUniqueUserName(char szNewUserName[ElteFailMsg::MsgUserSetup::nUserNameMaxLength]) const
{
    bool found = false;
    do
    {
        sprintf_s(szNewUserName, ElteFailMsg::MsgUserSetup::nUserNameMaxLength, "User%d", 10000 + (rand() % 100000));
        for (const auto& client : m_mapPlayers)
        {
            found = (client.first == szNewUserName);
            if (found)
            {
                break;
            }
        }
    } while (found);
}

void CustomPGE::WritePlayerList()
{
    getConsole().OLnOI("CustomPGE::%s()", __func__);
    for (const auto& player : m_mapPlayers)
    {
        getConsole().OLn("Username: %s; connHandleServerSide: %u; address: %s; trollFace: %s",
            player.first.c_str(), player.second.m_connHandleServerSide, player.second.m_sIpAddress.c_str(), player.second.m_sTrollface.c_str());
    }
    getConsole().OLnOO("");
}

void CustomPGE::HandleUserSetup(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const ElteFailMsg::MsgUserSetup& msg)
{
    if (getNetwork().isServer())
    {
        getConsole().EOLn("CustomPGE::%s(): server received MsgUserSetup, CANNOT HAPPEN!", __func__);
        assert(false);
        return;
    }

    if ((strnlen(msg.m_szUserName, ElteFailMsg::MsgUserSetup::nUserNameMaxLength) > 0) && (m_mapPlayers.end() != m_mapPlayers.find(msg.m_szUserName)))
    {
        getConsole().EOLn("CustomPGE::%s(): cannot happen: user %s (m_connHandleServerSide: %u) is already present in players list!",
            __func__, msg.m_szUserName, m_connHandleServerSide);
        assert(false);
        return;
    }

    if (msg.m_bCurrentClient)
    {
        getConsole().OLn("CustomPGE::%s(): this is me, my name is %s, m_connHandleServerSide: %u, my IP: %s",
            __func__, msg.m_szUserName, m_connHandleServerSide, msg.m_szIpAddress);
        // store our username so we can refer to it anytime later
        sUserName = msg.m_szUserName;
        getPRRE().getUImanager().addText("Client, User name: " + sUserName + "; IP: " + msg.m_szIpAddress, 10, 30);
    }
    else
    {
        getConsole().OLn("CustomPGE::%s(): new remote user %s (m_connHandleServerSide: %u; IP: %s) connected and I'm client",
            __func__, msg.m_szUserName, m_connHandleServerSide, msg.m_szIpAddress);
    }

    // insert user into map using wacky syntax
    m_mapPlayers[msg.m_szUserName];
    m_mapPlayers[msg.m_szUserName].m_sTrollface = msg.m_szTrollfaceTex;
    m_mapPlayers[msg.m_szUserName].m_connHandleServerSide = m_connHandleServerSide;
    m_mapPlayers[msg.m_szUserName].m_sIpAddress = msg.m_szIpAddress;

    PRREObject3D* const plane = getPRRE().getObject3DManager().createPlane(0.5f, 0.5f);
    if (!plane)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to create object for user %s!", __func__, msg.m_szUserName);
        // TODO: should exit or sg
        return;
    }

    plane->SetDoubleSided(true);
    plane->getPosVec().SetX(0);
    plane->getPosVec().SetZ(2);

    if (!m_mapPlayers[msg.m_szUserName].m_sTrollface.empty())
    {
        PRRETexture* const tex = getPRRE().getTextureManager().createFromFile(m_mapPlayers[msg.m_szUserName].m_sTrollface.c_str());
        if (tex)
        {
            plane->getMaterial().setTexture(tex);
        }
        else
        {
            getConsole().EOLn("CustomPGE::%s(): failed to load trollface texture %s for user %s!",
                __func__, m_mapPlayers[msg.m_szUserName].m_sTrollface.c_str(), msg.m_szUserName);
        }
    }
    else
    {
        getConsole().EOLn("CustomPGE::%s(): trollface texture name empty for user %s!",
            __func__, msg.m_szUserName);
    }

    plane->setVertexModifyingHabit(PRRE_VMOD_STATIC);
    plane->setVertexReferencingMode(PRRE_VREF_INDEXED);

    m_mapPlayers[msg.m_szUserName].pObject3D = plane;

    getNetwork().WriteList();
    WritePlayerList();
}

void CustomPGE::HandleUserConnected(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const pge_network::MsgUserConnected& msg)
{
    if (!getNetwork().isServer())
    {
        getConsole().EOLn("CustomPGE::%s(): client received PktUserConnected, CANNOT HAPPEN!", __func__);
        assert(false);
        return;
    }

    const char* szConnectedUserName = nullptr;
    std::string sTrollface;

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
            char szNewUserName[ElteFailMsg::MsgUserSetup::nUserNameMaxLength];
            genUniqueUserName(szNewUserName);
            getConsole().OLn("CustomPGE::%s(): first (local) user %s connected and I'm server, so this is me (m_connHandleServerSide: %u)",
                __func__, szNewUserName, m_connHandleServerSide);
            // store our username so we can refer to it anytime later
            sUserName = szNewUserName;
            szConnectedUserName = szNewUserName;
            getPRRE().getUImanager().addText("Server, User name: " + sUserName, 10, 30);
        }
        else
        {
            // cannot happen
            getConsole().EOLn("CustomPGE::%s(): user (m_connHandleServerSide: %u) connected with bCurrentClient as true but it is not me, CANNOT HAPPEN!", 
               __func__, m_connHandleServerSide);
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
            getConsole().EOLn("CustomPGE::%s(): non-server user (m_connHandleServerSide: %u) connected but map of players is still empty, CANNOT HAPPEN!",
                __func__, m_connHandleServerSide);
            assert(false);
            return;
        }

        char szNewUserName[ElteFailMsg::MsgUserSetup::nUserNameMaxLength];
        genUniqueUserName(szNewUserName);
        szConnectedUserName = szNewUserName;
        getConsole().OLn("CustomPGE::%s(): new remote user %s (m_connHandleServerSide: %u) connected (from %s) and I'm server",
            __func__, szConnectedUserName, m_connHandleServerSide, msg.szIpAddress);

        pge_network::PgePacket newPktSetup;
        ElteFailMsg::MsgUserSetup::initPkt(newPktSetup, m_connHandleServerSide, false, szConnectedUserName, sTrollface, msg.szIpAddress);

        // inform all other clients about this new user
        getNetwork().getServer().SendPacketToAllClients(newPktSetup, m_connHandleServerSide);

        // now we send this msg to the client with this bool flag set so client will know it is their connect
        ElteFailMsg::MsgUserSetup& msgUserSetup = reinterpret_cast<ElteFailMsg::MsgUserSetup&>(newPktSetup.msg.app.cData);
        msgUserSetup.m_bCurrentClient = true;
        getNetwork().getServer().SendPacketToClient(m_connHandleServerSide, newPktSetup);

        // we also send as many MsgUserSetup pkts to the client as the number of already connected players,
        // otherwise client won't know about them, so this way the client will detect them as newly connected users;
        // we also send MsgUserUpdate about each player so new client will immediately have their positions updated.
        pge_network::PgePacket newPktUserUpdate;
        for (const auto& it : m_mapPlayers)
        {
            ElteFailMsg::MsgUserSetup::initPkt(
                newPktSetup,
                it.second.m_connHandleServerSide,
                false,
                it.first, it.second.m_sTrollface, it.second.m_sIpAddress);
            getNetwork().getServer().SendPacketToClient(m_connHandleServerSide, newPktSetup);
            
            ElteFailMsg::MsgUserUpdate::initPkt(
                newPktUserUpdate,
                it.second.m_connHandleServerSide,
                it.first,
                it.second.pObject3D->getPosVec().getX(), it.second.pObject3D->getPosVec().getY(), it.second.pObject3D->getPosVec().getZ());
            getNetwork().getServer().SendPacketToClient(m_connHandleServerSide, newPktUserUpdate);
        }
    }

    // insert user into map using wacky syntax
    m_mapPlayers[szConnectedUserName];
    m_mapPlayers[szConnectedUserName].m_sTrollface = sTrollface;
    m_mapPlayers[szConnectedUserName].m_connHandleServerSide = m_connHandleServerSide;
    m_mapPlayers[szConnectedUserName].m_sIpAddress = msg.szIpAddress;

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

    getNetwork().WriteList();
    WritePlayerList();
}

void CustomPGE::HandleUserDisconnected(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const pge_network::MsgUserDisconnected&)
{
    auto it = m_mapPlayers.begin();
    while (it != m_mapPlayers.end())
    {
        if (it->second.m_connHandleServerSide == m_connHandleServerSide)
        {
            break;
        }
        it++;
    }

    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user with m_connHandleServerSide: %u!", __func__, m_connHandleServerSide);
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

    getNetwork().WriteList();
    WritePlayerList();
}

void CustomPGE::HandleUserCmdMove(pge_network::PgeNetworkConnectionHandle m_connHandleServerSide, const ElteFailMsg::MsgUserCmdMove& pktUserCmdMove)
{
    if (!getNetwork().isServer())
    {
        getConsole().EOLn("CustomPGE::%s(): client received MsgUserCmdMove, CANNOT HAPPEN!", __func__);
        assert(false);
        return;
    }

    auto it = m_mapPlayers.begin();
    while (it != m_mapPlayers.end())
    {
        if (it->second.m_connHandleServerSide == m_connHandleServerSide)
        {
            break;
        }
        it++;
    }
    
    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user with m_connHandleServerSide: %u!", __func__, m_connHandleServerSide);
        return;
    }

    const std::string& sClientUserName = it->first;

    PRREObject3D* obj = it->second.pObject3D;
    if (!obj)
    {
        getConsole().EOLn("CustomPGE::%s(): user %s doesn't have associated Object3D!", __func__, sClientUserName.c_str());
        return;
    }

    if ((pktUserCmdMove.m_dirHorizontal == ElteFailMsg::HorizontalDirection::NONE) &&
        (pktUserCmdMove.m_dirVertical == ElteFailMsg::VerticalDirection::NONE))
    {
        getConsole().EOLn("CustomPGE::%s(): user %s sent invalid cmdMove!", __func__, sClientUserName.c_str());
        return;
    }

    //getConsole().OLn("CustomPGE::%s(): user %s sent valid cmdMove", __func__, sClientUserName.c_str());
    switch (pktUserCmdMove.m_dirHorizontal)
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

    switch (pktUserCmdMove.m_dirVertical)
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
    ElteFailMsg::MsgUserUpdate::initPkt(pktOut, m_connHandleServerSide, sClientUserName, obj->getPosVec().getX(), obj->getPosVec().getY(), obj->getPosVec().getZ());
    getNetwork().getServer().SendPacketToAllClients(pktOut);
    // this msgUserUpdate should be also sent to server as self
    // maybe the SendPacketToAllClients() should be enhanced to contain packet injection for server's packet queue!
    getNetwork().getServer().getPacketQueue().push_back(pktOut);
}

void CustomPGE::HandleUserUpdate(pge_network::PgeNetworkConnectionHandle, const ElteFailMsg::MsgUserUpdate& msg)
{
    auto it = m_mapPlayers.find(msg.m_szUserName);
    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user: %s!", __func__, msg.m_szUserName);
        return;
    }

    PRREObject3D* obj = it->second.pObject3D;
    if (!obj)
    {
        getConsole().EOLn("CustomPGE::%s(): user %s doesn't have associated Object3D!", __func__, msg.m_szUserName);
        return;
    }

    obj->getPosVec().SetX(msg.m_pos.x);
    obj->getPosVec().SetY(msg.m_pos.y);
}


