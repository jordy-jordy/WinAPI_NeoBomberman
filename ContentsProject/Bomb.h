#pragma once
#include <EngineCore/Actor.h>

enum class EDirection
{
	Left,
	Right,
	Up,
	Down
};

class ABomb : public AActor
{
public:
	// constrcuter destructer
	ABomb();
	~ABomb();

	// delete Function
	ABomb(const ABomb& _Other) = delete;
	ABomb(ABomb&& _Other) noexcept = delete;
	ABomb& operator=(const ABomb& _Other) = delete;
	ABomb& operator=(ABomb&& _Other) noexcept = delete;

	class USpriteRenderer* SpriteRenderer;

	void SetWallTileMap(class ATileMap* _TileMap, FIntPoint _Index);

	void HandleExplosion(EDirection Direction, int Power);
	void HandleTileDestruction(const FVector2D& TargetPosition);

protected:
	friend class APlayer;
	void Tick(float _DeltaTime) override;

	void SetPower(int _Power)
	{
		CurBombPower = _Power;
	}

	void Bomb_ExPlode();
	void Bomb_Destroy();


private:
	class USpriteRenderer* Explode_Center = nullptr;
	class ATileMap* WallTileMap = nullptr;
	class ATile_Destroy* Object = nullptr;
	FIntPoint BombTileIndex;

	void ClearBombTile(); // ��ź ���� ���� Ÿ�ϸ� ������Ʈ

	int CurBombPower = 0; // ���� ��ź�� �Ŀ�

	float Bomb_Explode_Time = 2.0f;

	float IdleFrameSpeed = 0.15f;
	float ExplodeFrameSpeed = 0.05f;

	bool ISEXPLODING = false;

};

