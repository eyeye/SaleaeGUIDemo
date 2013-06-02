#include <SaleaeDeviceApi.h>

#include <memory>
#include <iostream>
#include <string>

void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data );
void __stdcall OnDisconnect( U64 device_id, void* user_data );
void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data );
void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data );
void __stdcall OnError( U64 device_id, void* user_data );

#define USE_LOGIC_16 1

#if( USE_LOGIC_16 )
	Logic16Interface* gDeviceInterface = NULL;
#else
	LogicInterface* gDeviceInterface = NULL;
#endif

U64 gLogicId = 0;
U32 gSampleRateHz = 4000000;

int main( int argc, char *argv[] )
{
	DevicesManagerInterface::RegisterOnConnect( &OnConnect );
	DevicesManagerInterface::RegisterOnDisconnect( &OnDisconnect );
	DevicesManagerInterface::BeginConnect();

	std::cout << std::uppercase << "Devices are currently set up to read and write at " << gSampleRateHz << " Hz.  You can change this in the code." << std::endl;

	while( true )
	{
#if( USE_LOGIC_16 )
		std::cout << std::endl << std::endl << "You can type read, stop, or exit." << std::endl << "(r, s, and e for short)" << std::endl << std::endl;
#else
		std::cout << std::endl << std::endl << "You can type read, write, readbyte, writebyte, stop, or exit." << std::endl << "(r, w, rb, wb, s, and e for short)" << std::endl << std::endl;
#endif
		std::string command;
		std::getline( std::cin, command );

		if( command == "exit" || command == "e" )
			break;

		if( command == "" )
			continue;

		if( gDeviceInterface == NULL )
		{
			std::cout << "Sorry, no devices are connected." << std::endl;
			continue;
		} 

		if( command == "stop" || command == "s" )
		{
			if( gDeviceInterface->IsStreaming() == false )
				std::cout << "Sorry, the device is not currently streaming." << std::endl;
			else
				gDeviceInterface->Stop();

			continue;
		}

		if( gDeviceInterface->IsStreaming() == true )
		{
			std::cout << "Sorry, the device is already streaming." << std::endl;
			continue;
		}

		if( command == "read" || command == "r" )
		{
			gDeviceInterface->ReadStart();
		}
		else
		if( command == "write" || command == "w" )
		{	
#if( USE_LOGIC_16 )
			std::cout << "Sorry, output mode is not supported for Logic16" << std::endl;
			continue;
#else
			gDeviceInterface->WriteStart();
#endif
			
		}
		else
		if( command == "readbyte" || command == "rb" )
		{

#if( USE_LOGIC_16 )
			std::cout << "Sorry, reading a single value is not currently supported for Logic16" << std::endl;
			continue;
#else
			std::cout << "Got value 0x" << std::hex << U32( gDeviceInterface->GetInput() ) << std::dec << std::endl;
#endif
			
		}
		else
		if( command == "writebyte" || command == "wb" )
		{

#if( USE_LOGIC_16 )
			std::cout << "Sorry, output mode is not supported for Logic16" << std::endl;
			continue;
#else
			static U8 write_val = 0;

			gDeviceInterface->SetOutput( write_val );
			std::cout << "Logic is now outputting 0x" << std::hex << U32( write_val ) << std::dec << std::endl;
			write_val++;
#endif


		}
	}

	return 0;
}

void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data )
{
#if( USE_LOGIC_16 )

	if( dynamic_cast<Logic16Interface*>( device_interface ) != NULL )
	{
		std::cout << "A Logic16 device was connected (id=0x" << std::hex << device_id << std::dec << ")." << std::endl;

		gDeviceInterface = (Logic16Interface*)device_interface;
		gLogicId = device_id;

		gDeviceInterface->RegisterOnReadData( &OnReadData );
		gDeviceInterface->RegisterOnWriteData( &OnWriteData );
		gDeviceInterface->RegisterOnError( &OnError );

		U32 channels[16];
		for( U32 i=0; i<16; i++ )
			channels[i] = i;

		gDeviceInterface->SetActiveChannels( channels, 16 );
		gDeviceInterface->SetSampleRateHz( gSampleRateHz );
	}

#else

	if( dynamic_cast<LogicInterface*>( device_interface ) != NULL )
	{
		std::cout << "A Logic device was connected (id=0x" << std::hex << device_id << std::dec << ")." << std::endl;

		gDeviceInterface = (LogicInterface*)device_interface;
		gLogicId = device_id;

		gDeviceInterface->RegisterOnReadData( &OnReadData );
		gDeviceInterface->RegisterOnWriteData( &OnWriteData );
		gDeviceInterface->RegisterOnError( &OnError );

		gDeviceInterface->SetSampleRateHz( gSampleRateHz );
	}

#endif
}

void __stdcall OnDisconnect( U64 device_id, void* user_data )
{
	if( device_id == gLogicId )
	{
		std::cout << "A device was disconnected (id=0x" << std::hex << device_id << std::dec << ")." << std::endl;
		gDeviceInterface = NULL;
	}
}

void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data )
{
#if( USE_LOGIC_16 )
	std::cout << "Read " << data_length/2 << " words, starting with 0x" << std::hex << *(U16*)data << std::dec << std::endl;
#else
	std::cout << "Read " << data_length << " bytes, starting with 0x" << std::hex << (int)*data << std::dec << std::endl;
#endif

	//you own this data.  You don't have to delete it immediately, you could keep it and process it later, for example, or pass it to another thread for processing.
	DevicesManagerInterface::DeleteU8ArrayPtr( data );
}

void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data )
{
#if( USE_LOGIC_16 )

#else
	static U8 dat = 0;

	//it's our job to feed data to Logic whenever this function gets called.  Here we're just counting.
	//Note that you probably won't be able to get Logic to write data at faster than 4MHz (on Windows) do to some driver limitations.

	//here we're just filling the data with a 0-255 pattern.
	for( U32 i=0; i<data_length; i++ )
	{
		*data = dat;
		dat++;
		data++;
	}

	std::cout << "Wrote " << data_length << " bytes of data." << std::endl;
#endif
}

void __stdcall OnError( U64 device_id, void* user_data )
{
	std::cout << "A device reported an Error.  This probably means that it could not keep up at the given data rate, or was disconnected. You can re-start the capture automatically, if your application can tolerate gaps in the data." << std::endl;
	//note that you should not attempt to restart data collection from this function -- you'll need to do it from your main thread (or at least not the one that just called this function).
}