// Copyright 2018-2019, Athian Games. All Rights Reserved. 

#include "Widgets/ColorBlock.h"

#define LOCTEXT_NAMESPACE "ColorBlock"

UColorBlock::UColorBlock()
{
	CurrentColor = FLinearColor(1, 1, 1, 1);
}

FLinearColor UColorBlock::GetColor() const
{
	return CurrentColor;
}


TSharedRef<SWidget> UColorBlock::RebuildWidget()
{
	auto OnGetColor = [&]()
	{
		return CurrentColor;
	};

	MyColorBlock = SNew(SColorBlock)
		//.Color(this, &FColorGradingVectorCustomization::OnGetHeaderColorBlock)
		.ShowBackgroundForAlpha(false).Color_Lambda(OnGetColor)
		//.Color(this, &UColorBlock::GetColor)
		.IgnoreAlpha(true)
		.ColorIsHSV(false)
		.Size(FVector2D(70.0f, 12.0f));


	return MyColorBlock.ToSharedRef();

}

#if WITH_EDITOR

const FText UColorBlock::GetPaletteCategory()
{
	return LOCTEXT("ColoPicker", "ColoPicker");
}

#endif

#undef LOCTEXT_NAMESPACE

