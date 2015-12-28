#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QThread>

class Task : public QObject
{
		Q_OBJECT
	public:
		explicit Task(QObject *parent = 0);
	private:
		QThread* mThread;
	signals:
		void finished();
	public slots:
		void process();

};

#endif // TASK_H
