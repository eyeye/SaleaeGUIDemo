#include <SaleaeDeviceApi.h>

#include <iostream>
#include <string>

void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data );
void __stdcall OnDisconnect( U64 device_id, void* user_data );
void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data );
void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data );
void __stdcall OnError( U64 device_id, void* user_data );

LogicInterface* gLogicInterface = NULL;
U64 gLogicId = 0;
U32 gSampleRateHz = 1000000;

#include "Trigger.h"
#include "DataCollector.h"

Trigger gTrigger;
DataCollector gDataCollector;

//Please Note:
//The trigger implemenation here is far from optimized.  To sample at higher than 1MHz, build for release mode.

//Change the path to your desktop, gDataCollector.ExportDataToFile( "C:\\Users\\Joe\\Desktop\\data.csv" ), below
//Change the sample rate (gSampleRateHz, above)
//Change the trigger (gTrigger.AddTriggerCondition, below)
//Change the number of samples to save: if( gDataCollector.GetSampleCount() > 10000000 ), below

int main( int argc, char *argv[] )
{
	DevicesManagerInterface::RegisterOnConnect( &OnConnect );
	DevicesManagerInterface::RegisterOnDisconnect( &OnDisconnect );
	DevicesManagerInterface::BeginConnect();

	std::cout << std::uppercase << "Logic is currently set up to read at " << gSampleRateHz << " Hz.  You can change this in the code." << std::endl;
	std::cout << std::uppercase << "This application is designed to wait until a trigger condition, and then collect a specific number of samples and save them to a file." << std::endl;

	//setup the trigger:
	TriggerCondition pin_0_low;
	pin_0_low.SetTriggerBitCondition( 0, Low );

	TriggerCondition pin_0_high;
	pin_0_high.SetTriggerBitCondition( 0, High );

	gTrigger.AddTriggerCondition(pin_0_low);
	gTrigger.AddTriggerCondition(pin_0_high);

	while( true )
	{
		std::cout << std::endl << std::endl << "Type s to start a capture, or e to exit. " << std::endl;

		std::string command;
		std::getline( std::cin, command );

		if( command == "exit" || command == "e" )
			break;

		if( command == "" )
			continue;

		if( gLogicInterface == NULL )
		{
			std::cout << "Sorry, no devices are connected." << std::endl;
			continue;
		} 

		if( command == "s" || command == "start" )
		{
			gTrigger.Reset();

			gLogicInterface->ReadStart();
		}
	}

	return 0;
}

void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data )
{
	if( dynamic_cast<LogicInterface*>( device_interface ) != NULL )
	{
		std::cout << "A Logic device was connected (id=0x" << std::hex << device_id << std::dec << ")." << std::endl;

		gLogicInterface = (LogicInterface*)device_interface;
		gLogicId = device_id;

		gLogicInterface->RegisterOnReadData( &OnReadData );
		gLogicInterface->RegisterOnWriteData( &OnWriteData );
		gLogicInterface->RegisterOnError( &OnError );

		gLogicInterface->SetSampleRateHz( gSampleRateHz );
	}
}

void __stdcall OnDisconnect( U64 device_id, void* user_data )
{
	if( device_id == gLogicId )
	{
		std::cout << "A Logic device was disconnceted (id=0x" << std::hex << device_id << std::dec << ")." << std::endl;
		gLogicInterface = NULL;
	}
}

void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data )
{
	if( gTrigger.IsTriggered() == false )
	{
		U32 i = 0;
		for( ; i<data_length; i++ )
		{
			if( gTrigger.DoesByteCompleteTrigger( data[i] ) == true )
			{
				gDataCollector.AddData( i, data, data_length );
				std::cout << "trigger found!" << std::endl;
				return;
			}
		}
		std::cout << "collected " << data_length << " samples; trigger not found yet." << std::endl;
		DevicesManagerInterface::DeleteU8ArrayPtr( data );
		return;
	}else
	{
		gDataCollector.AddData( 0, data, data_length );
	}

	if( gDataCollector.GetSampleCount() > 10000000 )
	{
		std::cout << "collection complete." << std::endl;

		gLogicInterface->Stop();
		gDataCollector.ExportDataToFile( "C:\\Users\\Joe\\Desktop\\data.csv" );
		gDataCollector.Reset();
	}else
	{
		std::cout << "Collected " << gDataCollector.GetSampleCount() << " samples so far." << std::endl;
	}
}

void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data )
{
}

void __stdcall OnError( U64 device_id, void* user_data )
{
	std::cout << "Logic Reported an Error.  This probably means that Logic could not keep up at the given data rate, or was disconnected. You can re-start the capture automatically, if your application can tolerate gaps in the data." << std::endl;
}