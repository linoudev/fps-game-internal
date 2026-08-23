#include "../CppSDK/SDK/Basic.cpp"
#include "../CppSDK/SDK/CoreUObject_functions.cpp"
#include "../CppSDK/SDK/Engine_functions.cpp"
#include "../CppSDK/SDK/Engine_structs.hpp"
#include "../CppSDK/SDK/BP_PlayerCharacter_classes.hpp"
#include "../CppSDK/SDK/BP_PlayerCharacter_functions.cpp"
#include "../CppSDK/SDK/BP_PlayerState_classes.hpp"
#include "../CppSDK/SDK/BP_PlayerState_functions.cpp"
#include "../CppSDK/SDK/BP_BaseWeapon_classes.hpp"
#include "../CppSDK/SDK/BP_HealthComponent_classes.hpp"
#include "../CppSDK/SDK/BP_BaseCharacter_classes.hpp"
#include "../CppSDK/SDK/BP_BasePlayerController_classes.hpp"
#include "../CppSDK/SDK/BP_Base_GM_classes.hpp"
#include "../CppSDK/SDK/BP_Base_GS_classes.hpp"
#include "../CppSDK/SDK/BP_Base_GS_functions.cpp"
#include "../CppSDK/SDK/BP_Bullet_classes.hpp"
#include "../CppSDK/SDK/AmmoMagazines_structs.hpp"
// SHITCODED INTERNAL BY https://github.com/linoudev
// my first internal, so it's shitcoded oke?
// u can use it but don't claim it as ur own work.
// game update?? just dump the sdk with dumper-7, and replace the CPPSDK folder by your dumped one, and boom its updated
// thanks to @kittyconspiracy on discord and https://github.com/MaddyOff/ue4-canvas-gui for that amazing zerogui

using namespace SDK;

#include "Render.h"
#include "Math.h"
#include "Bones.h"
#include "Menu.h"
#include <Windows.h>
#include <cmath>
#include <cfloat>
#include <vector>

SDK::UWorld* world{ nullptr };
SDK::ULevel* persistentLevel{ nullptr };
SDK::APlayerController* localPlayerController{ nullptr };
SDK::UGameViewportClient* gameViewPortClient{ nullptr };

bool       bHooked{ false };
uintptr_t* vTable{ nullptr };
static uintptr_t vTableHooked[4096];
void (*PostRender)(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas);

bool         bRButtonDown{ false };
int          localTeam{ 0 };
SDK::FVector viewPoint;

static inline bool IsValidPtr(const void* ptr)
{
    return ptr && (uintptr_t)ptr > 0x10000;
}

static inline bool IsValidObject(const SDK::UObject* obj)
{
    if (obj) return true;

    return false;
}


