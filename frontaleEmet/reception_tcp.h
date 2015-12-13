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
    QString getMsg();
    bool getConnexion();
    void attenteLecture();
    void lecture();

private:
    QTcpSocket *soc;
    bool connexion;
    QString msg;
};

#endif // RECEPTION_TCP_H
