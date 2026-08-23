#include "Menu.h"

SDK::FVector2D Menu::menuPos{ 100.0f, 100.0f };
bool Menu::bMenu{ true };

bool  Menu::bAimbot{ false };
bool  Menu::bAimbotAlwaysOn{ false };
bool  Menu::bSilentAim{ false };
bool  Menu::bAimbotVisCheck{ true };
bool  Menu::bAimbotHeadshot{ true };
float Menu::fAimbotFOV{ 0.0f };
float Menu::fAimbotSmooth{ 0.0f };

// Visuals
bool Menu::bSkeletonESP{ false };
bool Menu::bBoxESP{ false };
bool Menu::bHealthBar{ false };
bool Menu::bDistanceESP{ false };
bool Menu::bTracerLine{ false };
bool Menu::bSnapLine{ false };

// Exploits
bool  Menu::bGodMode{ false };
bool  Menu::bGodModePlayers{ false };
bool  Menu::bInfAmmo{ false };
bool  Menu::bInfMagazines{ false };
bool  Menu::bInfExplosives{ false };
bool  Menu::bNoRecoil{ false };
bool  Menu::bOneShot{ false };
bool  Menu::bNoFallDmg{ false };
bool  Menu::bSpeedHack{ false };
bool  Menu::bNoCooldown{ false };
bool  Menu::bInfClipPC{ false };
bool  Menu::bFriendlyFire{ false };
bool  Menu::bFastFire{ false };
bool  Menu::bLongRange{ false };
bool  Menu::bBulletDmg{ false };
bool  Menu::bKillAll{ false };
bool  Menu::bFly{ false };
bool  Menu::bInstantWin{ false };
bool  Menu::bPullPlayers{ false };
float Menu::speedMultiplier{ 2.0f };

static int currentTab = 0;

void Menu::DrawMenu(SDK::UCanvas* canvas)
{
    ZeroGUI::SetupCanvas(canvas);
    ZeroGUI::hover_element = false;
    ZeroGUI::Input::Handle();

    if (!ZeroGUI::Window("fps game cat test | free in discord.gg/linouservices", &menuPos, SDK::FVector2D{ 500.0f, 400.0f }, bMenu))
        return;

    if (ZeroGUI::ButtonTab("aim",   SDK::FVector2D{ 110.0f, 25.0f }, currentTab == 0)) currentTab = 0;
    ZeroGUI::NextColumn(115.0f);
    if (ZeroGUI::ButtonTab("esp",  SDK::FVector2D{ 110.0f, 25.0f }, currentTab == 1)) currentTab = 1;
    ZeroGUI::NextColumn(230.0f);
    if (ZeroGUI::ButtonTab("exploits", SDK::FVector2D{ 110.0f, 25.0f }, currentTab == 2)) currentTab = 2;
    ZeroGUI::NextColumn(345.0f);
    if (ZeroGUI::ButtonTab("settings", SDK::FVector2D{ 110.0f, 25.0f }, currentTab == 3)) currentTab = 3;
    ZeroGUI::NextColumn(0.0f);
    ZeroGUI::Text("");

    switch (currentTab)
    {
    case 0:
        ZeroGUI::Checkbox("aimbot (right click)",        &bAimbot);
        ZeroGUI::Checkbox("always on",  &bAimbotAlwaysOn);
        ZeroGUI::Checkbox("magic bulleh",          &bSilentAim);
        ZeroGUI::Checkbox("visible check",        &bAimbotVisCheck);
        ZeroGUI::Checkbox("aim head (if not = chest )",&bAimbotHeadshot);
        ZeroGUI::Checkbox("aim tracer",           &bSnapLine);
        break;

    case 1:
        ZeroGUI::Checkbox("skeleton",  &bSkeletonESP);
        ZeroGUI::Checkbox("box",       &bBoxESP);
        ZeroGUI::Checkbox("health",    &bHealthBar);
        ZeroGUI::Checkbox("distance",      &bDistanceESP);
        ZeroGUI::Checkbox("snapline",   &bTracerLine);
        break;

    case 2:
        ZeroGUI::Checkbox("godmode (host only or crash nigga)",             &bGodMode);
        ZeroGUI::Checkbox("godmode every player (host only)",  &bGodModePlayers);
        ZeroGUI::Checkbox("infinite ammo",       &bInfAmmo);
     //   ZeroGUI::Checkbox("inf magazines",         &bInfMagazines);
        ZeroGUI::Checkbox("infinite grenades",        &bInfExplosives);
        ZeroGUI::Checkbox("no recoil/spread",    &bNoRecoil);
        ZeroGUI::Checkbox("one shot",         &bOneShot);
        ZeroGUI::Checkbox("no fall damage",        &bNoFallDmg);
        ZeroGUI::Checkbox("speedhack",            &bSpeedHack);
      //  ZeroGUI::Checkbox("no ability cooldown",   &bNoCooldown);
       // ZeroGUI::Checkbox("inf clip pc)",    &bInfClipPC);
        ZeroGUI::Checkbox("rapid fire",        &bFastFire);
        ZeroGUI::Checkbox("long range",            &bLongRange);
        ZeroGUI::Checkbox("boost bullet dmg (useless)",      &bBulletDmg);
        ZeroGUI::Checkbox("teamkill",         &bFriendlyFire);
        ZeroGUI::Checkbox("kill all (host only)",              &bKillAll);
        ZeroGUI::Checkbox("fly",              &bFly);
        ZeroGUI::Checkbox("insta win (host only)",           &bInstantWin);
        ZeroGUI::Checkbox("freeze players (host only)",          &bPullPlayers);
    //    ZeroGUI::Checkbox("godmode players", &bGodModePlayers);
        break;

    case 3:
        ZeroGUI::Text("speedhack speed");
        ZeroGUI::SliderFloat("speed", &speedMultiplier, 1.0f, 10.0f);
        break;
    }

    ZeroGUI::Render();
    ZeroGUI::Draw_Cursor(true);
}
