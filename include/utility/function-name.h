#ifndef __UTILITY_FUNCTION_NAME_H__
#define __UTILITY_FUNCTION_NAME_H__

#ifndef __FUNCTION_NAME__
	#ifdef WIN32	//WINDOWS
		#define __FUNCTION_NAME__ __FUNCTION__
	#else			//*NIX
		#define __FUNCTION_NAME__ __func__
	#endif
#endif

#endif