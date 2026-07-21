// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// generated.h는 모든 헤더 중 마지막에 있어야 한다.
#include "TestActor.generated.h"

// UCLASS()
// - C++ 클래스를 언리얼 엔진의 객체 시스템에 등록(= UObject)
// - 가비지 콜렉팅 지원, 블루프린트 연동, 에디터서 노출 등이 가능해짐
// - 다중 상속을 막는다.

// CDO (Class Default Object)
// - 기본 클래스 인스턴스
// - 새 클래스가 인스턴싱될 때 CDO를 클로닝해서 사용
// - 기본값
// - 나중에 따로 한 번 찾아보시면 좋아요~

UCLASS()
class CPP_HELLOUNREAL_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;	// Tick을 protected로 변경

protected :
	int32 Data1 = 10;	// C++ 전용 변수. 블루프린트에서 보이지 않고, 접근도 안 됨. C++에서 내부적으로 사용하는 용도. 포인터인 경우에는 가비지 콜렉팅도 안 됨.
	
	// UPROPERTY: 클래스나 구조체의 멤버 변수를 언리얼 엔진에 등록하는 것 (리플렉션 지원, 가비지 콜렉팅 지원)
	UPROPERTY(VisibleAnywhere, Category = "TestActor 변수|Visible")	// 클래스 디폴트 창과, 맵에 배치된 액터의 디테일 창에서 확인 가능
	int32 Data2_1 = 21;

	UPROPERTY(VisibleDefaultsOnly, Category = "TestActor 변수|Visible")	// 클래스 디폴트 창에서만 보임
	int32 Data2_2 = 22;
	
	UPROPERTY(VisibleInstanceOnly, Category = "TestActor 변수|Visible")	// 맵에 배치된 액터의 디테일 창에서만 보임
	int32 Data2_3 = 23;

	UPROPERTY(EditAnywhere, Category = "TestActor 변수|Editable")	// 클래스 디폴트 창과 맵에 배치된 액터의 디테일 창에서 수정 가능
	int32 Data3_1 = 31;
	
	UPROPERTY(EditDefaultsOnly, Category = "TestActor 변수|Editable")	// 클래스 디폴트 창에서만 수정 가능
	int32 Data3_2 = 32;
	
	UPROPERTY(EditInstanceOnly, Category = "TestActor 변수|Editable")	// 맵에 배치된 액터의 디테일 창에서만 수정 가능
	int32 Data3_3 = 33;
	
	UPROPERTY(BlueprintReadOnly, Category = "TestActor 변수|Read")	// 블루프린트 이벤트 그래프에서 에서 읽기 가능
	int32 Data4_1 = 41;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "TestActor 변수|Read")	// 블루프린트 이벤트 그래프에서 읽을 수 있고, 디테일 창에서 편집도 가능
	int32 Data4_2 = 42;

	UPROPERTY(BlueprintReadWrite, Category = "TestActor 변수|ReadWrite")	// 블루프린트 이벤트 그래프에서 읽고 쓰기 가능
	int32 Data4_3 = 43;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TestActor 변수|ReadWrite")	// 블루프린트 이벤트 그래프에서 읽고 쓸 수 있고, 디테일 창에서 편집도 가능
	int32 Data4_4 = 44;
	
	// 옛날 방식: 메모리 관리가 제대로 안 될 수 있음 -> 댕글링 포인터 발생 가능
	//UStaticMeshComponent* MainMesh = nullptr;
	// 요즘 권장: 댕글링 포인터 처리 등이 자동으로 됨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MainMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 10.f;

protected :

	void Test_NormalFunction();
	
	UFUNCTION(BlueprintCallable)	// C++로 작성한 함수를 블루프린트에서 호출할 수 있다.
	void Test_UFunction();

	UFUNCTION(BlueprintImplementableEvent)	//C++에서 선언하고 구현은 블루프린트에서 하겠다.
	void Test_ImplementableFunction();		// (블루프린트 코드를 써야만 하는 상황일 때 사용. CPP에 구현하면 안 됨)

	UFUNCTION(BlueprintNativeEvent)		// C++에서 선언과 구현을 했는데, 블루프린트에서 재정의할 수도 있다.
	void Test_NativeEventFunction();	// (구현은 _Implementation을 붙여서 구현해야 한다.) (호출은 원래 이름 그대로)

	//UFUNCTION(BlueprintPure)
	//void Test_PureFunction();



};
