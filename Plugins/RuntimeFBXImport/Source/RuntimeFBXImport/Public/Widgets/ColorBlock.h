// Copyright 2018-2019, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/SWidget.h"
#include "Widgets/Colors/SColorBlock.h"
#include "ColorBlock.generated.h"

/**
 * UMG Widget control which adds a Color Block in Viewport
 */
UCLASS()
class UColorBlock : public UWidget
{
	GENERATED_BODY()
	
	
	TSharedPtr<SColorBlock> MyColorBlock;

	UColorBlock();

	FLinearColor GetColor() const;

protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;

public:

	/** Current color of the color block. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
		FLinearColor CurrentColor;


#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif


};
