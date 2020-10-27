/*
    ###################################################################################
    CustomPGE.cpp
    Customized PGE.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

#include "CustomPGE.h"

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


/** 
    Loading game content here.
*/
void CustomPGE::onGameInitialized()
{
    getConsole().OLnOI("CustomPGE::onGameInitialized()");

    getPRRE().getCamera().SetNearPlane(0.1f);
    getPRRE().getCamera().SetFarPlane(100.0f);

    PRRETexture* const tex1 = getPRRE().getTextureManager().createFromFile("gamedata\\proba128x128x24.bmp");

    /*  */
    box1 = getPRRE().getObject3DManager().createBox(1, 1, 1);
    box1->getPosVec().SetZ(2.0f);
    box1->getPosVec().SetX(1.5f);
    box1->getMaterial().getColors()[0].red    = 1.0f;
    box1->getMaterial().getColors()[0].green  = 0.0f;
    box1->getMaterial().getColors()[0].blue   = 0.0f;
    box1->getMaterial().getColors()[0].alpha  = 0.0f;
    box1->getMaterial().getColors()[9].red    = 1.0f;
    box1->getMaterial().getColors()[9].green  = 0.0f;
    box1->getMaterial().getColors()[9].blue   = 0.0f;
    box1->getMaterial().getColors()[9].alpha  = 0.0f;

    box1->getMaterial().SetTexture(tex1);
    box1->SetVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);  
    
    /*    */
    box2 = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\cube.obj");
    box2->SetVertexTransferMode(PRRE_VT_DYN_DIR_1_BY_1);
    box2->getPosVec().SetZ(3); 
    
    
    /*       
    PRREObject3D* const plane1 = getPRRE().getObject3DManager().createPlane(2, 2);
    plane1->getPosVec().SetX(0);
    plane1->getPosVec().SetZ(2);
    plane1->getMaterial().SetTexture(tex1);
    plane1->SetVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);
    */

    PRREObject3D* const snail = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail.obj");
    snail->SetScaling(0.02f);
    snail->getPosVec().SetZ(2);  

    /* */   
    PRREObject3D* snail_lm = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail_lm.obj");
    snail_lm->SetScaling(0.02f);
    snail_lm->Hide();
      
    // dealing with lightmaps ...

    if ( snail->getCount() == snail_lm->getCount() ) 
    {
        for (TPRREuint i = 0; i < snail->getCount(); i++)
        {
            PRREObject3D* const snailSub = (PRREObject3D*) snail->getAttachedAt(i);
            // assuming that snail_lm has the same subobjects and vertex count as snail
            PRREObject3D* const snailLMSub = (PRREObject3D*) snail_lm->getAttachedAt(i);
            if ( snailSub && snailLMSub )
            {
                // copying lightmap data into snail material's 2nd layer
                snailSub->getMaterial().copyFromMaterial(snailLMSub->getMaterial(), 1, 0);
                snailSub->getMaterial().SetBlendFuncs(PRRE_SRC_ALPHA, PRRE_ONE_MINUS_SRC_ALPHA, 1);
            }
        }
    }
    else
    {
        getConsole().EOLn("snail->getCount() != snail_lm->getCount(): %d != %d", snail->getCount(), snail_lm->getCount());
    }    
    
    snail->SetVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);
    //snail->SetVertexTransferMode(PRRE_VT_STA_IND_SVA_GEN);
    //snail->SetVertexTransferMode(PRRE_VT_STA_DIR_SVA_GEN);
    //snail->SetVertexTransferMode(PRRE_VT_DYN_DIR_SVA_GEN);
    //snail->SetVertexTransferMode(PRRE_VT_DYN_IND_RVA_CVA_RNG);
    //snail->SetVertexTransferMode(PRRE_VT_DYN_DIR_RVA_CVA);
    //snail->SetVertexTransferMode(PRRE_VT_DYN_DIR_1_BY_1);
    snail->SetDoubleSided(true);


    // at this point, we should be safe to delete snail_lm since object's dtor calls material's dtor which doesn't free up the textures
    // however, a mechanism is needed to be implemented to correctly handle this situation.
    // WA1: CopyFromMaterial() should hardcopy the textures also; deleting material should delete its textures too;
    // WA2: (better) textures should maintain refcount. Material deletion would decrement refcount and would effectively delete textures when refcount reaches 0.
    delete snail_lm;
    snail_lm = NULL; 

    /*PRREObject3D* snail_clone = getPRRE().getObject3DManager().createCloned(*snail);
    snail_clone->getPosVec().SetX(-1);*/

    /*         
    getPRRE().getTextureManager().SetDefaultIsoFilteringMode(PRRE_ISO_LINEAR_MIPMAP_LINEAR, PRRE_ISO_LINEAR);

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
                arenaSub->getMaterial().copyFromMaterial(arenaLMSub->getMaterial(), 1, 0);
                arenaSub->getMaterial().SetBlendFuncs(PRRE_SRC_ALPHA, PRRE_ONE_MINUS_SRC_ALPHA, 1);
            }
        }
    }
    else
    {
        getConsole().EOLn("arena->getCount() != arenaLMSub->getCount(): %d != %d", arena->getCount(), arena_lm->getCount());
    }

    arena->SetVertexTransferMode(PRRE_VT_DYN_DIR_SVA_GEN);

    delete arena_lm;
    arena_lm = NULL;
    */
    
    getPRRE().getUImanager().addText("almafaALMAFA012345óöüÓÕÛ_+", 10, 10);

    getConsole().OO();
    getConsole().OLn("");

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

    static bool bCameraLocked = false;

    const int oldmx = input.getMouse().getCursorPosX();
    const int oldmy = input.getMouse().getCursorPosY();

    input.getMouse().SetCursorPos(window.getX() + window.getWidth()/2,
                                  window.getY() + window.getHeight()/2);

    int changeX = oldmx - input.getMouse().getCursorPosX();
    int changeY = oldmy - input.getMouse().getCursorPosY();

    /*pGFX->getCamera().getTargetVec().SetY( pGFX->getCamera().getTargetVec().getY() - changeY / 10.0f );
    pGFX->getCamera().getTargetVec().SetX( pGFX->getCamera().getTargetVec().getX() - changeX / 10.0f );*/

    if ( box1 != NULL )
    {
       // box1->getAngleVec().SetY( box1->getAngleVec().getY() + 0.2f );
    }

    if ( input.getKeyboard().isKeyPressed(VK_UP) )
    {
        getPRRE().getCamera().Move(0.01f);
        getConsole().OLn("cam Z: %f", getPRRE().getCamera().getPosVec().getZ());
    }
    if ( input.getKeyboard().isKeyPressed(VK_DOWN) )
    {
        getPRRE().getCamera().Move(-0.01f);
        getConsole().OLn("cam Z: %f", getPRRE().getCamera().getPosVec().getZ());
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

    if ( input.getKeyboard().isKeyPressed(VkKeyScan('1')) )
    {
        if ( box1 != NULL )
        {
            box1->SetVisible( !box1->isVisible() );
            Sleep(200); /* to make sure key is released, avoid bouncing */
        }
    }

    if ( input.getKeyboard().isKeyPressed(VkKeyScan('2')) )
    {
        PRREObject3D* snailobj = (PRREObject3D*) getPRRE().getObject3DManager().getByFilename("gamedata\\models\\snail_proofps\\snail.obj");
        if ( snailobj != NULL )
        {
            snailobj->SetVisible( !snailobj->isVisible() );
            Sleep(200);
        }
    }

    if ( input.getKeyboard().isKeyPressed(VkKeyScan('3')) )
    {
        PRREObject3D* arenaobj = (PRREObject3D*) getPRRE().getObject3DManager().getByFilename("gamedata\\models\\arena\\arena.obj");
        if ( arenaobj != NULL )
        {
            arenaobj->SetVisible( !arenaobj->isVisible() );
            Sleep(200);
        }
    }

    if ( input.getKeyboard().isKeyPressed(VkKeyScan('w')) )
    {
        box1->getPosVec().SetZ( box1->getPosVec().getZ() + 0.01f );
    }
    if ( input.getKeyboard().isKeyPressed(VkKeyScan('s')) )
    {
        box1->getPosVec().SetZ( box1->getPosVec().getZ() - 0.01f );
    }
    if ( input.getKeyboard().isKeyPressed(VkKeyScan('a')) )
    {
        box1->getPosVec().SetX( box1->getPosVec().getX() - 0.01f );
    }
    if ( input.getKeyboard().isKeyPressed(VkKeyScan('d')) )
    {
        box1->getPosVec().SetX( box1->getPosVec().getX() + 0.01f );
    }
    if ( input.getKeyboard().isKeyPressed(VkKeyScan('i')) )
    {
        box1->getPosVec().SetY( box1->getPosVec().getY() + 0.01f );
    }
    if ( input.getKeyboard().isKeyPressed(VkKeyScan('k')) )
    {
        box1->getPosVec().SetY( box1->getPosVec().getY() - 0.01f );
    }

    // L for camera Lock
    if ( input.getKeyboard().isKeyPressed(VkKeyScan('l')) )
    {
        bCameraLocked = !bCameraLocked;
        Sleep(200);
    }

    if ( bCameraLocked )
    {
        getPRRE().getCamera().getTargetVec().Set( box1->getPosVec().getX(), box1->getPosVec().getY(), box1->getPosVec().getZ() );
    }



    std::stringstream str;
    //str << "MX1: " << changeX << "   MY1: " << changeY;
    //str << "key: " << input.getKeyboard().isKeyPressed(VK_RETURN);

    //window.SetCaption(str.str());
} // onGameRunning()


/** 
    Freeing up game content here.
    Free up everything that has been allocated in onGameInitialized() and onGameRunning().
*/
void CustomPGE::onGameDestroying()
{
    delete box1;
    box1 = NULL;
    getPRRE().getObject3DManager().DeleteAll();
} // onGameRunning()


// ############################### PRIVATE ###############################

