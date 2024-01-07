// Project BOOM


#include "UI/BOOMHealthInformation.h"

#include "Components/ProgressBar.h"

void UBOOMHealthInformation::SetShieldBar(const float InPercent)
{
	ShieldBar->SetPercent(InPercent);
}

void UBOOMHealthInformation::SetHealthBar(const float InPercent)
{
	HealthBar->SetPercent(InPercent);

}
