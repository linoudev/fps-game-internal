#pragma once

#include "ZeroGUI.h"

namespace Menu
{
    extern SDK::FVector2D menuPos;
    extern bool  bMenu;
    extern bool  bAimbot;
    extern bool  bAimbotAlwaysOn;
    extern bool  bSilentAim;
    extern bool  bAimbotVisCheck;
    extern bool  bAimbotHeadshot;
    extern float fAimbotFOV;
    extern float fAimbotSmooth;

    extern bool bSkeletonESP;
    extern bool bBoxESP;
    extern bool bHealthBar;
    extern bool bDistanceESP;
    extern bool bTracerLine;
    extern bool bSnapLine;

    extern bool  bGodMode;
    extern bool  bGodModePlayers;
    extern bool  bInfAmmo;
    extern bool  bInfMagazines;
    extern bool  bInfExplosives;
    extern bool  bNoRecoil;
    extern bool  bOneShot;
    extern bool  bNoFallDmg;
    extern bool  bSpeedHack;
    extern bool  bNoCooldown;
    extern bool  bInfClipPC;
    extern bool  bFriendlyFire;
    extern bool  bFastFire;
    extern bool  bLongRange;
    extern bool  bBulletDmg;
    extern bool  bKillAll;
    extern bool  bFly;
    extern bool  bInstantWin;
    extern bool  bPullPlayers;
    extern bool  bGodModePlayers;
    extern float speedMultiplier;

    void DrawMenu(SDK::UCanvas* canvas);
}
