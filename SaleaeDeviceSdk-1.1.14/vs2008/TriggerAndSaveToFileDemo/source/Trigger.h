#ifndef TRIGGER_H
#define TRIGGER_H

#include <SaleaeDeviceApi.h>


enum TriggerBitCondition { High, Low, DontCare };

class TriggerCondition
{
public:
	TriggerCondition();
	~TriggerCondition();

	void SetTriggerBitCondition( U32 bit, TriggerBitCondition trigger_bit_condition );

	bool DoesByteSatisfyTriggerCondition( U8 byte );

protected:
	std::vector<U8> mHighMasks;
	std::vector<U8> mLowMasks;
};

class Trigger
{
public:
	Trigger();
	~Trigger();

	void AddTriggerCondition( TriggerCondition trigger_condition );
	bool DoesByteCompleteTrigger( U8 byte );
	bool IsTriggered();
	void Reset();

protected:
	bool mAlreadyTriggered;
	U32 mState;
	std::vector<TriggerCondition> mTriggerConditions;
};


#endif //TRIGGER_H