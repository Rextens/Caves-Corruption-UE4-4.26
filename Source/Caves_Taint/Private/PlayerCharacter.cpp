// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "UObject/Class.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "PlacedRock.h"
#include "NONEReference.h"
#include "SaveWorld.h"
#include "CharacterController.h"
#include "VoxelWorld.h"


// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//characterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character's mesh"));
	//characterMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> testMannequin(TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin"));
	//const ConstructorHelpers::FObjectFinder<UAnimationAsset> walkAnimation(TEXT("/Game/AnimStarterPack/Walk_Fwd_Rifle_Ironsights"));



	if (testMannequin.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(testMannequin.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->AddWorldRotation(FRotator(0.0f, 270.0f, 0.0f));

		/*
		if (walkAnimation.Succeeded())
		{
			GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			//GetMesh()->SetAnimation(walkAnimation.Object);
			//GetMesh()->Play(true);
			GetMesh()->PlayAnimation(walkAnimation.Object, true);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Loaded"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Could not load animation"));
		}
		*/
		characterView = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
		characterView->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "head");//FName("head"), EAttachLocation::KeepRelativeOffset, true);
		characterView->AddRelativeRotation(FRotator(180.0f, -90.0f, 90.0f));
		characterView->bUsePawnControlRotation = true;
		//bUseControllerRotationPitch = true;
		characterView->bLockToHmd = true;
	}
	itemInHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item in hand mesh"));
	itemInHand->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "hand_l");
	itemInHand->AddLocalOffset(FVector(8.0f, -8.0f, 0.0f));


	//GetMesh()
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	addItemToEquipment("Bioprogrammator", ABioProgrammator::StaticClass(), true, false, 10);
	addItemToEquipment("NotStackable", nullptr, false, false, 10);

	playerController = Cast<ACharacterController>(GetController());
	
	if (playerController)
	{
		playerController->bEnableClickEvents = true;
		playerController->bEnableMouseOverEvents = true;
	}	

	USaveWorld* saveWorldInstance = Cast<USaveWorld>(UGameplayStatics::CreateSaveGameObject(USaveWorld::StaticClass()));

	if (saveWorldInstance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("loaded"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("not loaded"));
	}

//	for (int i = 0; i < 9; ++i)
//	{
//		itemsInToolBar.Add(nullptr);
//	}

	FVector test0 = FVector(15.0f, 15.0f, 15.0f);
	FVector test1 = FVector(15.0f, 15.0f, 15.0f);
		test0 = test0 + 2000.0f;

		bool test2 = false;

		if (test0 == test1 + 2000.0f)
		{
			test2 = true;
		}
	
		int32 test3;

		if (test2)
		{
			test3 = 1;
		}
		else
		{
			test3 = 0;
		}

//	if (GEngine)
//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("X: %f, Y: %f, Z: %f"), test0.X, test0.Y, test0.Z));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("test: %i"), test3));
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Cast<UCavesCorruptionGameInstance>(GetGameInstance())->currentWorldName != "")
	{
		checkChunks();
		removeChunks();
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::turnHorizontal);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::turnVertical);

	//PlayerInputComponent->BindAxis("ChooseItem", this, &APlayerCharacter::turnMouseWheel);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJump);

	//PlayerInputComponent->BindAction("Open/Close Gui", IE_Pressed, this, &APlayerCharacter::openEquipment);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &APlayerCharacter::action);
	PlayerInputComponent->BindAction("Action2", IE_Pressed, this, &APlayerCharacter::placeItem);
}

void APlayerCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();
}



