#include "DataCollector.h"
#include <fstream>

DataCollector::DataCollector()
{
}

DataCollector::~DataCollector()
{
}

void DataCollector::AddData( U32 offset_into_array, U8* data, U32 data_length )
{
	DataChunk chunk;
	chunk.mBeginningOffset = offset_into_array;
	chunk.mData = data;
	chunk.mDataLength = data_length;

	mDataChunks.push_back( chunk );

	mSampleCount += ( data_length - offset_into_array );
}


void DataCollector::ExportDataToFile( char* file_name )
{
	std::fstream f;
	f.open( file_name, std::fstream::out );

	U64 sample_number = 0;
	U32 last_value = 0xFFFFFFFF;
	f << "Sample Number,Value" << std::endl;

	U32 chunk_count = mDataChunks.size();
	for( U32 i=0; i<chunk_count; i++ )
	{
		U32 byte_count = mDataChunks[i].mDataLength;
		U8* data = mDataChunks[i].mData;
		for( U32 j=mDataChunks[i].mBeginningOffset; j<byte_count; j++ )
		{
			U32 value = data[j];  //this is really an 8 bit number.
			if( value != last_value )
			{
				f << sample_number << "," << value << std::endl;
				last_value = value;
			}
			sample_number++;
		}
	}
	f.close();
}

void DataCollector::Reset()
{
	U32 count = mDataChunks.size();
	for( U32 i=0; i<count; i++ )
	{
		DevicesManagerInterface::DeleteU8ArrayPtr( mDataChunks[i].mData );
	}
	mDataChunks.clear();
	mSampleCount = 0;
}

U64 DataCollector::GetSampleCount()
{
	return mSampleCount;
}