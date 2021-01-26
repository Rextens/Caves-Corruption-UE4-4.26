// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"


UItem::UItem()
{
	itemName = "notNUll";
	placedItemClass = APlacedRock::StaticClass();
	stack = 1;
	stackable = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> itemHoldMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_TriPyramid.Shape_TriPyramid'"));
	inHandMesh = itemHoldMesh.Object;
	//inHandMesh->GetBounds().GetBox().GetSize();
}