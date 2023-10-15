// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BOOM_API IPoolableObjectInterface
{
	GENERATED_BODY()

	virtual void InitActor() = 0;
	virtual void ReturnActorToPool() = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
