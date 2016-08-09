#include "client.hpp"

#include "XignCode.hpp"

#include "../XignCode Library/XignReader.hpp"
#include "../XignCode Library/XignWriter.hpp"

#include <thread>

namespace XignCode
{
	BOOL XIGNAPI _XignCode_initialize(const wchar_t* license_key, const wchar_t* xigncode_directory, int unknown)
	{
		printf("_XignCode_initialize - %ws, %ws, %d\n", license_key, xigncode_directory, unknown);
		return TRUE;
	}
	
	BOOL XIGNAPI _XignCode_uninitialize()
	{
		printf("_XignCode_uninitialize\n");
		return TRUE;
	}

	BOOL XIGNAPI _XignCode_start_service()
	{
		printf("_XignCode_start_service\n");
		return TRUE;
	}

	BOOL XIGNAPI _XignCode_stop_service()
	{
		printf("_XignCode_stop_service\n");
		return TRUE; 
	}
	
	BOOL XIGNAPI _XignCode_make_response(unsigned char* request, unsigned int size, _XignCode_heartbeat_callback_t response_callback, void* unknown)
	{
		static XignWriter writer;
		static DWORD last_tick = 0;

		if (last_tick == 0 || GetTickCount() - last_tick >= 5000)
		{
			last_tick = GetTickCount();

			printf("\n");
			writer.clear();

			std::thread thrd([&](_XignCode_heartbeat_callback_t callback) -> bool
			{
				while (GetTickCount() - last_tick < 5000)
				{
					Sleep(250);
				}
				
				network::client local_client(38666);
			
				bool result = local_client.make_interaction([&](network::client* client) -> bool
				{
					if (!client->write(writer.get().data(), writer.get().size()))
					{
						return false;
					}
					
					unsigned char data[8192];
					int size = client->read(data);
					
					if (!size)
					{
						return false;
					}

					XignReader reader(data, size);
				
					void* pointer = reinterpret_cast<void*>(reader.read<unsigned int>());

					unsigned char response_1[512];
					std::size_t response_1_size = reader.fetch(response_1);
				
					if (!response_1_size)
					{
						return false;
					}
					
					unsigned char response_2[512];
					std::size_t response_2_size = reader.fetch(response_2);
			
					if (!response_2_size)
					{
						return false;
					}
					
					printf("Heartbeat callback returned: %08X\n", callback(pointer, response_1, response_2, response_1_size, 0));
					return true;
				});
			
				printf("interaction returned: %d\n", result);
				return result;
			}, response_callback);

			thrd.detach();
		}
		else
		{
			last_tick = GetTickCount();
		}

		printf("_XignCode_make_response - %08X, %08X, %08X, %08X\n", request, size, response_callback, unknown);

		writer.append(request, size);
		return TRUE;
	}

	BOOL XIGNAPI _XignCode_set_error_callback(_XignCode_error_callback_t error_callback, void* unknown)
	{
		//printf("_XignCode_set_callback - %08X, %08X\n", error_callback, unknown);
		return TRUE;
	}
	
	BOOL XIGNAPI _XignCode_set_option(unsigned int option, void** param)
	{
		//printf("_XignCode_set_option - %08X, %08X\n", option, *param);
		return TRUE;
	}

	unsigned int XIGNAPI _XignCode_function_dispatch(void** function_ptr, FUNCTION_DISPATCH_TYPE function_type)
	{
		switch (function_type)
		{
		case FDT_INITIALIZE:
			*function_ptr = _XignCode_initialize;
			break;
		
		case FDT_UNINITIALIZE:
			*function_ptr = _XignCode_uninitialize;
			break;

		case FDT_START_SERVICE:
			*function_ptr = _XignCode_start_service;
			break;

		case FDT_STOP_SERVICE:
			*function_ptr = _XignCode_stop_service;
			break;

		case FDT_MAKE_RESPONSE:
			*function_ptr = _XignCode_make_response;
			break;
			
		case FDT_SET_ERROR_CALLBACK:
			*function_ptr = _XignCode_set_error_callback;
			break;
		
		case FDT_SET_OPTION:
			*function_ptr = _XignCode_set_option;
			break;

		default:
			printf("FunctionDispatch - function_ptr: %08X, function_type: %08X\n", function_ptr, function_type);
			break;
		}
		
		return 0;
	}
}