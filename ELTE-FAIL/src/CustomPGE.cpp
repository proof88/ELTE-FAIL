/*
    ###################################################################################
    CustomPGE.cpp
    Customized PGE.
    Made by PR00F88
    ###################################################################################
*/

#include "CustomPGE.h"

#include <cassert>
#include <filesystem>  // requires cpp17
#include <set>
#include <sstream>

#include "../../../PGE/PGE/Pure/include/external/PureUiManager.h"
#include "../../../PGE/PGE/Pure/include/external/Display/PureWindow.h"
#include "../../../PGE/PGE/Pure/include/external/PureCamera.h"
#include "../../../Console/CConsole/src/CConsole.h"


static constexpr char* CVAR_CL_SERVER_IP = "cl_server_ip";


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
bool CustomPGE::onGameInitializing()
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

    // Network logs
    getConsole().SetLoggingState("PgeGnsWrapper", true);
    getConsole().SetLoggingState(getNetwork().getLoggerModuleName(), true);
    getConsole().SetLoggingState(getNetwork().getServer().getLoggerModuleName(), true);
    getConsole().SetLoggingState(getNetwork().getClient().getLoggerModuleName(), true);

    // Misc engine logs
    getConsole().SetLoggingState("PureWindow", true);

    // Turn everything on for development only
    getConsole().SetLoggingState("4LLM0DUL3S", true);

    // we need PGE::runGame() invoke EVERYTHING even when window is NOT active, and we will decide in onGameRunning() what NOT to do if window is inactive
    SetInactiveLikeActive(true);

    return true;
}


