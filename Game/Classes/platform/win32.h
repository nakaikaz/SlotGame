#ifndef __Platform_Win32_H__
#define __Platform_Win32_H__

#include <string>

namespace platform {

	namespace win32 {
		extern "C++" std::string MultiByteToUTF8(const char* src);
	};

}

#endif // __Platform_Win32_H__