void RunExploits()
{
    if (!localPlayerController || !localPlayerController->AcknowledgedPawn) return;

    SDK::ABP_PlayerCharacter_C* lc = static_cast<SDK::ABP_PlayerCharacter_C*>(localPlayerController->AcknowledgedPawn);
    if (!lc || !IsValidPtr(lc)) return;
    if (lc->bDead || !IsValidPtr(lc->Controller)) return;

    if (Menu::bGodMode && lc && lc->HealthComponent)
    {
        float mx = lc->HealthComponent->MaxHP;
        if (mx > 0.f)
        {
            lc->HealthComponent->MaxHP = 99999.f;
            lc->HealthComponent->HP = 99999.f;
        }
    }

    if (lc->bDead || !IsValidPtr(lc->Controller)) return;

    SDK::ABP_BasePlayerController_C* pc =
        static_cast<SDK::ABP_BasePlayerController_C*>(localPlayerController);
    if (IsValidPtr(pc))
    {
        pc->bInfiniteClip = Menu::bInfClipPC;
        pc->bInfiniteExplosives = Menu::bInfExplosives;
        pc->bNoAbilityCooldownEnable = Menu::bNoCooldown;
    }

    if (Menu::bNoFallDmg)  lc->bEnableFallDamage = false;
    if (Menu::bNoCooldown) { lc->AbilityCoolDown = 0.f; lc->bCanUseSkill = true; }
    if (Menu::bSpeedHack)
    {
        lc->DefaultMovementSpeed = 600.f * Menu::speedMultiplier;
        lc->RunSpeedModifier = 1.5f * Menu::speedMultiplier;
    }

    if (IsValidPtr(lc->CharacterMovement))
    {
        if (Menu::bFly)
        {
            lc->CharacterMovement->MovementMode = SDK::EMovementMode::MOVE_Flying;
            lc->CharacterMovement->GravityScale = 0.f;
            lc->CharacterMovement->MaxFlySpeed = 1500.f * Menu::speedMultiplier;
        }
        else if (lc->CharacterMovement->MovementMode == SDK::EMovementMode::MOVE_Flying)
        {
            lc->CharacterMovement->MovementMode = SDK::EMovementMode::MOVE_Walking;
            lc->CharacterMovement->GravityScale = 1.f;
        }
    }

    if (Menu::bInfMagazines)
    {
        int32_t n = lc->Magazines.Num();
        if (n > 0 && n < 64)
            for (int i = 0; i < n; i++)
            {
                int32_t mx = lc->Magazines[i].MaxQuantity_8_546EA24A4AE28730EF32C0869D82333D;
                if (mx > 0)
                    lc->Magazines[i].Quantity_4_50296D3443367E8FA683FAA68E169687 = mx;
            }
    }


    SDK::ABP_BaseWeapon_C* w = lc->CurrentWeapon;
    if (!w) return;

    if (Menu::bInfAmmo && w->AmmoPerMag > 0) w->ActualAmmo = w->AmmoPerMag;
    if (Menu::bOneShot) { w->Damage = 9999.f; w->HeadshotMultiplier = 9999.f; }
    if (Menu::bFastFire)  w->FireRate = 0.01f;
    if (Menu::bLongRange) w->WeapongRange = 999999.f;

    if (Menu::bNoRecoil)
    {
        w->WeaponSpread = w->AimingSpread = 0.f;
        w->WeaponVerticalRecoilMax = w->WeaponVerticalRecoilMin = 0.f;
        w->WeaponHorizontalRecoilMax = w->WeaponHorizontalRecoilMin = 0.f;
        lc->PitchAcum = lc->YawAcum = lc->TargetSpread = 0.f;
    }
}

void RunGodModePlayers()
{
    if (!IsValidPtr(persistentLevel)) return;

    SDK::TArray<SDK::AActor*> actors = persistentLevel->Actors;
    int32_t n = actors.Num();
    if (n <= 0 || n > 8192) return;

    for (int i = 0; i < n; i++)
    {
        SDK::AActor* a = actors[i];
        if (!IsValidPtr(a) || !IsValidObject(a)) continue;
        if (!a->IsA(SDK::EClassCastFlags::Pawn)) continue;

        SDK::ABP_PlayerCharacter_C* p =
            static_cast<SDK::ABP_PlayerCharacter_C*>(a);
        if (!IsValidObject(p) || p->bDead) continue;
        if (!IsValidObject(p->HealthComponent)) continue;

        float mx = p->HealthComponent->MaxHP;
        if (mx <= 0.f) continue;

        p->HealthComponent->MaxHP = 99999.f;
        p->HealthComponent->HP = 99999.f;
    }
}

void RunGlobalExploits()
{
    if (!IsValidPtr(world)) return;

    if (Menu::bFriendlyFire && IsValidPtr(world->AuthorityGameMode))
    {
        if (world->AuthorityGameMode->IsA(SDK::EClassCastFlags::Actor))
        {
            SDK::ABP_Base_GM_C* gm =
                static_cast<SDK::ABP_Base_GM_C*>(world->AuthorityGameMode);
            uint8_t bgo = *reinterpret_cast<uint8_t*>(
                reinterpret_cast<uintptr_t>(gm) + 0x0358);
            if (bgo == 0 || bgo == 1) gm->bFriendlyFire = true;
        }
    }
}

