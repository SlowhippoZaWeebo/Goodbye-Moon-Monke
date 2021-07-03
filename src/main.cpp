#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "custom-types/shared/register.hpp"
#include "UnityEngine/Rigidbody.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/CapsuleCollider.hpp"
#include "UnityEngine/SphereCollider.hpp"
#include "UnityEngine/GameObject.hpp"
#include "GorillaLocomotion/Player.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/utils-functions.h"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/ForceMode.hpp"
#include "UnityEngine/Transform.hpp"
#include "GoodbyeMoonMonkeWatchView.hpp"
#include "config.hpp"
#include "monkecomputer/shared/GorillaUI.hpp"
#include "monkecomputer/shared/Register.hpp"
#include "custom-types/shared/register.hpp"

ModInfo modInfo;

#define INFO(value...) getLogger().info(value)
#define ERROR(value...) getLogger().error(value)

using namespace UnityEngine;
using namespace UnityEngine::XR;
using namespace GorillaLocomotion;

Logger& getLogger()
{
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

bool isRoom = false;
bool enabled = false;
bool lowGravModeEnabled = false;
float thrust = config.power * 1000.0;

MAKE_HOOK_OFFSETLESS(PhotonNetworkController_OnJoinedRoom, void, Il2CppObject* self)
{
    PhotonNetworkController_OnJoinedRoom(self);

    Il2CppObject* currentRoom = CRASH_UNLESS(il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "get_CurrentRoom"));

    if (currentRoom)
    {
        isRoom = !CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(currentRoom, "get_IsVisible"));
    }
    else isRoom = true;

}

void UpdateButton()
{

    using namespace GlobalNamespace;
    bool AInput = false;
	bool XInput = false;
    bool leftGripInput = false;
    bool rightGripInput = false;
	//Remove whatever inputs you dont need
    AInput = OVRInput::Get(OVRInput::Button::One, OVRInput::Controller::RTouch);
    XInput = OVRInput::Get(OVRInput::Button::One, OVRInput::Controller::LTouch);
    leftGripInput = OVRInput::Get(OVRInput::Button::PrimaryHandTrigger, OVRInput::Controller::LTouch);
    rightGripInput = OVRInput::Get(OVRInput::Button::PrimaryHandTrigger, OVRInput::Controller::RTouch);

    if (isRoom && config.enabled)
    {
        // If you want just a single button press then do if (AInput) replace AInput with whatever button you want
        if (AInput && rightGripInput || XInput && leftGripInput)
        {
            INFO("Flipping off");
            enabled = true;
        }
        // For checking if a specific button is not being pressed then do if (!AInput)
        else
        {
            INFO("Not flipping off");
            enabled = false;
        }
    }
    else return;
}

    #include "GlobalNamespace/GorillaTagManager.hpp"

    MAKE_HOOK_OFFSETLESS(GorillaTagManager_Update, void, GlobalNamespace::GorillaTagManager* self) {
        using namespace GlobalNamespace;
        using namespace GorillaLocomotion;

        Player* playerInstance = Player::get_Instance();
        if(playerInstance == nullptr) return;

        Rigidbody* playerPhysics = playerInstance->playerRigidBody;
        if(playerPhysics == nullptr) return;

        GameObject* playerGameObject = playerPhysics->get_gameObject();
        if(playerGameObject == nullptr) return;

        if(isRoom && config.enabled) {
            if(enabled) {
                if(lowGravModeEnabled) {
                    lowGravModeEnabled = false;
                    playerPhysics->set_useGravity(false);
                    playerPhysics->AddForce(Vector3::get_up() * thrust);
                } else if(!lowGravModeEnabled){
                    lowGravModeEnabled = true;
                    playerPhysics->set_useGravity(true);
                }
            } else {
                playerPhysics->set_useGravity(true);
            }
        } else {
            playerPhysics->set_useGravity(true);
        }
    }

MAKE_HOOK_OFFSETLESS(Player_Update, void, Il2CppObject* self)
{
    using namespace UnityEngine;
    using namespace GlobalNamespace;
    INFO("player update was called");
    Player_Update(self);
    UpdateButton();
    OVRInput::Update();
    OVRInput::FixedUpdate();
}

extern "C" void setup(ModInfo& info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

}

extern "C" void load()
{
    getLogger().info("Loading mod...");

    GorillaUI::Init();

    INSTALL_HOOK_OFFSETLESS(getLogger(), PhotonNetworkController_OnJoinedRoom, il2cpp_utils::FindMethodUnsafe("", "PhotonNetworkController", "OnJoinedRoom", 0));
	INSTALL_HOOK_OFFSETLESS(getLogger(), Player_Update, il2cpp_utils::FindMethodUnsafe("GorillaLocomotion", "Player", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), GorillaTagManager_Update, il2cpp_utils::FindMethodUnsafe("", "GorillaTagManager", "Update", 0));

    custom_types::Register::RegisterType<GoodbyeMoonMonke::GoodbyeMoonMonkeWatchView>(); 
    GorillaUI::Register::RegisterWatchView<GoodbyeMoonMonke::GoodbyeMoonMonkeWatchView*>("Goodbye Moon Monke", VERSION);
    
    getLogger().info("Mod loaded!");
}