/** 
    Loading game content here.
*/
bool CustomPGE::onGameInitialized()
{
    getConsole().OLnOI("CustomPGE::onGameInitialized()");

    // Dont want to see logs of loading of resources cause I'm debugging network now
    getConsole().SetLoggingState("4LLM0DUL3S", false);

    getPure().getCamera().SetNearPlane(0.1f);
    getPure().getCamera().SetFarPlane(100.0f);

    getPure().getScreen().setVSyncEnabled(true);

    PureTexture* const tex1 = getPure().getTextureManager().createFromFile("gamedata\\proba128x128x24.bmp");

    {   // create box object internally
        m_box1 = getPure().getObject3DManager().createBox(1, 1, 1);
        m_box1->getPosVec().SetZ(2.0f);
        m_box1->getPosVec().SetX(1.5f);
        m_box1->SetOccluder(true);
        m_box1->SetOcclusionTested(false);

        // since a while this vertex coloring is not working
        m_box1->getMaterial().getColors()[0].red = 1.0f;
        m_box1->getMaterial().getColors()[0].green = 0.0f;
        m_box1->getMaterial().getColors()[0].blue = 0.0f;
        m_box1->getMaterial().getColors()[0].alpha = 0.0f;
        m_box1->getMaterial().getColors()[9].red = 1.0f;
        m_box1->getMaterial().getColors()[9].green = 0.0f;
        m_box1->getMaterial().getColors()[9].blue = 0.0f;
        m_box1->getMaterial().getColors()[9].alpha = 0.0f;

        m_box1->getMaterial().setTexture(tex1);
        m_box1->setVertexTransferMode(PURE_VT_DYN_IND_SVA_GEN);
    }
    
    {   // load box object from file
        m_box2 = getPure().getObject3DManager().createFromFile("gamedata\\models\\cube.obj");
        m_box2->setVertexTransferMode(PURE_VT_DYN_DIR_1_BY_1);
        m_box2->getPosVec().SetZ(4);
    }
    
    /*       
    PureObject3D* const plane1 = getPure().getObject3DManager().createPlane(2, 2);
    plane1->getPosVec().SetX(0);
    plane1->getPosVec().SetZ(2);
    plane1->getMaterial().setTexture(tex1);
    plane1->setVertexTransferMode(PURE_VT_DYN_IND_SVA_GEN);
    */

    {   // snail
        PureObject3D* const snail = getPure().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail.obj");
        snail->SetScaling(0.02f);
        snail->getPosVec().SetX(-1.5f);
        snail->getPosVec().SetZ(2.7f);

        PureObject3D* snail_lm = getPure().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail_lm.obj");
        snail_lm->SetScaling(0.02f);
        snail_lm->Hide();

        // dealing with lightmaps ...
        if (snail->getCount() == snail_lm->getCount())
        {
            for (TPureInt i = 0; i < snail->getCount(); i++)
            {
                PureObject3D* const snailSub = (PureObject3D*)snail->getAttachedAt(i);
                // assuming that snail_lm has the same subobjects and vertex count as snail
                PureObject3D* const snailLMSub = (PureObject3D*)snail_lm->getAttachedAt(i);
                if (snailSub && snailLMSub)
                {
                    // copying lightmap data into snail material's 2nd layer
                    snailSub->getMaterial(false).copyFromMaterial(snailLMSub->getMaterial(false), 1, 0);
                    snailSub->getMaterial(false).setBlendFuncs(PURE_SRC_ALPHA, PURE_ONE_MINUS_SRC_ALPHA, 1);
                }
            }
        }
        else
        {
            getConsole().EOLn("snail->getCount() != snail_lm->getCount(): %d != %d", snail->getCount(), snail_lm->getCount());
        }

        snail->setVertexTransferMode(PURE_VT_DYN_IND_SVA_GEN);
        snail->SetDoubleSided(true);

        // at this point, we should be safe to delete snail_lm since object's dtor calls material's dtor which doesn't free up the textures
        // however, a mechanism is needed to be implemented to correctly handle this situation.
        // WA1: CopyFromMaterial() should hardcopy the textures also; deleting material should delete its textures too;
        // WA2: (better) textures should maintain refcount. Material deletion would decrement refcount and would effectively delete textures when refcount reaches 0.
        delete snail_lm;
    }

    /*
    PureObject3D* snail_clone = getPure().getObject3DManager().createCloned(*snail);
    snail_clone->getPosVec().SetX(-1);
    snail->SetOcclusionTested(false);
    */

    {   // arena
        getPure().getTextureManager().setDefaultIsoFilteringMode(PURE_ISO_LINEAR_MIPMAP_LINEAR, PURE_ISO_LINEAR);

        PureObject3D* const arena = getPure().getObject3DManager().createFromFile("gamedata\\models\\arena\\arena.obj");
        arena->SetScaling(0.002f);
        arena->getPosVec().SetZ(2.f);
        arena->getPosVec().SetY(-1.5f);

        PureObject3D* arena_lm = getPure().getObject3DManager().createFromFile("gamedata\\models\\arena\\arena_lm.obj");
        arena_lm->SetScaling(0.02f);
        arena_lm->Hide();

        // dealing with lightmaps ...
        if (arena->getCount() == arena_lm->getCount())
        {
            for (TPureInt i = 0; i < arena->getCount(); i++)
            {
                PureObject3D* const arenaSub = (PureObject3D*)arena->getAttachedAt(i);
                // assuming that arena_lm has the same subobjects and vertex count as arena
                PureObject3D* const arenaLMSub = (PureObject3D*)arena_lm->getAttachedAt(i);
                if (arenaSub && arenaLMSub)
                {
                    // copying lightmap data into snail material's 2nd layer
                    arenaSub->getMaterial(false).copyFromMaterial(arenaLMSub->getMaterial(false), 1, 0);
                    arenaSub->getMaterial(false).setBlendFuncs(PURE_SRC_ALPHA, PURE_ONE_MINUS_SRC_ALPHA, 1);
                }
            }
        }
        else
        {
            getConsole().EOLn("arena->getCount() != arenaLMSub->getCount(): %d != %d", arena->getCount(), arena_lm->getCount());
        }

        arena->setVertexTransferMode(PURE_VT_DYN_DIR_SVA_GEN);

        delete arena_lm;
    }

    // Gather some trollface pictures for the players
    // Building this set up initially, each face is removed from the set when assigned to a player, so
    // all players will have unique face texture assigned.
    for (const auto& entry : std::filesystem::directory_iterator("gamedata/trollfaces/"))
    {
        if ((entry.path().extension().string() == ".bmp"))
        {
            m_trollFaces.insert(entry.path().string());
        }
    }
    getConsole().OLn("%s() Server parsed %d trollfaces", __func__, m_trollFaces.size());
    
    getPure().getUImanager().textPermanentLegacy("almafaALMAFA012345������_+", 10, 10);

    getConsole().OO();
    getConsole().OLn("");

    getPure().WriteList();

    // We tell the names of our app messages to the network engine so it can properly log message stats with message names
    for (const auto& msgAppId2StringPair : elte_fail::MapMsgAppId2String)
    {
        getNetwork().getServerClientInstance()->getMsgAppId2StringMap().insert(
            { static_cast<pge_network::MsgApp::TMsgId>(msgAppId2StringPair.msgId),
              std::string(msgAppId2StringPair.zstring) }
        );
    }

    if (getNetwork().isServer())
    {
        getNetwork().getServer().getAllowListedAppMessages().insert(static_cast<pge_network::MsgApp::TMsgId>(elte_fail::MsgUserCmdMoveFromClient::id));

        if (!getNetwork().getServer().startListening())
        {
            PGE::showErrorDialog("Server has FAILED to start listening!");
            assert(false);
        }
    }
    else
    {
        // MsgUserSetupFromServer is also processed by server, but it injects this pkt into its own queue when needed.
        // MsgUserSetupFromServer MUST NOT be received by server over network!
        // MsgUserSetupFromServer is received only by clients over network!
        getNetwork().getClient().getAllowListedAppMessages().insert(static_cast<pge_network::MsgApp::TMsgId>(elte_fail::MsgUserSetupFromServer::id));

        // MsgUserUpdateFromServer is also processed by server, but it injects this pkt into its own queue when needed.
        // MsgUserUpdateFromServer MUST NOT be received by server over network!
        // MsgUserUpdateFromServer is received only by clients over network!
        getNetwork().getClient().getAllowListedAppMessages().insert(static_cast<pge_network::MsgApp::TMsgId>(elte_fail::MsgUserUpdateFromServer::id));

        std::string sIp = "127.0.0.1";
        if (!getConfigProfiles().getVars()[CVAR_CL_SERVER_IP].getAsString().empty())
        {
            sIp = getConfigProfiles().getVars()[CVAR_CL_SERVER_IP].getAsString();
            getConsole().OLn("IP from config: %s", sIp.c_str());
        }
        // TODO: log level override support: getConsole().SetLoggingState(sTrimmedLine.c_str(), true);

        if (!getNetwork().getClient().connectToServer(sIp))
        {
            PGE::showErrorDialog("Client has FAILED to establish connection to the server!");
            assert(false);
        }
    }

    return true;
} // onGameInitialized()