void RunBulletDmg()
{
    if (!IsValidPtr(persistentLevel)) return;
    SDK::UClass* bc = SDK::ABP_Bullet_C::StaticClass();
    if (!IsValidPtr(bc)) return;

    SDK::TArray<SDK::AActor*> actors = persistentLevel->Actors;
    int32_t n = actors.Num();
    if (n <= 0 || n > 8192) return;

    for (int i = 0; i < n; i++)
    {
        SDK::AActor* a = actors[i];
        if (!IsValidPtr(a) || a->Class != bc) continue;
        static_cast<SDK::ABP_Bullet_C*>(a)->Damage = 9999.f;
    }
}

void RunKillAll()
{
    if (!IsValidPtr(persistentLevel)) return;
    SDK::TArray<SDK::AActor*> actors = persistentLevel->Actors;
    int32_t n = actors.Num();
    if (n <= 0 || n > 8192) return;

    for (int i = 0; i < n; i++)
    {
        SDK::AActor* a = actors[i];
        if (!IsValidPtr(a) || !IsValidObject(a)) continue;
        if (!a->IsA(SDK::EClassCastFlags::Pawn)) continue;
        SDK::ABP_PlayerCharacter_C* t = static_cast<SDK::ABP_PlayerCharacter_C*>(a);
        if (!IsValidObject(t) || t == localPlayerController->AcknowledgedPawn) continue;
        if (t->bDead || !IsValidObject(t->PlayerState)) continue;
        SDK::ABP_PlayerState_C* ps = static_cast<SDK::ABP_PlayerState_C*>(t->PlayerState);
        if (!IsValidObject(ps) || ps->Team == localTeam) continue;
        t->Suicide();
    }
    Menu::bKillAll = false;
}

void RunInstantWin()
{
    if (!IsValidPtr(world)) return;
    SDK::ABP_Base_GS_C* gs = static_cast<SDK::ABP_Base_GS_C*>(world->GameState);
    if (!IsValidPtr(gs)) return;
    int32_t ptw = gs->PointsToWin;
    if (ptw <= 0 || ptw > 9999) return;

    if (IsValidPtr(localPlayerController) && IsValidPtr(localPlayerController->PlayerState))
    {
        SDK::ABP_PlayerState_C* ps =
            static_cast<SDK::ABP_PlayerState_C*>(localPlayerController->PlayerState);
        if (IsValidPtr(ps))
        {
            ps->Kills = 9999; ps->PlayerScore = 999999.f;
            ps->KillStreak = 9999; ps->KillingSpree = 9999;
        }
    }
    for (int i = 0; i <= ptw; i++) gs->AddTeamPoints(localTeam);
    Menu::bInstantWin = false;
}

static SDK::FVector s_prevLocalPos{};
static bool         s_pullInit{ false };

