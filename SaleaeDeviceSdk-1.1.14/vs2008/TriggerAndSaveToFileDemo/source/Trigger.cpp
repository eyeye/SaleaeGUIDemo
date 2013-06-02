#include "Trigger.h"
#include <vector>


TriggerCondition::TriggerCondition()
{
}

TriggerCondition::~TriggerCondition()
{
}

void TriggerCondition::SetTriggerBitCondition( U32 bit, TriggerBitCondition trigger_bit_condition )
{
	if( bit > 7 )
		return; //or assert, etc

	if( trigger_bit_condition == High )
	{
		mHighMasks.push_back( U8( 0x1 << bit ) );
	}else
	if( trigger_bit_condition == Low )
	{
		mLowMasks.push_back( U8( 0x1 << bit ) );
	}
}

bool TriggerCondition::DoesByteSatisfyTriggerCondition( U8 byte )
{

	U32 count = mHighMasks.size();
	for( U32 i=0; i<count; i++ )
	{
		if( ( byte & mHighMasks[i] ) == 0 )
			return false;
	}

	count = mLowMasks.size();
	for( U32 i=0; i<count; i++ )
	{
		if( ( byte & mLowMasks[i] ) != 0 )
			return false;
	}

	return true;
}


Trigger::Trigger()
:	mAlreadyTriggered(false),
	mState(0)
{
}

Trigger::~Trigger()
{
}

void Trigger::AddTriggerCondition( TriggerCondition trigger_condition )
{
	mTriggerConditions.push_back( trigger_condition );
}

bool Trigger::DoesByteCompleteTrigger( U8 byte )
{
	if( mTriggerConditions.size() == 0 )
		return true;

	if( mTriggerConditions[mState].DoesByteSatisfyTriggerCondition( byte ) == true )
	{
		mState++;
		if( mState == mTriggerConditions.size() )
		{
			mState = 0;
			mAlreadyTriggered = true;
			return true;
		}
	}else
	{
		//the byte doesn't match the new trigger condition.  
		//make sure it still matches the old one, otherwise reset.
		if( mState != 0 )
		{
			if( mTriggerConditions[mState-1].DoesByteSatisfyTriggerCondition( byte ) == false )
				mState = 0;
		}
	}

	return false;
}

bool Trigger::IsTriggered()
{
	return mAlreadyTriggered;
}


void Trigger::Reset()
{
	mState = 0;
	mAlreadyTriggered = false;
}
