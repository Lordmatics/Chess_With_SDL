#include "Utils.h"
#include "ChessUser.h"
#include "Player.h"								 
#include "BasicAI.h"
#include <stdarg.h>
#include <stdio.h>

const char* Utils::GetChessUser(ChessUser* pUser)
{
	if (ChessUser* pOwner = pUser)
	{
		if (Player* pPlayer = dynamic_cast<Player*>(pOwner))
		{
			return "[Player]";
		}
		else if (BasicAI* pPlayer = dynamic_cast<BasicAI*>(pOwner))
		{
			return "[AI]";
		}
	}
	return "Unknown Chess User";
}

std::string Utils::FormatString(const char* format, ...)
{
	va_list args;
	va_start(args, format);
#ifndef _MSC_VER

	//GCC generates warning for valid use of snprintf to get
	//size of result string. We suppress warning with below macro.
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

	size_t size = std::snprintf(nullptr, 0, format, args) + 1; // Extra space for '\0'

#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif

	std::unique_ptr<char[]> buf(new char[size]);
	std::vsnprintf(buf.get(), size, format, args);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
#else
	int size = _vscprintf(format, args);
	std::string result(++size, 0);
	vsnprintf_s((char*)result.data(), size, _TRUNCATE, format, args);
	return result;
#endif
	va_end(args);
}