void RunPullPlayers()
{
    if (!IsValidPtr(persistentLevel) || !IsValidPtr(localPlayerController)) return;
    SDK::ABP_PlayerCharacter_C* lc =
        static_cast<SDK::ABP_PlayerCharacter_C*>(localPlayerController->AcknowledgedPawn);
    if (!IsValidPtr(lc) || lc->bDead || !IsValidPtr(lc->CapsuleComponent)) return;

    SDK::FVector* localPos =
        reinterpret_cast<SDK::FVector*>(
            reinterpret_cast<uintptr_t>(lc->CapsuleComponent) + 0x011C);

    if (!s_pullInit)
    {
        s_prevLocalPos = *localPos;
        s_pullInit = true;
        return;
    }

    SDK::FVector delta{
        localPos->X - s_prevLocalPos.X,
        localPos->Y - s_prevLocalPos.Y,
        localPos->Z - s_prevLocalPos.Z
    };
    float dLen = sqrtf(delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z);

    SDK::FRotator cr = localPlayerController->GetControlRotation();
    float yR = cr.Yaw * (3.14159265f / 180.f);
    float pR = cr.Pitch * (3.14159265f / 180.f);
    float fwX = cosf(pR) * cosf(yR);
    float fwY = cosf(pR) * sinf(yR);
    float fwZ = -sinf(pR);

    constexpr float DIST = 150.f;
    SDK::FVector tgt{
        localPos->X + fwX * DIST,
        localPos->Y + fwY * DIST,
        localPos->Z + fwZ * DIST
    };

    SDK::TArray<SDK::AActor*> actors = persistentLevel->Actors;
    int32_t cnt = actors.Num();
    if (cnt <= 0 || cnt > 8192) { s_prevLocalPos = *localPos; return; }

    int ei = 0;
    for (int i = 0; i < cnt; i++)
    {
        SDK::AActor* a = actors[i];
        if (!IsValidPtr(a) || !IsValidObject(a)) continue;
        if (!a->IsA(SDK::EClassCastFlags::Pawn)) continue;
        SDK::ABP_PlayerCharacter_C* t = static_cast<SDK::ABP_PlayerCharacter_C*>(a);
        if (!IsValidObject(t) || t == lc || t->bDead) continue;
        if (!IsValidObject(t->PlayerState)) continue;
        SDK::ABP_PlayerState_C* ps = static_cast<SDK::ABP_PlayerState_C*>(t->PlayerState);
        if (!IsValidObject(ps) || ps->Team == localTeam) continue;
        if (!IsValidObject(t->CapsuleComponent)) continue;

        SDK::FVector* ep =
            reinterpret_cast<SDK::FVector*>(
                reinterpret_cast<uintptr_t>(t->CapsuleComponent) + 0x011C);

        if (IsValidPtr(t->CharacterMovement))
        {
            t->CharacterMovement->Velocity = SDK::FVector{ 0.f, 0.f, 0.f };
            t->CharacterMovement->GravityScale = 0.f;
        }

        if (dLen < 0.1f)
        {
            float ang = (float)ei * (6.2831853f / 8.f);
            float spr = (ei == 0) ? 0.f : 60.f;
            ep->X = tgt.X + cosf(ang) * spr;
            ep->Y = tgt.Y + sinf(ang) * spr;
            ep->Z = tgt.Z;
        }
        else
        {
            ep->X += delta.X;
            ep->Y += delta.Y;
            ep->Z += delta.Z;
        }
        ei++;
    }
    s_prevLocalPos = *localPos;
}

void RunSilentAim(SDK::FVector bestHead, bool hasTarget)
{
    if (!hasTarget || !IsValidPtr(persistentLevel)) return;
    if (!IsValidPtr(localPlayerController) || !IsValidPtr(localPlayerController->AcknowledgedPawn)) return;

    SDK::ABP_PlayerCharacter_C* lc =
        static_cast<SDK::ABP_PlayerCharacter_C*>(localPlayerController->AcknowledgedPawn);
    if (!IsValidPtr(lc) || !IsValidPtr(lc->CurrentWeapon)) return;
    if (!IsValidPtr(localPlayerController->PlayerCameraManager)) return;

    SDK::UClass* bc = SDK::ABP_Bullet_C::StaticClass();
    if (!IsValidPtr(bc)) return;

    SDK::FVector camLoc = localPlayerController->PlayerCameraManager->GetCameraLocation();
    float dx = bestHead.X - camLoc.X;
    float dy = bestHead.Y - camLoc.Y;
    float dz = bestHead.Z - camLoc.Z;
    float len = sqrtf(dx * dx + dy * dy + dz * dz);
    if (len < 0.001f) return;

    constexpr float SPD = 99999.f;
    SDK::FVector newVel{ (dx / len) * SPD, (dy / len) * SPD, (dz / len) * SPD };

    SDK::TArray<SDK::AActor*> actors = persistentLevel->Actors;
    int32_t cnt = actors.Num();
    if (cnt <= 0 || cnt > 8192) return;

    for (int i = 0; i < cnt; i++)
    {
        SDK::AActor* a = actors[i];
        if (!IsValidPtr(a) || a->Class != bc) continue;
        SDK::ABP_Bullet_C* b = static_cast<SDK::ABP_Bullet_C*>(a);
        if (!IsValidPtr(b->WeaponRef) || b->WeaponRef != lc->CurrentWeapon) continue;
        b->Velocity = newVel;
    }
}

