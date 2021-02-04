 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Item.h"
#include "VoxelTools/Gen/VoxelSphereTools.h"
#include "VoxelTools/VoxelDataTools.h"
#include "VoxelData/VoxelData.h"
#include "BioProgrammator.h"
#include "Item.h"
#include "Equipment.h"
#include "UsableItem.h"
#include <map>
#include "Utilites.h"
#include "chunkBox.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class CAVES_TAINT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& objectInitializer);

protected: 
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//Main function 
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	//My variables

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isGuiOpen = false;												//true if any gui is open

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool showHUD = true;                                                //true if HUD should be visible

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool showItemStats = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		APlayerController *playerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent *characterView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
		USkeletalMesh* skeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = SkeletalMesh)
		USkeletalMeshComponent* characterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* itemInHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UItem*> itemsInEquipment;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//TArray<UItem*> itemsInToolBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 selectedItem = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UUIBase*> currentlyOpenUIs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isItemDraggableGuiOpen = false;
	//My function 

	UFUNCTION()
		void MoveForward(float Value);

	UFUNCTION()
		void MoveRight(float Value);

	UFUNCTION()
		void StartJump();

	UFUNCTION()
		void StopJump();

	UFUNCTION()
		void turnVertical(float value);

	UFUNCTION()
		void turnHorizontal(float value);

	UFUNCTION()
		void action();																													//if left click on surface, dig a hole																

	UFUNCTION()
		void placeItem();																												//if right click on empty space, place or use holded item, if clicked on usable item,
																																		//uses it

	UFUNCTION()
		void materialsInSphere(TArray<FModifiedVoxelValue> &modifiedValues, AVoxelWorld* voxelWorldReference);							//check materials ids in sphere, then give items to equipment based on these ids

	UFUNCTION()
		int32 isInTheInventory(FName itemID);																							//return the first instance of item with given name  in inventory

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UItem* dragItem;																												//currently holding item, used to drag items from one gui to another

	UFUNCTION(BlueprintCallable)
		void addItemToEquipment(FName objectID, UClass* objectClass, bool stackable, bool splitStack = false, int32 stackAmount = 1)
		{
			if (stackable)
			{
				UItem* stackableItem = NewObject<UItem>();

				stackableItem->itemName = objectID;
				stackableItem->placedItemClass = objectClass;

				int32 equipmentIndex = isInTheInventory(stackableItem->itemName);

				if (equipmentIndex == -1 || splitStack)
				{
					stackableItem->stack = stackAmount;
					itemsInEquipment.Add(stackableItem);
					itemsInEquipment[itemsInEquipment.Num() - 1]->equipmentIndex = itemsInEquipment.Num() - 1;
				}
				else
				{
					Cast<UItem>(itemsInEquipment[equipmentIndex])->stack += 1;
				}
			}
			else
			{
				UItem* createdItem = NewObject<UItem>();

				createdItem->itemName = objectID;
				createdItem->placedItemClass = objectClass;

				itemsInEquipment.Add(createdItem);
				itemsInEquipment[itemsInEquipment.Num() - 1]->equipmentIndex = itemsInEquipment.Num() - 1;
			}
		}

	UFUNCTION(BlueprintCallable)
		void addItemReferenceToEquipment(UItem* itemReference);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UEquipment *equipment;;

	UFUNCTION(BlueprintCallable)
		void insertItemToEquipment(FName objectID, UClass* objectClass, bool stackable, int32 index, int32 stack);							//construct item, then inserts it into equipment at given index

	UFUNCTION(BlueprintCallable)
		void insertItemReferenceToEquipment(UItem *itemReference, int32 index);																//inserts item into equipment at given index

	UFUNCTION(BlueprintCallable)
		void removeItemFromEquipment(int32 index, bool removeWholeStack = false, int32 removeMoreThanOneItem = 1);							//remove item at given index, can remove whole stack or only given number

	UFUNCTION(BlueprintCallable)
		void moveItems(UItem* source, UPARAM(ref) UItem*& destination, int32 stack, int32 maxStack, bool cutStack = true);					//move stacks from one UItem to another

	UFUNCTION(BlueprintCallable)
		void checkItemToRemove(int32 index);																								//if UItem's stack in equipment at given index is equal to 0, remove it

	UFUNCTION(BlueprintCallable)
		void updateItemIndexes();																											//Update indexes in UItem's

	UFUNCTION(BlueprintCallable)
		FVector getPlayerCube();																											//return chunk, player is currently in

	UFUNCTION(BlueprintCallable)
		void checkChunks();																													//checks if new chunk should be spawned
		
	UFUNCTION(BlueprintCallable)
		void removeChunks();																												//checks if chunk should be removed

	UFUNCTION(BlueprintCallable)
		void changeItemMeshInHand();																										//chenge mesh in hand to the item that is currently in use

	UFUNCTION(BlueprintCallable)
		int32 findChunk(FVector position);																									//checks if chunks with given position exists, if not, return -1
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AchunkBox*> chunks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector lastChunk;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 renderRadius = 3;

};
