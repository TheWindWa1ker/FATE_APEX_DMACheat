#pragma once
//GameVersion = "v3.0.79.44"
// Core
constexpr uint64_t OFF_LEVEL = 0x17ec544; //[Miscellaneous]->LevelName
constexpr uint64_t OFF_LOCAL_PLAYER = 0x23e91f8; //[Miscellaneous]->LocalPlayer
constexpr uint64_t OFF_ENTITY_LIST = 0x1f15f88; //[Miscellaneous]->cl_entitylist
//OFF_ENTITY_LIST = gamepad_aim_assist_melee=0x01ea9a40 +0xA8

constexpr uint64_t OFF_NAME_LIST = 0xd350450; //[Miscellaneous]->NameList
constexpr uint64_t OFF_NAME_INDEX = 0x38; //nameIndex

constexpr uint64_t OFF_GAMEMODE = 0x02421250;                      //[ConVars]->mp_gamemode
constexpr long OFF_SQUAD_ID = 0x0344;							  //[RecvTable.DT_BaseEntity]->m_squadID

// HUD
constexpr uint64_t OFF_VIEWRENDER = 0x769b558; //[Miscellaneous]->ViewRender
constexpr uint64_t OFF_VIEWMATRIX = 0x11a350; //[Miscellaneous]->ViewMatrix

// Player
constexpr uint64_t OFF_XP = 0x3724;//[RecvTable.DT_Player]->m_xp
constexpr uint64_t OFF_HEALTH = 0x0328; //[RecvTable.DT_Player]->m_iHealth
constexpr uint64_t OFF_MAXHEALTH = 0x0470; //[RecvTable.DT_Player]->m_iMaxHealth
constexpr uint64_t OFF_SHIELD = 0x01a0; //[RecvTable.DT_TitanSoul]->m_shieldHealth
constexpr uint64_t OFF_MAXSHIELD = 0x01a4; //[RecvTable.DT_TitanSoul]->m_shieldHealthMax
constexpr uint64_t OFF_ARMORTYPE = 0x46c4; //[RecvTable.DT_Player]->m_armorType

constexpr uint64_t OFF_CAMERAORIGIN = 0x1ee0; //[Miscellaneous]->CPlayer!camera_origin
constexpr uint64_t OFF_STUDIOHDR = 0x1000; //[Miscellaneous]->CBaseAnimating!m_pStudioHdr
constexpr uint64_t OFF_BONES = 0x0db0 + 0x48; //[RecvTable.DT_BaseAnimating]->m_nForceBone + 0x48

constexpr uint64_t OFF_LOCAL_ORIGIN = 0x017c; //[DataMap.C_BaseEntity]->m_vecAbsOrigin
constexpr uint64_t OFF_ABSVELOCITY = 0x0170; //[DataMap.C_BaseEntity]->m_vecAbsVelocity

constexpr uint64_t OFF_ZOOMING = 0x1be1; //[RecvTable.DT_Player]->m_bZooming
constexpr uint64_t OFF_TEAM_NUMBER = 0x0338; //[RecvTable.DT_BaseEntity]->m_iTeamNum
constexpr uint64_t OFF_NAME = 0x0481; //[RecvTable.DT_BaseEntity]->m_iName
constexpr uint64_t OFF_LIFE_STATE = 0x0690; //[RecvTable.DT_Player]->m_lifeState
constexpr uint64_t OFF_BLEEDOUT_STATE = 0x2760; //[RecvTable.DT_Player]->m_bleedoutState
constexpr uint64_t OFF_LAST_VISIBLE_TIME = 0x19a0; //[Miscellaneous]->CPlayer!lastVisibleTime or [RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x2
constexpr uint64_t OFF_LAST_AIMEDAT_TIME = 0x19a8; //CWeaponX!lastCrosshairTargetTime or [RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x2 + 0x8
constexpr uint64_t OFF_VIEW_ANGLES = 0x2534 - 0x14; //[DataMap.C_Player]->m_ammoPoolCapacity - 0x14
constexpr uint64_t OFF_PUNCH_ANGLES = 0x2438; //[DataMap.C_Player]->m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
constexpr uint64_t OFF_YAW = 0x223c - 0x8; //[DataMap.C_Player]->m_currentFramePlayer.m_ammoPoolCount - 0x8

