#include "timertoken.h"


std::string TimerToken::getToken() const
{
	return token;
}

void TimerToken::setToken(const std::string &value)
{
	token = value;
}

TimerToken::TimerToken(std::string token): token(token)
{
}