SDK::FVector RunAimbot(SDK::UCanvas* canvas, bool& outHasTarget)
{
    outHasTarget = false;
    SDK::FVector bestWorld{};
    if (!IsValidPtr(persistentLevel)) return bestWorld;

    bool doAim = (Menu::bAimbot && bRButtonDown) || Menu::bAimbotAlwaysOn;
    bool needTarget = doAim || Menu::bSilentAim || Menu::bSnapLine;
    if (!needTarget) return bestWorld;

    SDK::TArray<SDK::AActor*> actors = persistentLevel->Actors;
    int32_t cnt = actors.Num();
    if (cnt <= 0 || cnt > 8192) return bestWorld;

    float cx = canvas->ClipX * 0.5f;
    float cy = canvas->ClipY * 0.5f;
    float bestDist = FLT_MAX;

    for (int i = 0; i < cnt; i++)
    {
        SDK::AActor* a = actors[i];
        if (!IsValidPtr(a) || !IsValidObject(a)) continue;
        if (!a->IsA(SDK::EClassCastFlags::Pawn)) continue;
        SDK::ABP_PlayerCharacter_C* t = static_cast<SDK::ABP_PlayerCharacter_C*>(a);
        if (!IsValidObject(t) || t == localPlayerController->AcknowledgedPawn || t->bDead) continue;
        if (!IsValidObject(t->PlayerState)) continue;
        SDK::ABP_PlayerState_C* ps = static_cast<SDK::ABP_PlayerState_C*>(t->PlayerState);
        if (!IsValidObject(ps) || ps->Team == localTeam) continue;
        if (!IsValidObject(t->Mesh)) continue;

        int targetBone = Menu::bAimbotHeadshot ? Bones::Head : Bones::spine_03;
        SDK::USkeletalMeshComponent* mesh = t->Mesh;
        if (!IsValidObject(mesh)) continue;
        SDK::FName bn = mesh->GetBoneName(targetBone);
        SDK::FVector wp = mesh->GetSocketLocation(bn);

        if (Menu::bAimbotVisCheck && !Math::IsVisible(world, viewPoint, wp))
            continue;

        SDK::FVector2D sp{};
        if (!Math::WorldToScreen(*localPlayerController, wp, &sp)) continue;

        float d = sqrtf((sp.X - cx) * (sp.X - cx) + (sp.Y - cy) * (sp.Y - cy));
        if (d < bestDist) { bestDist = d; bestWorld = wp; outHasTarget = true; }
    }

    if (!outHasTarget) return bestWorld;

    if (Menu::bSnapLine)
    {
        SDK::FVector2D sp{};
        if (Math::WorldToScreen(*localPlayerController, bestWorld, &sp))
        {
            bool vis = Math::IsVisible(world, viewPoint, bestWorld);
            canvas->K2_DrawLine(
                SDK::FVector2D{ cx, cy }, sp, 1.0f,
                vis ? Color::green : Color::red);
        }
    }

    if (doAim)
    {
        SDK::FRotator rot = SDK::UKismetMathLibrary::FindLookAtRotation(
            localPlayerController->PlayerCameraManager->GetCameraLocation(), bestWorld);
        localPlayerController->SetControlRotation(rot);
    }

    return bestWorld;
}


