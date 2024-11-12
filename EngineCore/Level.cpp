#include "PreCompile.h"
#include "Level.h"
#include "EngineAPICore.h"

#include <EngineBase/EngineMath.h>
#include <EnginePlatform/EngineWindow.h>
#include <EnginePlatform/EngineWinImage.h>

#include "SpriteRenderer.h"
#include "2DCollision.h"

#include "EngineCoreDebug.h"

ULevel::ULevel()
{
}

ULevel::~ULevel()
{
	{
		// BeginPlayList 한번도 체인지 안한 액터는 
		// 액터들이 다 비긴 플레이 리스트에 들어가 있다.

		std::list<AActor*>::iterator StartIter = BeginPlayList.begin();
		std::list<AActor*>::iterator EndIter = BeginPlayList.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			AActor* CurActor = *StartIter;
			delete CurActor;
		}
	}



	std::list<AActor*>::iterator StartIter = AllActors.begin();
	std::list<AActor*>::iterator EndIter = AllActors.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		AActor* CurActor = *StartIter;

		if (nullptr != CurActor)
		{
			delete* StartIter;
		}
	}
}

// 내가 CurLevel 됐을대
void ULevel::LevelChangeStart()
{
	{
		{
			std::list<AActor*>::iterator StartIter = AllActors.begin();
			std::list<AActor*>::iterator EndIter = AllActors.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				AActor* CurActor = *StartIter;

				// 이건 꺼진애도 호출됩니다.
				CurActor->LevelChangeStart();
			}
		}

		{
			std::list<AActor*>::iterator StartIter = BeginPlayList.begin();
			std::list<AActor*>::iterator EndIter = BeginPlayList.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				AActor* CurActor = *StartIter;

				// 이건 꺼진애도 호출됩니다.
				CurActor->LevelChangeStart();
			}
		}
	}

}

// 나 이제 새로운 레벨로 바뀔거야.
void ULevel::LevelChangeEnd()
{
	{
		{
			std::list<AActor*>::iterator StartIter = AllActors.begin();
			std::list<AActor*>::iterator EndIter = AllActors.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				AActor* CurActor = *StartIter;

				CurActor->LevelChangeEnd();
			}
		}

		{
			std::list<AActor*>::iterator StartIter = BeginPlayList.begin();
			std::list<AActor*>::iterator EndIter = BeginPlayList.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				AActor* CurActor = *StartIter;

				// 이건 꺼진애도 호출됩니다.
				CurActor->LevelChangeEnd();
			}
		}
	}

}

void ULevel::Tick(float _DeltaTime)
{
	{
		std::list<AActor*>::iterator StartIter = BeginPlayList.begin();
		std::list<AActor*>::iterator EndIter = BeginPlayList.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			AActor* CurActor = *StartIter;
			CurActor->BeginPlay();
			AllActors.push_back(CurActor);
		}

		BeginPlayList.clear();

		// todtjdtl 
		AActor::ComponentBeginPlay();
	}

	{
		std::list<AActor*>::iterator StartIter = AllActors.begin();
		std::list<AActor*>::iterator EndIter = AllActors.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			AActor* CurActor = *StartIter;

			if (false == CurActor->IsActive())
			{
				continue;
			}

			CurActor->Tick(_DeltaTime);
		}
	}
}

void ULevel::Render(float _DeltaTime)
{
	ScreenClear();

	// 지금 이제 랜더링의 주체가 USpriteRenderer 바뀌었다.
	// 액터를 기반으로 랜더링을 돌리는건 곧 지워질 겁니다.

	// 액터가 SpriteRenderer를 만들면
	// Level도 그 스프라이트 랜더러를 알아야 한다.

	if (true == IsCameraToMainPawn)
	{
		// CameraPivot = FVector2D(-1280, -720) * 0.5f;
		CameraPos = MainPawn->GetTransform().Location + CameraPivot;
	}


	std::map<int, std::list<class USpriteRenderer*>>::iterator StartOrderIter = Renderers.begin();
	std::map<int, std::list<class USpriteRenderer*>>::iterator EndOrderIter = Renderers.end();

	for (; StartOrderIter != EndOrderIter; ++StartOrderIter)
	{
		std::list<class USpriteRenderer*>& RendererList = StartOrderIter->second;

		std::list<class USpriteRenderer*>::iterator RenderStartIter = RendererList.begin();
		std::list<class USpriteRenderer*>::iterator RenderEndIter = RendererList.end();

		for (; RenderStartIter != RenderEndIter; ++RenderStartIter)
		{
			if (false == (*RenderStartIter)->IsActive())
			{
				continue;
			}

			(*RenderStartIter)->Render(_DeltaTime);
		}

	}

	UEngineDebug::PrintEngineDebugRender();

	DoubleBuffering();
}

