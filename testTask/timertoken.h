#ifndef TIMERTOKEN_H
#define TIMERTOKEN_H

#include <QTimer>

class TimerToken : public QTimer
{
		Q_OBJECT
	signals:
		void timeOutMyToken(std::string token);
	private:
		std::string token;
		bool firstTime=true;
	public:
		TimerToken(std::string token);
		std::string getToken() const;
		void setToken(const std::string &value);
	public slots:
		void emitToken();
};

#endif // TIMERTOKEN_H
