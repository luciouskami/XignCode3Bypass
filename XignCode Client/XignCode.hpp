#pragma once

#include "../XignCode Library/XignDefs.hpp"

#include <stdio.h>

namespace XignCode
{
	BOOL XIGNAPI _XignCode_initialize(const wchar_t* license_key, const wchar_t* xigncode_directory, int unknown);
	BOOL XIGNAPI _XignCode_uninitialize();

	BOOL XIGNAPI _XignCode_start_service();
	BOOL XIGNAPI _XignCode_stop_service();
	
	BOOL XIGNAPI _XignCode_make_response(unsigned char* request, unsigned int size, _XignCode_heartbeat_callback_t response_callback, void* unknown);
	
	BOOL XIGNAPI _XignCode_set_error_callback(_XignCode_error_callback_t error_callback, void* unknown);
	BOOL XIGNAPI _XignCode_set_option(unsigned int option, void** param);

	unsigned int XIGNAPI _XignCode_function_dispatch(void** function_ptr, FUNCTION_DISPATCH_TYPE function_type);
}