void ULevel::Collision(float _DeltaTime)
{
	for (size_t i = 0; i < CollisionLink.size(); i++)
	{
		CollisionLinkData Data = CollisionLink[i];

		int Left = Data.Left;
		int Right = Data.Right;

		// 이벤트로 충돌체크하는 그룹
		std::list<class U2DCollision*>& LeftList = CheckCollisions[Left];

		// 그 대상은 이벤트 그룹이 아니어도 되므로 그냥 콜리전 모음에서 가져온다.
		std::list<class U2DCollision*>& RightList = Collisions[Right];

		std::list<class U2DCollision*>::iterator StartLeftIter = LeftList.begin();
		std::list<class U2DCollision*>::iterator EndLeftIter = LeftList.end();

		std::list<class U2DCollision*>::iterator StartRightIter = RightList.begin();
		std::list<class U2DCollision*>::iterator EndRightIter = RightList.end();

		for (; StartLeftIter != EndLeftIter; ++StartLeftIter)
		{
			U2DCollision* LeftCollision = *StartLeftIter;

			if (false == LeftCollision->IsActive())
			{
				continue;
			}

			for (; StartRightIter != EndRightIter; ++StartRightIter)
			{
				U2DCollision* RightCollision = *StartRightIter;
				if (false == RightCollision->IsActive())
				{
					continue;
				}

				LeftCollision->CollisionEventCheck(RightCollision);
			}
		}
	}
}

// 엔진 이벤트코드니까 이상한 곳에서 할필요가 없다.
// 컨텐츠에서는 존재하는지도 몰라야 한다.


void ULevel::Release(float _DeltaTime)
{
	// 릴리즈 순서는 말단부터 돌려야 합니다.
	std::list<AActor*>::iterator StartIter = AllActors.begin();
	std::list<AActor*>::iterator EndIter = AllActors.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		AActor* CurActor = *StartIter;
		CurActor->ReleaseTimeCheck(_DeltaTime);
	}

	// 충돌체 제거
	{
		std::map<int, std::list<class U2DCollision*>>::iterator StartOrderIter = Collisions.begin();
		std::map<int, std::list<class U2DCollision*>>::iterator EndOrderIter = Collisions.end();

		for (; StartOrderIter != EndOrderIter; ++StartOrderIter)
		{
			std::list<class U2DCollision*>& CollisionList = StartOrderIter->second;

			std::list<class U2DCollision*>::iterator CollisionStartIter = CollisionList.begin();
			std::list<class U2DCollision*>::iterator CollisionEndIter = CollisionList.end();

			// 언리얼은 중간에 삭제할수 없어.
			for (; CollisionStartIter != CollisionEndIter; )
			{
				if (false == (*CollisionStartIter)->IsDestroy())
				{
					++CollisionStartIter;
					continue;
				}

				// 랜더러는 지울 필요가 없습니다.
				// (*RenderStartIter) 누가 지울 권한을 가졌느냐.
				// 컴포넌트의 메모리를 삭제할수 권한은 오로지 액터만 가지고 있다.
				CollisionStartIter = CollisionList.erase(CollisionStartIter);
			}
		}
	}

	// 이벤트 충돌체 제거
	{
		std::map<int, std::list<class U2DCollision*>>::iterator StartOrderIter = CheckCollisions.begin();
		std::map<int, std::list<class U2DCollision*>>::iterator EndOrderIter = CheckCollisions.end();

		for (; StartOrderIter != EndOrderIter; ++StartOrderIter)
		{
			std::list<class U2DCollision*>& CollisionList = StartOrderIter->second;

			std::list<class U2DCollision*>::iterator CollisionStartIter = CollisionList.begin();
			std::list<class U2DCollision*>::iterator CollisionEndIter = CollisionList.end();

			// 언리얼은 중간에 삭제할수 없어.
			for (; CollisionStartIter != CollisionEndIter; )
			{
				if (false == (*CollisionStartIter)->IsDestroy())
				{
					++CollisionStartIter;
					continue;
				}

				// 랜더러는 지울 필요가 없습니다.
				// (*RenderStartIter) 누가 지울 권한을 가졌느냐.
				// 컴포넌트의 메모리를 삭제할수 권한은 오로지 액터만 가지고 있다.
				CollisionStartIter = CollisionList.erase(CollisionStartIter);
			}
		}
	}

	// 랜더러 제거
	{
		std::map<int, std::list<class USpriteRenderer*>>::iterator StartOrderIter = Renderers.begin();
		std::map<int, std::list<class USpriteRenderer*>>::iterator EndOrderIter = Renderers.end();

		for (; StartOrderIter != EndOrderIter; ++StartOrderIter)
		{
			std::list<class USpriteRenderer*>& RendererList = StartOrderIter->second;

			std::list<class USpriteRenderer*>::iterator RenderStartIter = RendererList.begin();
			std::list<class USpriteRenderer*>::iterator RenderEndIter = RendererList.end();

			// 언리얼은 중간에 삭제할수 없어.
			for (; RenderStartIter != RenderEndIter; )
			{
				if (false == (*RenderStartIter)->IsDestroy())
				{
					++RenderStartIter;
					continue;
				}

				// 랜더러는 지울 필요가 없습니다.
				// (*RenderStartIter) 누가 지울 권한을 가졌느냐.
				// 컴포넌트의 메모리를 삭제할수 권한은 오로지 액터만 가지고 있다.
				RenderStartIter = RendererList.erase(RenderStartIter);
			}
		}
	}

	// 액터 제거
	{
		std::list<AActor*>::iterator StartIter = AllActors.begin();
		std::list<AActor*>::iterator EndIter = AllActors.end();

		for (; StartIter != EndIter; )
		{
			AActor* CurActor = *StartIter;


			if (false == CurActor->IsDestroy())
			{
				CurActor->ReleaseCheck(_DeltaTime);
				++StartIter;
				continue;
			}

			// 레벨은 액터의 삭제권한을 가지고 있으니 액터는 진짜 지워 준다.
			delete CurActor;
			StartIter = AllActors.erase(StartIter);
		}
	}
}

