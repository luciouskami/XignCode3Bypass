#include "XignCode.hpp"

namespace XignCode
{
	bool Initialize(std::wstring const& maplestory_directory)
	{
		/* Load MapleStory stub */
		HMODULE maplestory_module = LoadLibraryEx((maplestory_directory + std::wstring(L"\\CW.exe")).c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
		
		if (!maplestory_module)
		{
			return false;
		}

		__asm
		{ 
			push eax  
			push ecx
			mov eax,fs:[0x18]
			mov eax,[eax+0x30]
			mov ecx,maplestory_module
			mov [eax+0x08],ecx
			pop ecx
			pop eax
		};
		
		/* Load XignCode */
		std::wstring xigncode_directory = maplestory_directory + L"\\XIGNCODE";
		HMODULE xigncode_module = LoadLibrary((xigncode_directory + std::wstring(L"\\X3.XEM")).c_str());
	
		if (!xigncode_module)
		{
			return false;
		}
		
		/* Get the function dispatch */
		_XignCode_function_dispatch = reinterpret_cast<_XignCode_function_dispatch_t>(GetProcAddress(xigncode_module, reinterpret_cast<LPCSTR>(1)));

		if (!_XignCode_function_dispatch)
		{
			return false;
		}
		
		/* Handle the initialize function */
		_XignCode_initialize_t _XignCode_initialize = nullptr;

		if (_XignCode_function_dispatch(reinterpret_cast<void**>(&_XignCode_initialize), FDT_INITIALIZE) || _XignCode_initialize == nullptr)
		{
			return false;
		}

		if (!_XignCode_initialize(L"bD_cXsyitNtI", xigncode_directory.c_str(), 7))
		{
			return false;
		}

		/* Handle the error callback function */
		_XignCode_set_error_callback_t _XignCode_set_error_callback = nullptr;
		
		if (_XignCode_function_dispatch(reinterpret_cast<void**>(&_XignCode_set_error_callback), FDT_SET_ERROR_CALLBACK) || _XignCode_set_error_callback == nullptr)
		{
			return false;
		}

		_XignCode_error_callback_t _XignCode_error_callback = [](unsigned int error_code, unsigned int param_size, void* param, void* _4) -> unsigned int
		{
			//printf("_XignCode_error_callback: %08X\n", error_code);
			return 0;
		};
		
		if (!_XignCode_set_error_callback(_XignCode_error_callback, 0))
		{
			return false;
		}

		/* Handle the start service function */
		_XignCode_start_service_t _XignCode_start_service = nullptr;

		if (_XignCode_function_dispatch(reinterpret_cast<void**>(&_XignCode_start_service), FDT_START_SERVICE) || _XignCode_start_service == nullptr)
		{
			return false;
		}

		if (!_XignCode_start_service())
		{
			return false;
		}

		return true;
	}
	
	bool Uninitialize()
	{
		/* Handle the stop service function */
		_XignCode_stop_service_t _XignCode_stop_service = nullptr;

		if (_XignCode_function_dispatch(reinterpret_cast<void**>(&_XignCode_stop_service), FDT_STOP_SERVICE) || _XignCode_stop_service == nullptr)
		{
			return false;
		}

		if (!_XignCode_stop_service())
		{
			return false;
		}

		/* Handle the uninitialize function */
		_XignCode_uninitialize_t _XignCode_uninitialize = nullptr;

		if (_XignCode_function_dispatch(reinterpret_cast<void**>(&_XignCode_uninitialize), FDT_UNINITIALIZE) || _XignCode_uninitialize == nullptr)
		{
			return false;
		}

		if (!_XignCode_uninitialize())
		{
			return false;
		}

		return true;
	}

	bool MakeResponse(unsigned char* request, unsigned int size, _XignCode_heartbeat_callback_t heartbeat_callback)
	{
		_XignCode_make_response_t _XignCode_make_response = nullptr;

		if (_XignCode_function_dispatch(reinterpret_cast<void**>(&_XignCode_make_response), FDT_MAKE_RESPONSE) || _XignCode_make_response == nullptr)
		{
			return false;
		}

		if (!_XignCode_make_response(request, size, heartbeat_callback, 0))
		{
			return false;
		}

		return true;
	}
}