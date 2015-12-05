#ifndef RECEPTION_TCP_H
#define RECEPTION_TCP_H
#include <QCoreApplication>
#include <QTcpServer>


class reception_tcp : public QTcpServer
{

    Q_OBJECT

public:
    reception_tcp();
    void bind();

private slots:
    void lecture();
    void affichage();

private:
    QTcpSocket *soc;
};

#endif // RECEPTION_TCP_H