void APlayerCharacter::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.

	if (!isGuiOpen)
	{
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	if (!isGuiOpen)
	{
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::StartJump()
{
	if (!isGuiOpen)
	{
		bPressedJump = true;
	}
}

void APlayerCharacter::StopJump()
{
	if (!isGuiOpen)
	{
		bPressedJump = false;
	}
}

void APlayerCharacter::turnVertical(float value)
{
	if (!isGuiOpen)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Pitch: %f"), value));
		APlayerCharacter::AddControllerPitchInput(value);
	}
}

void APlayerCharacter::turnHorizontal(float value)
{
	if (!isGuiOpen)
	{
		APlayerCharacter::AddControllerYawInput(value);
	}
}

void APlayerCharacter::materialsInSphere(TArray<FModifiedVoxelValue> &modifiedValues, AVoxelWorld *voxelWorldReference)
{
	for (int i = 0; i < modifiedValues.Num(); ++i) {
		FVoxelMaterial gettedMaterial;
		UVoxelDataTools::GetMaterial(gettedMaterial, voxelWorldReference, modifiedValues[i].Position);
	
		if (gettedMaterial.GetSingleIndex() == 0 || gettedMaterial.GetSingleIndex() == 1) {
			addItemToEquipment("Shattered Stone", nullptr, true, false);
		}
		else if(gettedMaterial.GetSingleIndex() == 2)
		{
			addItemToEquipment("Selfprogrammable Ore", nullptr, true, false);
		}
	}
}

void APlayerCharacter::action() 
{
	if (!isGuiOpen)
	{
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult, characterView->GetComponentLocation(), characterView->GetForwardVector() * 1000.0f + characterView->GetComponentLocation(), ECC_Visibility);
		DrawDebugLine(GetWorld(), characterView->GetComponentLocation(), characterView->GetForwardVector() * 1000.0f + characterView->GetComponentLocation(), FColor::Red, false, 2.0f);

		AVoxelWorld* voxelWorldReference = nullptr;

		voxelWorldReference = Cast<AVoxelWorld>(hitResult.Actor);

		if (voxelWorldReference != nullptr)
		{			
			TArray<FModifiedVoxelValue> modifiedValues;

			UVoxelSphereTools::RemoveSphere(voxelWorldReference, hitResult.Location, 20.0f, &modifiedValues);
			UVoxelSphereTools::SmoothSphere(voxelWorldReference, hitResult.Location, 20.0f, 1.0f);
			materialsInSphere(modifiedValues, voxelWorldReference);
		}
	}
}

void APlayerCharacter::placeItem()
{
	if (!isGuiOpen)
	{
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult, characterView->GetComponentLocation(), characterView->GetForwardVector() * 1000.0f + characterView->GetComponentLocation(), ECC_Visibility);
		DrawDebugLine(GetWorld(), characterView->GetComponentLocation(), characterView->GetForwardVector() * 1000.0f + characterView->GetComponentLocation(), FColor::Red, false, 2.0f);

		
		//AActiveableItem* activeable = Cast<AActiveableItem>(hitResult.Actor);

		if (!hitResult.Actor->GetClass()->ImplementsInterface(UUsableItem::StaticClass()))
		{
			if (selectedItem < itemsInEquipment.Num())
			{
				if (itemsInEquipment[selectedItem]->placedItemClass != nullptr && itemsInEquipment[selectedItem] != nullptr)
				{
					FRotator Rotation(0.0f, 0.0f, 0.0f);
					FActorSpawnParameters SpawnInfo;

					GetWorld()->SpawnActor<APlacedItem>(itemsInEquipment[selectedItem]->placedItemClass, hitResult.Location, Rotation, SpawnInfo);
					removeItemFromEquipment(itemsInEquipment[selectedItem]->equipmentIndex);
				}
			}
		}
		else
		{
			Cast<IUsableItem>(hitResult.Actor)->activation();
		}
	}
}

int32 APlayerCharacter::isInTheInventory(FName itemID)
{
	for (int i = 0; i < itemsInEquipment.Num(); ++i)
	{
		if (itemsInEquipment[i]->itemName == itemID)
		{
			return i;
		}
	}

	return -1;
}

