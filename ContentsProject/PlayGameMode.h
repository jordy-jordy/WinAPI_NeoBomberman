#pragma once
#include <EngineCore/GameMode.h>


// ���� :
class APlayGameMode : public AGameMode
{
public:
	// constrcuter destructer
	APlayGameMode();
	~APlayGameMode();

	// delete Function
	APlayGameMode(const APlayGameMode& _Other) = delete;
	APlayGameMode(APlayGameMode&& _Other) noexcept = delete;
	APlayGameMode& operator=(const APlayGameMode& _Other) = delete;
	APlayGameMode& operator=(APlayGameMode&& _Other) noexcept = delete;

	void BeginPlay();

	FVector2D WindowSize = { 608, 448 };
	FVector2D WallTileMapLocation = { 96, 64 };

	bool IsMonsterAllDead();

	void PlusScore(int _Score)
	{
		PlayerScore += _Score;
	}

	int GetScore()
	{
		return PlayerScore;
	}

	void StartTIme()
	{
		IsStopTime = false;
	}

	void StopTIme()
	{
		IsStopTime = true;
	}

	static float StageTime;
	static int PlayerScore;


protected:
	void Tick(float _DeltaTime) override;
	void PlayerInit();
	void PlayTileMapInit();
	void MonsterInit();
	void PortalInit();
	void PortalON();
	void MOVETO_BOSS();

private:
	class ATileMap* WallTileMap = nullptr;
	class APortal* Portal = nullptr;

	class USpriteRenderer* SpriteRendererSTAGE = nullptr;
	class USpriteRenderer* SpriteRendererSPAWN = nullptr;
	class USpriteRenderer* SpriteRendererPORTAL = nullptr;
	
	class AScore* Minute = nullptr;
	class AScore* Second = nullptr;
	class AScore* Zero = nullptr;
	class AScore* Score = nullptr;

	class AFade* Actor_Fade = nullptr;


	bool IsStopTime = false;
};

