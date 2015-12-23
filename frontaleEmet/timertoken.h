#ifndef TIMERTOKEN_H
#define TIMERTOKEN_H

#include <QTimer>

class TimerToken : public QTimer
{
	private:
		std::string token;
		bool firstTime=true;
	public:
		TimerToken(std::string token);
		std::string getToken() const;
		void setToken(const std::string &value);
};

#endif // TIMERTOKEN_H