// Weapon
constexpr uint64_t OFF_WEAPON_HANDLE = 0x1944; //[RecvTable.DT_Player]->m_latestPrimaryWeapons
constexpr uint64_t OFF_WEAPON_INDEX = 0x1788; //[RecvTable.DT_WeaponX]->m_weaponNameIndex
constexpr uint64_t OFF_PROJECTILESCALE = 0x04f4 + 0x19d8; //CWeaponX!m_flProjectileScale or [WeaponSettings]->projectile_gravity_scale + [WeaponSettingsMeta]->base
constexpr uint64_t OFF_PROJECTILESPEED = 0x04ec + 0x19d8; //CWeaponX!m_flProjectileSpeed or [WeaponSettings]->projectile_launch_speed + [WeaponSettingsMeta]->base
constexpr uint64_t OFF_OFFHAND_WEAPON = 0x1954; //[RecvTable.DT_BaseCombatCharacter]->m_latestNonOffhandWeapons
constexpr uint64_t OFF_CURRENTZOOMFOV = 0x15e0 + 0x00b8; //[RecvTable.DT_WeaponX]->m_playerData + [RecvTable.DT_WeaponPlayerData]->m_curZoomFOV
constexpr uint64_t OFF_TARGETZOOMFOV = 0x15e0 + 0x00bc; //[RecvTable.DT_WeaponX]->m_playerData + [RecvTable.DT_WeaponPlayerData]->m_targetZoomFOV
constexpr uint64_t OFF_WEAPON_AMMO = 0x1590; //[RecvTable.DT_WeaponX_LocalWeaponData]->m_ammoInClip
constexpr uint64_t OFF_RELOADING = 0x15aa; //[RecvTable.DT_WeaponX_LocalWeaponData]->m_bInReload

constexpr uint64_t OFFSET_VIEWANGLES = 0x2534 - 0x14; //[RecvTable.DT_Player]->m_ammoPoolCapacity - 0x14
constexpr uint64_t OFFSET_BREATH_ANGLES = OFFSET_VIEWANGLES - 0x10;
constexpr uint64_t OFFSET_VIEW_OFFSET = 0x00e8; //[DataMap.C_BaseEntity]->m_currentFrame.viewOffset
constexpr uint64_t OFFSET_ACTIVE_WEAPON = 0x18d8 + 0x0058; // [RecvTable.DT_AI_BaseNPC]->m_inventory + [DataMap.WeaponInventory_Client]->activeWeapons
constexpr uint64_t OFFSET_WEAPON_BITFIELD = 0x1714; // [RecvTable.DT_WeaponX]->m_modBitfieldFromPlayer

// Glow
constexpr uint64_t OFF_GLOW_HIGHLIGHTS = 0xb13c6a0; //HighlightSettings
constexpr uint64_t OFF_GLOW_ENABLE = 0x29C; //Script_Highlight_GetCurrentContext
constexpr uint64_t OFF_GLOW_THROUGH_WALL = 0x26C; //Script_Highlight_SetVisibilityType
constexpr uint64_t OFF_GLOW_FIX = 0x278;
constexpr uint64_t OFF_GLOW_HIGHLIGHT_ID = 0x29c; //[DT_HighlightSettings].m_highlightServerActiveStates
constexpr uint64_t OFF_GLOW_HIGHLIGHT_TYPE_SIZE = 0x34;