/** 
    Game logic here.
    DO NOT make any unnecessary operations here, as this function must complete below 16 msecs to keep 60 fps!
    Avoid dynamic memory allocations as well! Make sure you use a preallocated memory pool!
*/
void CustomPGE::onGameRunning()
{
    PureWindow& window = getPure().getWindow();

    static bool bCameraLocked = true;

    if (window.isActive())
    {
        getInput().getMouse().SetCursorPos(
            window.getX() + window.getWidth() / 2,
            window.getY() + window.getHeight() / 2);
    }

    if (!getNetwork().isServer())
    {
        getPure().getUImanager().textTemporalLegacy(
            "Ping: " + std::to_string(getNetwork().getClient().getPing(true)) + " ms",
            10, 50);
        getPure().getUImanager().textTemporalLegacy(
            "Quality: local: " + std::to_string(getNetwork().getClient().getQualityLocal(false)) +
            "; remote: " + std::to_string(getNetwork().getClient().getQualityRemote(false)),
            10, 70);
        getPure().getUImanager().textTemporalLegacy(
            "Tx Speed: " + std::to_string(getNetwork().getClient().getTxByteRate(false)) +
            " Bps; Rx Speed: " + std::to_string(getNetwork().getClient().getRxByteRate(false)) + " Bps",
            10, 90);
        getPure().getUImanager().textTemporalLegacy(
            "Internal Queue Time: " + std::to_string(getNetwork().getClient().getInternalQueueTimeUSecs(false)) + " us",
            10, 110);
    }

    if (m_box1 != NULL )
    {
        m_box1->getAngleVec().SetY(m_box1->getAngleVec().getY() + 0.2f );
    }

    if (window.isActive())
    {
        if (getInput().getKeyboard().isKeyPressed(VK_ESCAPE))
        {
            window.Close();
        }

        if (getInput().getKeyboard().isKeyPressed(VK_UP))
        {
            getPure().getCamera().Move(0.01f);
        }
        if (getInput().getKeyboard().isKeyPressed(VK_DOWN))
        {
            getPure().getCamera().Move(-0.01f);
        }
        if (getInput().getKeyboard().isKeyPressed(VK_LEFT))
        {
            getPure().getCamera().Strafe(-0.01f);
        }
        if (getInput().getKeyboard().isKeyPressed(VK_RIGHT))
        {
            getPure().getCamera().Strafe(0.01f);
        }
        if (getInput().getKeyboard().isKeyPressed(VK_SPACE))
        {
            getPure().getCamera().Elevate(0.01f);
        }
        if (getInput().getKeyboard().isKeyPressed(VK_CONTROL))
        {
            getPure().getCamera().Elevate(-0.01f);
        }

        if (getInput().getKeyboard().isKeyPressed((unsigned char)VkKeyScan('1')))
        {
            if (m_box1 != NULL)
            {
                m_box1->SetRenderingAllowed(!m_box1->isRenderingAllowed());
                Sleep(200); /* to make sure key is released, avoid bouncing */
            }
        }

        if (getInput().getKeyboard().isKeyPressed((unsigned char)VkKeyScan('2')))
        {
            PureObject3D* snailobj = (PureObject3D*)getPure().getObject3DManager().getByFilename("gamedata\\models\\snail_proofps\\snail.obj");
            if (snailobj != NULL)
            {
                snailobj->SetRenderingAllowed(!snailobj->isRenderingAllowed());
                Sleep(200);
            }
        }

        if (getInput().getKeyboard().isKeyPressed((unsigned char)VkKeyScan('3')))
        {
            PureObject3D* arenaobj = (PureObject3D*)getPure().getObject3DManager().getByFilename("gamedata\\models\\arena\\arena.obj");
            if (arenaobj != NULL)
            {
                arenaobj->SetRenderingAllowed(!arenaobj->isRenderingAllowed());
                Sleep(200);
            }
        }

        elte_fail::HorizontalDirection horDir = elte_fail::HorizontalDirection::NONE;
        elte_fail::VerticalDirection verDir = elte_fail::VerticalDirection::NONE;
        if (getInput().getKeyboard().isKeyPressed((unsigned char)VkKeyScan('w')))
        {
            verDir = elte_fail::VerticalDirection::UP;
        }
        if (getInput().getKeyboard().isKeyPressed((unsigned char)VkKeyScan('s')))
        {
            verDir = elte_fail::VerticalDirection::DOWN;
        }
        if (getInput().getKeyboard().isKeyPressed((unsigned char)VkKeyScan('a')))
        {
            horDir = elte_fail::HorizontalDirection::LEFT;
        }
        if (getInput().getKeyboard().isKeyPressed((unsigned char)VkKeyScan('d')))
        {
            horDir = elte_fail::HorizontalDirection::RIGHT;
        }

        if ((horDir != elte_fail::HorizontalDirection::NONE) || (verDir != elte_fail::VerticalDirection::NONE))
        {
            pge_network::PgePacket pkt;
            if (elte_fail::MsgUserCmdMoveFromClient::initPkt(pkt, horDir, verDir))
            {
                // instead of using sendToServer() of getClient() or getServer() instances, we use the sendToServer() of
                // their common interface which always points to the initialized instance, which is either client or server.
                getNetwork().getServerClientInstance()->send(pkt);
            }
            else
            {
                getConsole().EOLn("PRooFPSddPGE::%s(): initPkt() FAILED at line %d!", __func__, __LINE__);
                assert(false);
            }
        }

        // L for camera Lock
        if (getInput().getKeyboard().isKeyPressed((unsigned char)VkKeyScan('l')))
        {
            bCameraLocked = !bCameraLocked;
            Sleep(200);
        }
    }

    if ( bCameraLocked )
    {
        //getPure().getCamera().getTargetVec().Set( box1->getPosVec().getX(), box1->getPosVec().getY(), box1->getPosVec().getZ() );

        const auto it = m_mapPlayers.find(m_sUserName);
        if (it != m_mapPlayers.end())
        {
            PureObject3D* const pPlayerObj = it->second.m_pObject3D;
            if (pPlayerObj)
            {
                getPure().getCamera().getTargetVec().Set(pPlayerObj->getPosVec().getX(), pPlayerObj->getPosVec().getY(), pPlayerObj->getPosVec().getZ());
            }
        }
    }

    std::stringstream str;
    //str << "MX1: " << changeX << "   MY1: " << changeY;
    //str << "key: " << getInput().getKeyboard().isKeyPressed(VK_RETURN);

    //window.SetCaption(str.str());
} // onGameRunning()


