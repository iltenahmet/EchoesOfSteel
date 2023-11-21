#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_Shotgun UMETA(DisplayName = "Shotgun"),
	EAT_Rifle UMETA(DisplayName = "Rifle"),
	EAT_Sniper UMETA(DisplayName = "Sniper"),
	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "Fire Timer In Progress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Sniper UMETA(DisplayName = "Sniper"),
	EWT_DefaultMAX UMETA(DisplayName = "DefaultMAX")
};

