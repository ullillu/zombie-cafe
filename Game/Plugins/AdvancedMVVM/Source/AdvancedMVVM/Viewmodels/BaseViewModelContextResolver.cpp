#include "BaseViewModelContextResolver.h"

#include "AdvancedMVVM/Interfaces/AdvancedMVVMInterface.h"

#include "Blueprint/UserWidget.h"

#include "BaseViewModel.h"

UObject* UBaseViewModelContextResolver::CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const
{
	if (UserWidget == nullptr || ExpectedType == nullptr)
	{
		return nullptr;
	}

	UBaseViewModel* NewBaseViewModel = NewObject<UBaseViewModel>(const_cast<UUserWidget*>(UserWidget), ExpectedType, NAME_None, RF_Transient);
	if (NewBaseViewModel != nullptr)
	{
		if (bOverrideDefaultProperties)
		{
			NewBaseViewModel->SetupProperties(DefaultProperties);
		}
		else if (UserWidget->Implements<UAdvancedMVVMInterface>())
		{
			NewBaseViewModel->SetupProperties(IAdvancedMVVMInterface::Execute_GetSettingsStruct(UserWidget));
		}

		// TODO: Find a way to determine whether a widget was created via a Widget Component or not, in order to avoid calling OnConstruct for widgets that were created by Widget Components.
		NewBaseViewModel->OnConstruct(UserWidget->GetOwningPlayer());
	}

	return NewBaseViewModel;
}

void UBaseViewModelContextResolver::DestroyInstance(const UObject* ViewModel, const UMVVMView* View) const
{
	if (UBaseViewModel* BaseViewModel = Cast<UBaseViewModel>(const_cast<UObject*>(ViewModel)))
	{
		BaseViewModel->OnDestruct();
	}
}