/**
    Called when a new network packet is received.

    @return True on successful packet handling, false on serious error that should result in terminating the application.
*/
bool CustomPGE::onPacketReceived(const pge_network::PgePacket& pkt)
{
    const pge_network::PgePktId& pgePktId = pge_network::PgePacket::getPacketId(pkt);
    switch (pgePktId)
    {
    case pge_network::MsgUserConnectedServerSelf::id:
        return handleUserConnected(pge_network::PgePacket::getServerSideConnectionHandle(pkt), pge_network::PgePacket::getMessageAsUserConnected(pkt));
    case pge_network::MsgUserDisconnectedFromServer::id:
        return handleUserDisconnected(pge_network::PgePacket::getServerSideConnectionHandle(pkt), pge_network::PgePacket::getMessageAsUserDisconnected(pkt));
    case pge_network::MsgApp::id:
    {
        // for now we support only 1 app msg per pkt
        assert(pge_network::PgePacket::getMessageAppCount(pkt) == 1);
        assert(pge_network::PgePacket::getMessageAppsTotalActualLengthBytes(pkt) > 0);  // for now we dont have empty messages

        // TODO: here we will need to iterate over all app msg but for now there is only 1 inside!

        const elte_fail::ElteFailMsgId& eltefailAppMsgId = static_cast<elte_fail::ElteFailMsgId>(pge_network::PgePacket::getMsgAppIdFromPkt(pkt));

        switch (eltefailAppMsgId)
        {
        case elte_fail::MsgUserSetupFromServer::id:
            return handleUserSetup(
                pge_network::PgePacket::getServerSideConnectionHandle(pkt),
                pge_network::PgePacket::getMsgAppDataFromPkt<elte_fail::MsgUserSetupFromServer>(pkt));
        case elte_fail::MsgUserCmdMoveFromClient::id:
            return handleUserCmdMove(
                pge_network::PgePacket::getServerSideConnectionHandle(pkt),
                pge_network::PgePacket::getMsgAppDataFromPkt<elte_fail::MsgUserCmdMoveFromClient>(pkt));
        case elte_fail::MsgUserUpdateFromServer::id:
            return handleUserUpdate(
                pge_network::PgePacket::getServerSideConnectionHandle(pkt),
                pge_network::PgePacket::getMsgAppDataFromPkt<elte_fail::MsgUserUpdateFromServer>(pkt));
        default:
            getConsole().EOLn("CustomPGE::%s(): unknown msgId %u in MsgAppArea!", __func__, eltefailAppMsgId);
        }
        break;
    }
    default:
        getConsole().EOLn("CustomPGE::%s(): unknown pktId %u!", __func__, pgePktId);
    }
    return false;
}