void RunESP(SDK::UCanvas* canvas)
{
    if (!IsValidPtr(persistentLevel)) return;
    if (!Menu::bSkeletonESP && !Menu::bBoxESP && !Menu::bHealthBar &&
        !Menu::bDistanceESP && !Menu::bTracerLine) return;

    float cx = canvas->ClipX * 0.5f;


    SDK::TArray<SDK::AActor*> Actors;
    SDK::UGameplayStatics::GetAllActorsOfClass(SDK::UWorld::GetWorld(), SDK::ABP_PlayerCharacter_C::StaticClass(), &Actors);
    for (int i = 0; i < Actors.Num(); i++)
    {
        if (!Actors.IsValidIndex(i)) continue;
        if (!Actors[i]) continue;

        SDK::AActor* Actor = Actors[i];
        if (!Actor) continue;

        if (!Actor->IsA(SDK::ABP_PlayerCharacter_C::StaticClass())) continue; // fr
        SDK::ABP_PlayerCharacter_C* Character = reinterpret_cast<SDK::ABP_PlayerCharacter_C*>(Actor);
        if (!Character) continue;
        if (!Character->Mesh) continue;
        if (!Character->PlayerState) continue;
        //  if (Character == AcknowledgedPawn) continue;
        if (Character->bDead) continue;
        SDK::USkeletalMeshComponent* mesh = Character->Mesh;
        if (!mesh) continue;
        auto p = Character;
        if (localPlayerController && p == localPlayerController->AcknowledgedPawn) continue;

        SDK::FVector headW = mesh->GetSocketTransform(SDK::UKismetStringLibrary::Conv_StringToName(L"Head"), SDK::ERelativeTransformSpace::RTS_World).Translation;
        SDK::FVector footW = mesh->GetSocketTransform(SDK::UKismetStringLibrary::Conv_StringToName(L"root"), SDK::ERelativeTransformSpace::RTS_World).Translation;

        SDK::FVector2D headS{}, footS{};
        if (!Math::WorldToScreen(*localPlayerController, headW, &headS)) continue;
        if (!Math::WorldToScreen(*localPlayerController, footW, &footS)) continue;

        bool vis = Math::IsVisible(world, viewPoint, headW);
        SDK::FLinearColor col = vis ? Color::green : Color::red;

        if (Menu::bTracerLine)
            canvas->K2_DrawLine(
                SDK::FVector2D{ cx, canvas->ClipY }, footS, 1.f, Color::yellow);

        if (Menu::bBoxESP)
        {
            float bh = fabsf(headS.Y - footS.Y);
            float bw = bh * 0.4f;
            float bx = headS.X - bw * 0.5f;
            float by = headS.Y;

            canvas->K2_DrawLine({ bx,      by }, { bx + bw,  by }, 1.f, col);
            canvas->K2_DrawLine({ bx,      by + bh }, { bx + bw,  by + bh }, 1.f, col);
            canvas->K2_DrawLine({ bx,      by }, { bx,     by + bh }, 1.f, col);
            canvas->K2_DrawLine({ bx + bw,   by }, { bx + bw,  by + bh }, 1.f, col);

            if (Menu::bHealthBar && IsValidPtr(p->HealthComponent))
            {
                float hp = p->HealthComponent->HP;
                float mhp = p->HealthComponent->MaxHP;
                float r = (mhp > 0.f) ? (hp / mhp) : 0.f;
                r = r < 0.f ? 0.f : r > 1.f ? 1.f : r;
                float bBarX = bx - 6.f;
                float fillY = by + bh - bh * r;
                canvas->K2_DrawLine({ bBarX, by }, { bBarX, by + bh }, 3.f,
                    SDK::FLinearColor{ 0.3f, 0.f, 0.f, 1.f });
                canvas->K2_DrawLine({ bBarX, fillY }, { bBarX, by + bh }, 3.f,
                    SDK::FLinearColor{ 1.f - r, r, 0.f, 1.f });
            }

            if (Menu::bDistanceESP)
            {
                SDK::FVector ap = p->K2_GetActorLocation();
                float dx = ap.X - viewPoint.X, dy2 = ap.Y - viewPoint.Y, dz = ap.Z - viewPoint.Z;
                float dm = sqrtf(dx * dx + dy2 * dy2 + dz * dz) / 100.f;
                float barW = (dm < 50.f) ? (dm / 50.f) * bw : bw;
                SDK::FLinearColor dc{ dm / 100.f, 1.f - dm / 100.f, 0.f, 1.f };
                canvas->K2_DrawLine(
                    { bx,       by + bh + 4.f },
                    { bx + barW,  by + bh + 4.f }, 2.f, dc);
            }
        }

        if (Menu::bSkeletonESP)
        {
            for (auto& bl : boneLinks)
            {
                SDK::FName b1 = mesh->GetBoneName(bl.first);
                SDK::FName b2 = mesh->GetBoneName(bl.second);
                SDK::FVector w1 = mesh->GetSocketLocation(b1);
                SDK::FVector w2 = mesh->GetSocketLocation(b2);
                SDK::FVector2D s1{}, s2{};
                if (Math::WorldToScreen(*localPlayerController, w1, &s1) &&
                    Math::WorldToScreen(*localPlayerController, w2, &s2))
                    canvas->K2_DrawLine(s1, s2, 1.f, col);
            }
        }
    }
}

