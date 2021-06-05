#pragma once
#include <string>

class ChessUser;
namespace Utils
{
	const char* GetChessUser(ChessUser* pUser);	

	//std::string format(const std::string fmt_str, ...);

	std::string FormatString(const char* format, ...);
};

