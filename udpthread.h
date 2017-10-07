#ifndef UDPTHREAD_H
#define UDPTHREAD_H
#include <QThread>
#include <QByteArray>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>

class udpthread: public QThread
{
    Q_OBJECT
public:
    udpthread();
    ~udpthread();
    void set_host_ip(const QHostAddress &addr);
    void set_port(quint16 port);
    qint64 send(const char * data, qint64 size,
             const QHostAddress &address, quint16 port);
    void stop();

private slots:
    void recv();

protected:
    void run();

signals:
    void broadinfo(unsigned short broadid, QByteArray info);
    void configinfo(unsigned short broadid);
    void adcdata(unsigned short broadid, unsigned short channel,
                 unsigned int index, QByteArray data);
    void sddata(unsigned short broadid, unsigned short parameter,
                unsigned short pagecnt, QByteArray data);
    void fireware_write_start_ack(unsigned short broadid);
    void fireware_next_package_request(unsigned short broadid, unsigned int index);
    void fireware_repeat_package_request(unsigned short broadid, unsigned int index);
    void error(unsigned short broadid, unsigned short cmd);

private:
    bool flag;
    QUdpSocket *udp;
    quint16 port;
    QHostAddress address;
    QByteArray recv_data;

    bool checkdata(QByteArray data);
};

#endif // UDPTHREAD_H