void ULevel::ScreenClear()
{
	UEngineWindow& MainWindow = UEngineAPICore::GetCore()->GetMainWindow();
	UEngineWinImage* BackBufferImage = MainWindow.GetBackBuffer();
	FVector2D Size = MainWindow.GetWindowSize();

	Rectangle(BackBufferImage->GetDC(), -1, -1, Size.iX() + 2, Size.iY() + 2);
}

void ULevel::DoubleBuffering()
{
	// 레벨의 랜더링이 끝났다.
	UEngineWindow& MainWindow = UEngineAPICore::GetCore()->GetMainWindow();

	UEngineWinImage* WindowImage = MainWindow.GetWindowImage();
	UEngineWinImage* BackBufferImage = MainWindow.GetBackBuffer();

	FTransform Trans;
	Trans.Location = MainWindow.GetWindowSize().Half();
	Trans.Scale = MainWindow.GetWindowSize();

	// 이미지 들은 백버퍼에 다 그려졌을 것이다.
	BackBufferImage->CopyToBit(WindowImage, Trans);

}

void ULevel::PushRenderer(class USpriteRenderer* _Renderer)
{
	int Order = _Renderer->GetOrder();

	Renderers[Order].push_back(_Renderer);
}

void ULevel::PushCollision(U2DCollision* _Collision)
{
	int Order = _Collision->GetGroup();
	Collisions[Order].push_back(_Collision);
}

std::vector<CollisionLinkData> ULevel::CollisionLink;

void ULevel::PushCheckCollision(class U2DCollision* _Collision)
{
	int Order = _Collision->GetGroup();
	CheckCollisions[Order].push_back(_Collision);
}

void ULevel::ChangeRenderOrder(class USpriteRenderer* _Renderer, int _PrevOrder)
{
	//std::vector<int> Value;
	// 벡터는 리무브가 없다.
	//Value.remove

	// 0번에 들어있었을 것이다.
	// 별로 빠른 함수는 아닙니다.
	Renderers[_PrevOrder].remove(_Renderer);

	Renderers[_Renderer->GetOrder()].push_back(_Renderer);


}