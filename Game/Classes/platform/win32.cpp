#include "win32.h"
#include <Windows.h>

namespace platform {

	namespace win32 {
		std::string MultiByteToUTF8(const char* src)
		{
			std::string ret;
			unsigned int size = ::MultiByteToWideChar(CP_ACP, 0, src, -1, nullptr, 0);
			wchar_t* widebuffer = new wchar_t[size];
			::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, widebuffer, size);
			size = ::WideCharToMultiByte(CP_UTF8, 0, widebuffer, -1, nullptr, 0, nullptr, nullptr);
			char* utf8buffer = new char[size + 1];
			::WideCharToMultiByte(CP_UTF8, 0, widebuffer, -1, utf8buffer, size, nullptr, nullptr);
			delete[] widebuffer;
			ret = utf8buffer;
			delete[] utf8buffer;

			return ret;
		}
	}
}