#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

#include <SaleaeDeviceApi.h>
#include <vector>

struct DataChunk
{
	U32 mBeginningOffset;  //used for data that triggers in the middle of an array
	U8* mData;
	U32 mDataLength;
};

class DataCollector
{
public:
	DataCollector();
	~DataCollector();

	void AddData( U32 offset_into_array, U8* data, U32 data_length );
	void ExportDataToFile( char* file_name );
	void Reset();
	U64 GetSampleCount();

protected:
	std::vector<DataChunk> mDataChunks;
	U64 mSampleCount;
};


#endif //DATA_COLLECTOR_H