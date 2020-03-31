// Copyright 2018-2019, Athian Games. All Rights Reserved. 

#include "Widgets/ColorPicker.h"

#define LOCTEXT_NAMESPACE "UMG"

UColorPicker::UColorPicker()
{
	BorderColorAndOpacity = FLinearColor(0.3 , 0.3 , 0.3 , 0.8);
}

FLinearColor UColorPicker::OnGetColor() const
{
	return BorderColorAndOpacity;
}

void UColorPicker::OnBindingChanged(const FName& property)
{
	Super::OnBindingChanged(property);

}

TSharedRef<SWidget> UColorPicker::RebuildWidget()
{
	MyColorPickerBorder = SNew(SBorder);
	
	return MyColorPickerBorder.ToSharedRef();
}

void UColorPicker::SetColorPicker(FLinearColor InitialColor)
{
	TSharedPtr<SColorPicker> MyColorPicker = SNew(SColorPicker)
		.TargetColorAttribute(InitialColor)
		.UseAlpha(true)
		.OnlyRefreshOnMouseUp(false).OnInteractivePickBegin(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnEyeDropperPickBegin))
		.OnInteractivePickEnd(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnEyeDropperPickEnd))
		.OnlyRefreshOnOk(false)
		.ExpandAdvancedSection(true)
		.OnColorCommitted(BIND_UOBJECT_DELEGATE(FOnLinearColorValueChanged, HandleOnValueChanged));

	if (MyColorPickerBorder.IsValid())
	{
		MyColorPickerBorder->SetContent(MyColorPicker.ToSharedRef());
	}
}


void UColorPicker::HandleOnValueChanged(FLinearColor newcolor)
{
	SynchronizeProperties();
	OnColorValueChanged.Broadcast(newcolor);
	
}


void UColorPicker::HandleOnEyeDropperPickBegin()
{
}

void UColorPicker::HandleOnEyeDropperPickEnd()
{

}


 void UColorPicker::SynchronizeProperties()
 {
	 Super::SynchronizeProperties();

	 MyColorPickerBorder->SetBorderBackgroundColor(BorderColorAndOpacity);
 }

#if WITH_EDITOR

const FText UColorPicker::GetPaletteCategory()
{
	return LOCTEXT("ColoPicker", "ColoPicker");
}

#endif


#undef LOCTEXT_NAMESPACE