/** 
    Freeing up game content here.
    Free up everything that has been allocated in onGameInitialized() and onGameRunning().
*/
void CustomPGE::onGameDestroying()
{
    m_mapPlayers.clear();

    delete m_box1;
    m_box1 = NULL;
    delete m_box2;
    m_box2 = NULL;
    getPure().getObject3DManager().DeleteAll();

    getConsole().Deinitialize();
} // onGameRunning()


// ############################### PRIVATE ###############################


void CustomPGE::genUniqueUserName(char szNewUserName[elte_fail::MsgUserSetupFromServer::nUserNameBufferLength]) const
{
    bool found = false;
    do
    {
        sprintf_s(szNewUserName, elte_fail::MsgUserSetupFromServer::nUserNameBufferLength, "User%d", 10000 + (rand() % 100000));
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
    getConsole().OO();
}

bool CustomPGE::handleUserSetup(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const elte_fail::MsgUserSetupFromServer& msg)
{
    if ((strnlen(msg.m_szUserName, elte_fail::MsgUserSetupFromServer::nUserNameBufferLength) > 0) && (m_mapPlayers.end() != m_mapPlayers.find(msg.m_szUserName)))
    {
        getConsole().EOLn("CustomPGE::%s(): cannot happen: user %s (connHandleServerSide: %u) is already present in players list!",
            __func__, msg.m_szUserName, connHandleServerSide);
        assert(false);
        return false;
    }

    if (msg.m_bCurrentClient)
    {
        getConsole().OLn("CustomPGE::%s(): this is me, my name is %s, connHandleServerSide: %u, my IP: %s",
            __func__, msg.m_szUserName, connHandleServerSide, msg.m_szIpAddress);
        // store our username so we can refer to it anytime later
        m_sUserName = msg.m_szUserName;

        if (getNetwork().isServer())
        {
            getPure().getUImanager().textPermanentLegacy("Server, User name: " + m_sUserName, 10, 30);
        }
        else
        {
            getPure().getUImanager().textPermanentLegacy("Client, User name: " + m_sUserName + "; IP: " + msg.m_szIpAddress, 10, 30);
        }
    }
    else
    {
        getConsole().OLn("CustomPGE::%s(): new user %s (connHandleServerSide: %u; IP: %s) connected",
            __func__, msg.m_szUserName, connHandleServerSide, msg.m_szIpAddress);
    }

    // insert user into map using wacky syntax
    m_mapPlayers[msg.m_szUserName];
    m_mapPlayers[msg.m_szUserName].m_sTrollface = msg.m_szTrollfaceTex;
    m_mapPlayers[msg.m_szUserName].m_connHandleServerSide = connHandleServerSide;
    m_mapPlayers[msg.m_szUserName].m_sIpAddress = msg.m_szIpAddress;

    PureObject3D* const plane = getPure().getObject3DManager().createPlane(0.5f, 0.5f);
    if (!plane)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to create object for user %s!", __func__, msg.m_szUserName);
        return false;
    }

    plane->SetDoubleSided(true);
    plane->getPosVec().SetX(0);
    plane->getPosVec().SetZ(2);

    if (!m_mapPlayers[msg.m_szUserName].m_sTrollface.empty())
    {
        PureTexture* const tex = getPure().getTextureManager().createFromFile(m_mapPlayers[msg.m_szUserName].m_sTrollface.c_str());
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

    plane->setVertexModifyingHabit(PURE_VMOD_STATIC);
    plane->setVertexReferencingMode(PURE_VREF_INDEXED);

    m_mapPlayers[msg.m_szUserName].m_pObject3D = plane;

    getNetwork().WriteList();
    WritePlayerList();

    return true;
}

bool CustomPGE::handleUserConnected(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const pge_network::MsgUserConnectedServerSelf& msg)
{
    if (!getNetwork().isServer())
    {
        getConsole().EOLn("CustomPGE::%s(): client received MsgUserConnectedServerSelf, CANNOT HAPPEN!", __func__);
        assert(false);
        return false;
    }

    const char* szConnectedUserName = nullptr;
    std::string sTrollface;

    if (m_trollFaces.size() > 0)
    {
        sTrollface = *m_trollFaces.begin();
        m_trollFaces.erase(m_trollFaces.begin());
    }
    else
    {
        CConsole::getConsoleInstance("PgeGnsWrapper").EOLn("%s: SERVER No more trollfaces left for user with connHandle %u", __func__, connHandleServerSide);
    }

    if (msg.m_bCurrentClient)
    {
        // server is processing its own birth
        if (m_mapPlayers.size() == 0)
        {
            char szNewUserName[elte_fail::MsgUserSetupFromServer::nUserNameBufferLength];
            genUniqueUserName(szNewUserName);
            getConsole().OLn("CustomPGE::%s(): first (local) user %s connected and I'm server, so this is me (connHandleServerSide: %u)",
                __func__, szNewUserName, connHandleServerSide);

            szConnectedUserName = szNewUserName;

            pge_network::PgePacket newPktSetup;
            if (elte_fail::MsgUserSetupFromServer::initPkt(newPktSetup, connHandleServerSide, true, szConnectedUserName, sTrollface, msg.m_szIpAddress))
            {
                // server injects this msg to self so resources for player will be allocated
                getNetwork().getServer().send(newPktSetup);
            }
            else
            {
                getConsole().EOLn("PRooFPSddPGE::%s(): initPkt() FAILED at line %d!", __func__, __LINE__);
                assert(false);
            }
        }
        else
        {
            // cannot happen
            getConsole().EOLn("CustomPGE::%s(): user (connHandleServerSide: %u) connected with bCurrentClient as true but it is not me, CANNOT HAPPEN!", 
               __func__, connHandleServerSide);
            assert(false);
            return false;
        }
    }
    else
    {
        // server is processing another user's birth
        if (m_mapPlayers.empty())
        {
            // cannot happen because at least the user of the server should be in the map!
            // this should happen only if we are dedicated server but currently only listen-server is supported!
            getConsole().EOLn("CustomPGE::%s(): non-server user (connHandleServerSide: %u) connected but map of players is still empty, CANNOT HAPPEN!",
                __func__, connHandleServerSide);
            assert(false);
            return false;
        }

        char szNewUserName[elte_fail::MsgUserSetupFromServer::nUserNameBufferLength];
        genUniqueUserName(szNewUserName);
        szConnectedUserName = szNewUserName;
        getConsole().OLn("CustomPGE::%s(): new remote user %s (connHandleServerSide: %u) connected (from %s) and I'm server",
            __func__, szConnectedUserName, connHandleServerSide, msg.m_szIpAddress);

        pge_network::PgePacket newPktSetup;
        if (!elte_fail::MsgUserSetupFromServer::initPkt(newPktSetup, connHandleServerSide, false, szConnectedUserName, sTrollface, msg.m_szIpAddress))
        {
            getConsole().EOLn("PRooFPSddPGE::%s(): initPkt() FAILED at line %d!", __func__, __LINE__);
            assert(false);
            return false;
        }

        // server injects this msg to self so resources for player will be allocated
        getNetwork().getServer().send(newPktSetup);

        // inform all other clients about this new user
        getNetwork().getServer().sendToAllClientsExcept(newPktSetup, connHandleServerSide);

        // now we send this msg to the client with this bool flag set so client will know it is their connect
        elte_fail::MsgUserSetupFromServer& msgUserSetup = pge_network::PgePacket::getMsgAppDataFromPkt<elte_fail::MsgUserSetupFromServer>(newPktSetup);
        msgUserSetup.m_bCurrentClient = true;
        getNetwork().getServer().send(newPktSetup, connHandleServerSide);

        // we also send as many MsgUserSetupFromServer pkts to the client as the number of already connected players,
        // otherwise client won't know about them, so this way the client will detect them as newly connected users;
        // we also send MsgUserUpdateFromServer about each player so new client will immediately have their positions updated.
        pge_network::PgePacket newPktUserUpdate;
        for (const auto& it : m_mapPlayers)
        {
            if (!elte_fail::MsgUserSetupFromServer::initPkt(
                newPktSetup,
                it.second.m_connHandleServerSide,
                false,
                it.first, it.second.m_sTrollface, it.second.m_sIpAddress))
            {
                getConsole().EOLn("PRooFPSddPGE::%s(): initPkt() FAILED at line %d!", __func__, __LINE__);
                assert(false);
                continue;
            }
            getNetwork().getServer().send(newPktSetup, connHandleServerSide);
            
            if (!elte_fail::MsgUserUpdateFromServer::initPkt(
                newPktUserUpdate,
                it.second.m_connHandleServerSide,
                it.second.m_pObject3D->getPosVec().getX(), it.second.m_pObject3D->getPosVec().getY(), it.second.m_pObject3D->getPosVec().getZ()))
            {
                getConsole().EOLn("PRooFPSddPGE::%s(): initPkt() FAILED at line %d!", __func__, __LINE__);
                assert(false);
                continue;
            }
            getNetwork().getServer().send(newPktUserUpdate, connHandleServerSide);
        }
    }

    return true;
}

bool CustomPGE::handleUserDisconnected(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const pge_network::MsgUserDisconnectedFromServer&)
{
    auto it = m_mapPlayers.begin();
    while (it != m_mapPlayers.end())
    {
        if (it->second.m_connHandleServerSide == connHandleServerSide)
        {
            break;
        }
        it++;
    }

    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user with connHandleServerSide: %u!", __func__, connHandleServerSide);
        assert(false); // in debug mode, try to understand this scenario
        return true; // in release mode, dont terminate
    }

    const std::string& sClientUserName = it->first;

    if (getNetwork().isServer())
    {
        getConsole().OLn("CustomPGE::%s(): user %s disconnected and I'm server", __func__, sClientUserName.c_str());
        m_trollFaces.insert(it->second.m_sTrollface);  // re-insert the unneeded trollface texture into the set
    }
    else
    {
        getConsole().OLn("CustomPGE::%s(): user %s disconnected and I'm client", __func__, sClientUserName.c_str());
    }

    if (it->second.m_pObject3D)
    {
        delete it->second.m_pObject3D;  // yes, dtor will remove this from its Object3DManager too!
    }

    m_mapPlayers.erase(it);

    getNetwork().WriteList();
    WritePlayerList();

    return true;
}