void APlayerCharacter::addItemReferenceToEquipment(UItem* itemReference)
{
	if (itemReference != nullptr)
	{
		if (itemReference->stackable)
		{
			UItem* stackableItem = NewObject<UItem>();

			stackableItem->itemName = itemReference->itemName;
			stackableItem->placedItemClass = itemReference->placedItemClass;

			stackableItem->stack = Cast<UItem>(itemReference)->stack;
			itemsInEquipment.Add(stackableItem);
			itemsInEquipment[itemsInEquipment.Num() - 1]->equipmentIndex = itemsInEquipment.Num() - 1;
		}
		else
		{
			UItem* createdItem = NewObject<UItem>();

			createdItem->itemName = itemReference->itemName;
			createdItem->placedItemClass = itemReference->placedItemClass;

			itemsInEquipment.Add(createdItem);
			itemsInEquipment[itemsInEquipment.Num() - 1]->equipmentIndex = itemsInEquipment.Num() - 1;
		}
	}
}

void APlayerCharacter::insertItemToEquipment(FName objectID, UClass* objectClass, bool stackable, int32 index, int32 stack)
{
	if (stackable)
	{
		UItem* stackableItem = NewObject<UItem>();

		stackableItem->itemName = objectID;
		stackableItem->placedItemClass = objectClass;
		stackableItem->stack = stack;

		itemsInEquipment.SetNum(itemsInEquipment.Num() + 1);

		for (int i = itemsInEquipment.Num() - 1; i >= index + 1; --i)
		{
			itemsInEquipment[i] = itemsInEquipment[i - 1];
		}

		itemsInEquipment[index] = stackableItem;
	}
	else
	{
		UItem* createdItem = NewObject<UItem>();
		
		createdItem->itemName = objectID;
		createdItem->placedItemClass = objectClass;

		itemsInEquipment.Add(nullptr);

		for (int i = itemsInEquipment.Num(); i-- > index; )
		{
			itemsInEquipment[i] = itemsInEquipment[i + 1];
		}

		itemsInEquipment[index] = createdItem;
	}
}

void APlayerCharacter::insertItemReferenceToEquipment(UItem* itemReference, int32 index)
{
	if (itemReference->stackable)
	{
		UItem* stackableItemReference = Cast<UItem>(DuplicateObject<UItem>(itemReference, this));

		itemsInEquipment.SetNum(itemsInEquipment.Num() + 1);

		for (int i = itemsInEquipment.Num() - 1; i >= index + 1; --i)
		{
			itemsInEquipment[i] = itemsInEquipment[i - 1];
		}

		itemsInEquipment[index] = stackableItemReference;
	}
	else
	{
		UItem* duplicatedItemReference = DuplicateObject<UItem>(itemReference, this);

		itemsInEquipment.SetNum(itemsInEquipment.Num() + 1);

		for (int i = itemsInEquipment.Num() - 1; i >= index + 1; --i)
		{
			itemsInEquipment[i] = itemsInEquipment[i - 1];
		}

		itemsInEquipment[index] = duplicatedItemReference;
	}

	updateItemIndexes();
}


void APlayerCharacter::removeItemFromEquipment(int32 index, bool removeWholeStack, int32 removeMoreThanOneItem)
{
	if (itemsInEquipment[index]->stackable)
	{
		if (removeWholeStack)
		{
			itemsInEquipment.RemoveAt(index);
		}
		else
		{
			UItem* stackableItemReference = Cast<UItem>(itemsInEquipment[index]);
			stackableItemReference->stack -= removeMoreThanOneItem;

			if (stackableItemReference->stack == 0)
			{
				itemsInEquipment.RemoveAt(index);
			}
		}
	}
	else
	{
		itemsInEquipment.RemoveAt(index);
	}

	updateItemIndexes();
}

void APlayerCharacter::moveItems(UItem* source, UPARAM(ref) UItem*& destination, int32 stack, int32 maxStack, bool cutStack)
{
	if (source != nullptr)
	{
		if (source->stack < stack)
		{
			stack = source->stack;
		}
		if (destination != nullptr && destination->stack < maxStack)
		{
			if (destination->stack + stack <= maxStack)
			{
				if (destination->itemName == source->itemName)
				{
					destination->stack += stack;
					source->stack -= stack;
				}
			}
			else if (cutStack)
			{

			}
		}
		else if(destination == nullptr)
		{
			destination = DuplicateObject(source, this);
			destination->stack = stack;
			source->stack -= stack;
		}
	}
}

