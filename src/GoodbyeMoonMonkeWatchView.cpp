
#include "GoodbyeMoonMonkeWatchView.hpp"
#include "config.hpp"
#include "monkecomputer/shared/ViewLib/MonkeWatch.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "UnityEngine/Vector3.hpp"

DEFINE_TYPE(GoodbyeMoonMonke::GoodbyeMoonMonkeWatchView);

using namespace GorillaUI;
using namespace UnityEngine;

extern bool isRoom;

namespace GoodbyeMoonMonke
{
    void GoodbyeMoonMonkeWatchView::Awake()
    {
        toggleHandler = new UIToggleInputHandler(EKeyboardKey::Enter, EKeyboardKey::Enter, true);
    }

    void GoodbyeMoonMonkeWatchView::DidActivate(bool firstActivation)
    {
        std::function<void(bool)> fun = std::bind(&GoodbyeMoonMonkeWatchView::OnToggle, this, std::placeholders::_1);
        toggleHandler->toggleCallback = fun;
        Redraw();
    }

    void GoodbyeMoonMonkeWatchView::Redraw()
    {
        text = "";

        DrawHeader();
        DrawBody();

        watch->Redraw();
    }

    void GoodbyeMoonMonkeWatchView::DrawHeader()
    {
        text += "<color=#136CFF>== <color=#FF2F3C>GoodbyeMoon Monke</color> ==</color>\n";
    }

    void GoodbyeMoonMonkeWatchView::DrawBody()
    {
        text += "\nGoodbye Moon Monke is:\n\n  ";
        text += config.enabled ? "<color=#00ff00>> Enabled</color>" : "<color=#ff0000>> Disabled</color>";

        if (config.enabled && !isRoom)
        {
            text += "\n\nBut is disabled\ndue to not being in\na private room\n";
        }
    }

    void GoodbyeMoonMonkeWatchView::OnToggle(bool value)
    {
        config.enabled = value;

        SaveConfig();
    }

    void GoodbyeMoonMonkeWatchView::OnKeyPressed(int key)
    {
        toggleHandler->HandleKey((EKeyboardKey)key);
        Redraw();
    }
}