bool CustomPGE::handleUserCmdMove(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const elte_fail::MsgUserCmdMoveFromClient& pktUserCmdMove)
{
    if (!getNetwork().isServer())
    {
        getConsole().EOLn("CustomPGE::%s(): client received MsgUserCmdMoveFromClient, CANNOT HAPPEN!", __func__);
        assert(false);
        return false;
    }

    auto it = m_mapPlayers.begin();
    while (it != m_mapPlayers.end())
    {
        if (it->second.m_connHandleServerSide == connHandleServerSide)
        {
            break;
        }
        it++;
    }
    
    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user with connHandleServerSide: %u!", __func__, connHandleServerSide);
        assert(false);  // in debug mode this terminates server
        return true;    // in release mode, we dont terminate the server, just silently ignore
    }

    const std::string& sClientUserName = it->first;

    PureObject3D* obj = it->second.m_pObject3D;
    if (!obj)
    {
        getConsole().EOLn("CustomPGE::%s(): user %s doesn't have associated Object3D!", __func__, sClientUserName.c_str());
        return false;
    }

    if ((pktUserCmdMove.m_dirHorizontal == elte_fail::HorizontalDirection::NONE) &&
        (pktUserCmdMove.m_dirVertical == elte_fail::VerticalDirection::NONE))
    {
        getConsole().EOLn("CustomPGE::%s(): user %s sent invalid cmdMove!", __func__, sClientUserName.c_str());
        assert(false);  // in debug mode this terminates server
        return false;   // in release mode, we dont terminate the server, just silently ignore
        // TODO: I might disconnect this client!
    }

    //getConsole().OLn("CustomPGE::%s(): user %s sent valid cmdMove", __func__, sClientUserName.c_str());
    switch (pktUserCmdMove.m_dirHorizontal)
    {
    case elte_fail::HorizontalDirection::LEFT:
        obj->getPosVec().SetX( obj->getPosVec().getX() - 0.01f );
        break;
    case elte_fail::HorizontalDirection::RIGHT:
        obj->getPosVec().SetX(obj->getPosVec().getX() + 0.01f);
        break;
    default: /* no-op */
        break;
    }

    switch (pktUserCmdMove.m_dirVertical)
    {
    case elte_fail::VerticalDirection::DOWN:
        obj->getPosVec().SetY(obj->getPosVec().getY() - 0.01f);
        break;
    case elte_fail::VerticalDirection::UP:
        obj->getPosVec().SetY(obj->getPosVec().getY() + 0.01f);
        break;
    default: /* no-op */
        break;
    }

    pge_network::PgePacket pktOut;
    if (elte_fail::MsgUserUpdateFromServer::initPkt(pktOut, connHandleServerSide, obj->getPosVec().getX(), obj->getPosVec().getY(), obj->getPosVec().getZ()))
    {
        getNetwork().getServer().sendToAll(pktOut);
    }
    else
    {
        getConsole().EOLn("PRooFPSddPGE::%s(): initPkt() FAILED at line %d!", __func__, __LINE__);
        return false;
    }

    return true;
}

bool CustomPGE::handleUserUpdate(pge_network::PgeNetworkConnectionHandle connHandleServerSide, const elte_fail::MsgUserUpdateFromServer& msg)
{
    auto it = m_mapPlayers.begin();
    while (it != m_mapPlayers.end())
    {
        if (it->second.m_connHandleServerSide == connHandleServerSide)
        {
            break;
        }
        it++;
    }

    if (m_mapPlayers.end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user with connHandleServerSide: %u!", __func__, connHandleServerSide);
        return true;  // might NOT be fatal error in some circumstances, although I cannot think about any, but dont terminate the app for this ...
    }

    PureObject3D* obj = it->second.m_pObject3D;
    if (!obj)
    {
        getConsole().EOLn("CustomPGE::%s(): user %s doesn't have associated Object3D!", __func__, it->first.c_str());
        return false;
    }

    obj->getPosVec().SetX(msg.m_pos.x);
    obj->getPosVec().SetY(msg.m_pos.y);

    return true;
}