void APlayerCharacter::checkItemToRemove(int32 index)
{
	if (itemsInEquipment[index]->stackable)
	{
		UItem* stackableItemReference = Cast<UItem>(itemsInEquipment[index]);
		if (stackableItemReference->stack == 0)
		{
			itemsInEquipment.RemoveAt(index);
			updateItemIndexes();
		}
	}
}

void APlayerCharacter::updateItemIndexes()
{
	for (int i = 0; i < itemsInEquipment.Num(); ++i)
	{
		itemsInEquipment[i]->equipmentIndex = i;
	}
}

FVector APlayerCharacter::getPlayerCube()
{
	return FVector(floor(GetActorLocation().X / 4096.0f), floor(GetActorLocation().Y / 4096.0f), floor(GetActorLocation().Z / 4096.0f));
}

void APlayerCharacter::checkChunks()
{
	FVector playerChunk = getPlayerCube();
	for (int x = -renderRadius; x < renderRadius; ++x)
	{
		for (int y = -renderRadius; y < renderRadius; ++y)
		{
			for (int z = -renderRadius; z < renderRadius; ++z)
			{
				FVector currentChunk = getPlayerCube();
				FVector finalLocation = FVector(4096.0f * (currentChunk.X + x) + 2048.0f, 4096.0f * (currentChunk.Y + y) + 2048.0f, 4096.0f * (currentChunk.Z + z) + 2048.0f);
				
				if (findChunk(finalLocation) == -1)
				{
					chunks.Add(GetWorld()->SpawnActor<AchunkBox>());
					chunks[chunks.Num() - 1]->SetActorLocation(finalLocation);
					chunks[chunks.Num() - 1]->onSpawned();
				}
			}
		}
	}
}

void APlayerCharacter::removeChunks()
{
	for (int i = 0; i < chunks.Num(); ++i)
	{
		if (abs(floor(chunks[i]->GetActorLocation().X / 4096.0f) - getPlayerCube().X) > renderRadius ||
			abs(floor(chunks[i]->GetActorLocation().Y / 4096.0f) - getPlayerCube().Y) > renderRadius ||
			abs(floor(chunks[i]->GetActorLocation().Z / 4096.0f) - getPlayerCube().Z) > renderRadius)
		{
			chunks[i]->DestroyChunk(false);
			chunks[i]->Destroy();
			chunks.RemoveAt(i);
		}
	}
}

int32 APlayerCharacter::findChunk(FVector position)
{
	for (int i = 0; i < chunks.Num(); ++i)
	{
		if (chunks[i]->GetActorLocation() == position)
		{
			return i;
		}
	}
	return -1;
}

void APlayerCharacter::changeItemMeshInHand()
{
	if (selectedItem < itemsInEquipment.Num())
	{
		if (itemsInEquipment[selectedItem]->inHandMesh != nullptr)
		{
			FVector currentSize = itemsInEquipment[selectedItem]->inHandMesh->GetBounds().GetBox().GetSize();
			
			float getHightestValue = currentSize.X;
			if (currentSize.Y > getHightestValue)
			{
				getHightestValue = currentSize.Y;
			}
			if (currentSize.Z > getHightestValue)
			{
				getHightestValue = currentSize.Z;
			}
			 
			float scale = 10 / getHightestValue;
				
			itemInHand->SetWorldScale3D(FVector(scale));
			itemInHand->SetStaticMesh(itemsInEquipment[selectedItem]->inHandMesh);
		}
		else
		{
			itemInHand->SetStaticMesh(nullptr);
		}
	}
	else
	{
		itemInHand->SetStaticMesh(nullptr);
	}
}