// Misc
constexpr long OFF_TIME_BASE = 0x2088; //[DataMap.C_Player]->m_currentFramePlayer.timeBase
constexpr long OFFSET_TRAVERSAL_START_TIME = 0x2b70; //[RecvTable.DT_LocalPlayerExclusive]->m_traversalStartTime
constexpr long OFFSET_TRAVERSAL_PROGRESS = 0x2b6c; //[RecvTable.DT_LocalPlayerExclusive]->m_traversalProgress
constexpr long OFFSET_UID = 0x2548;								  //[RecvTable.DT_Player]->m_platformUserId
constexpr long OFFSET_VIEWMODEL = 0x2d98;						  //[RecvTable.DT_Player]->m_hViewModels
constexpr long OFF_OBSERVER_LIST = 0x1f17fa8;   //observerList IDA signature >> [48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 FF ? ? 48 85 C0 74 ? 48 63 4E 38]
constexpr long OFF_OBSERVER_ARRAY = 0x974;      //OBSERVER_INDEX signature >> [8B 84 C8 ? ? ? ? 83 F8]

constexpr long OFF_Wall_RunStart_TIME = 0x3614;//[DataMap.C_Player]->m_wallRunStartTime
constexpr long OFF_Wall_RunClear_TIME = 0x3618;//[DataMap.C_Player]->m_wallRunClearTime
constexpr long OFF_SET_FLAGS = 0x00c8;//[DataMap.C_Player]->m_fFlags
constexpr long OFF_SET_SKYDRIVESTATE = 0x4724;//[RecvTable.DT_Player]->m_skydiveState
constexpr long OFF_SET_IN_DUCKSTATE = 0x2a60;//[DataMap.C_Player]->m_duckState

//身法
constexpr uint64_t OFF_INATTACK = 0x0769b658;                         //[Buttons]->in_attack
constexpr uint64_t OFFSET_IN_JUMP = 0x0769b778;						  //[Buttons]->in_jump
constexpr uint64_t OFFSET_IN_DUCK = 0x0769b868;						  //[Buttons]->in_duck
constexpr uint64_t OFFSET_IN_TOGGLE_DUCK = 0x0769b698;				  //[Buttons]->in_toggle_duck
constexpr uint64_t OFFSET_IN_FORWARD = 0x0769b5a8;					  //[Buttons]->in_forward
constexpr uint64_t OFFSET_IN_backward = 0x0769b5d0;					  //[Buttons]->in_backward
constexpr uint64_t OFFSET_IN_moveleft = 0x0769b598;					  //[Buttons]->in_moveleft
constexpr uint64_t OFFSET_IN_moveright = 0x0769b5c0;				  //[Buttons]->in_moveright
constexpr uint64_t OFFSET_IN_RELOAD = 0x0769b6b8;				      //[Buttons]->in_reload
constexpr uint64_t OFFSET_IN_m_skydiveState = 0x4724;				  //[RecvTable.DT_Player]->m_skydiveState
constexpr uint64_t OFFSET_wallRunStartTime = 0x3614;				  //[DataMap.C_Player]->m_wallRunStartTime
constexpr uint64_t OFFSET_wallRunClearTime = 0x3618;				  //[DataMap.C_Player]->m_wallRunClearTime
constexpr uint64_t OFFSET_GRAPPLE = 0x2cc0;							  //[RecvTable.DT_Player]->m_grapple
constexpr uint64_t OFFSET_GRAPPLEACTIVED = 0x2d50;					  //[RecvTable.DT_Player]->m_grappleActive
constexpr uint64_t OFFSET_GRAPPLEATTACHED = 0x0048;					  //[DataMap.GrappleData]->m_grappleAttached
constexpr uint64_t OFFSET_FLAGS = 0x00c8;							  //[RecvTable.DT_Player]->m_fFlags
constexpr uint64_t OFFSET_IN_ZOOM = 0x0769b7f0;		                  //[Buttons]->in_zoom
constexpr uint64_t OFFSET_GlobalVars = 0x17ec040;			          //[Miscellaneous]->GlobalVars
constexpr uint64_t OFFSET_INPUT_SYSTEM = 0x1898e00;                   //[Miscellaneous]->InputSystem

//状态
constexpr uint64_t OFF_CHARGE_START = 0x16d8;                     //[RecvTable.DT_WeaponX]->m_chargeStartTime
constexpr uint64_t OFF_CHARGE_END = 0x16dc;                     //[RecvTable.DT_WeaponX]->m_chargeEndTime