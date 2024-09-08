#include "libs/vmmdll.h"
#include <string>
#include "Registry.h"
#include "Memory.h"

std::string c_registry::QueryValue(const char* path, e_registry_type type)
{
	if (!mem.vHandle)
		return "";

	BYTE buffer[0x128];
	auto _type = (DWORD)type;
	DWORD size = sizeof(buffer);

	if (!VMMDLL_WinReg_QueryValueExU(mem.vHandle, CC_TO_LPSTR(path), &_type, buffer, &size))
	{
		return "";
	}

    if (type == e_registry_type::dword)
    {
        DWORD dwordValue = *reinterpret_cast<DWORD*>(buffer);
        return std::to_string(dwordValue);
    }

	std::wstring wstr = std::wstring((wchar_t*)buffer);
	return std::string(wstr.begin(), wstr.end());
}