bool UpdateInstance()
{
    world = SDK::UWorld::GetWorld();
    if (!world || !world->PersistentLevel) return false;
    persistentLevel = world->PersistentLevel;
    if (!world->OwningGameInstance) return false;
    if (!world->OwningGameInstance->LocalPlayers[0]) return false;
    if (!world->OwningGameInstance->LocalPlayers[0]->PlayerController) return false;
    localPlayerController = world->OwningGameInstance->LocalPlayers[0]->PlayerController;

    if (IsValidPtr(localPlayerController->AcknowledgedPawn) && localPlayerController->AcknowledgedPawn)
    {
        if (!localPlayerController->AcknowledgedPawn->IsA(ABP_PlayerCharacter_C::StaticClass())) return false;

        SDK::ABP_PlayerCharacter_C* lc = static_cast<SDK::ABP_PlayerCharacter_C*>(localPlayerController->AcknowledgedPawn);
        if (IsValidPtr(lc))
        {
            lc->GetActorEyesViewPoint(&viewPoint, nullptr);
            if (IsValidPtr(lc->PlayerState))
            {
                SDK::ABP_PlayerState_C* ps =
                    static_cast<SDK::ABP_PlayerState_C*>(lc->PlayerState);
                if (IsValidPtr(ps)) localTeam = ps->Team;
            }
        }

        return true;
    }

    return false;
}

void UnHook() { *(PVOID*)gameViewPortClient = vTable; }

void PostRenderHook(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas)
{
    if (UpdateInstance())
    {
        if (GetAsyncKeyState(VK_INSERT) & 1) Menu::bMenu = !Menu::bMenu;
        bRButtonDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;


        auto ViewportClient = _this;

        SDK::UWorld* World = reinterpret_cast<SDK::UWorld*>(ViewportClient->World);
        if (!World) return;
        SDK::UGameInstance* GameInstance = ViewportClient->GameInstance;
        if (!GameInstance) return;
        SDK::ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
        if (!LocalPlayer) return;
        SDK::APlayerController* PlayerController = LocalPlayer->PlayerController;
        if (!PlayerController) return;

        SDK::ACharacter* AcknowledgedPawn = reinterpret_cast<SDK::ACharacter*>(PlayerController->Pawn);
        if (!AcknowledgedPawn) return;


        RunExploits();
        RunGlobalExploits();

        if (Menu::bGodModePlayers) RunGodModePlayers();
        if (Menu::bBulletDmg)     RunBulletDmg();
        if (Menu::bKillAll)       RunKillAll();
        if (Menu::bInstantWin)    RunInstantWin();

        if (Menu::bPullPlayers)   RunPullPlayers();
        else                      s_pullInit = false;

        bool hasTarget = false;
        SDK::FVector bestHead = RunAimbot(Canvas, hasTarget);
        if (Menu::bSilentAim) RunSilentAim(bestHead, hasTarget);

        RunESP(Canvas);

        if (Menu::bMenu) Menu::DrawMenu(Canvas);
    }

    return PostRender(_this, Canvas);
}

void InitHook()
{
    bHooked = true;
    vTable = *(uintptr_t**)gameViewPortClient;
    memcpy(vTableHooked, vTable, sizeof(vTableHooked));
    vTableHooked[0x63] = (uintptr_t)&PostRenderHook;
    PostRender = (decltype(PostRender))vTable[0x63];
    *(PVOID*)gameViewPortClient = vTableHooked;
}

DWORD HackThread(HMODULE hModule)
{
    do
    {
        world = SDK::UWorld::GetWorld();
        if (!world || !world->OwningGameInstance) continue;
        if (!world->OwningGameInstance->LocalPlayers[0]) continue;
        if (!world->OwningGameInstance->LocalPlayers[0]->ViewportClient) continue;
        gameViewPortClient = world->OwningGameInstance->LocalPlayers[0]->ViewportClient;
        InitHook();
    } while (!bHooked);

    while (bHooked)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            UnHook();
            FreeLibraryAndExitThread(hModule, 0);
            break;
        }
        Sleep(5);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
    }
    return TRUE;
}