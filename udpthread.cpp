#include "udpthread.h"

udpthread::udpthread(): flag(true)
{
    address = QHostAddress("192.168.1.166");
    port = 62618;
    udp = new QUdpSocket;
    udp->bind(QHostAddress::Any, port);
    connect(udp, SIGNAL(readyRead()),
       this, SLOT(recv()),Qt::DirectConnection);
}

udpthread::~udpthread()
{
    flag = false;
    if (udp!=NULL)
        delete udp;
}

qint64 udpthread::send(const char *data, qint64 size,
         const QHostAddress &address, quint16 port)
{
    return udp->writeDatagram(data, size, address, port);
}

void udpthread::run()
{
    QByteArray temp;
    temp.resize(4);
    temp.fill(0x00, 4);
    int i = 0;
    unsigned short broad = 0;
    unsigned short channel = 0;
    unsigned short parameter = 0;
    unsigned int index = 0;
    unsigned short cmd = 0;
    while(flag) {
        /* 设备信息 */
        temp[1] = 0x10;
        temp[3] = 0x11;
        i = recv_data.indexOf(temp);
        if (i > 0 && checkdata(recv_data.mid(i - 2, 136))) {
            broad = (recv_data[i - 2] << 8) | recv_data[i - 1];
            emit broadinfo(broad, recv_data.mid(i + 4, 128));
            recv_data.remove(0, i + 134);
        }
        /* 配置信息 */
        temp[1] = 0x11;
        temp[3] = 0x00;
        i = recv_data.indexOf(temp);
        if (i > 0 && checkdata(recv_data.mid(i - 2, 8))) {
            broad = (recv_data[i - 2] << 8) | recv_data[i - 1];
            emit configinfo(broad);
            recv_data.remove(0, i + 6);
        }
        /* adc通道实时信息 */
        temp[1] = 0x14;
        temp[3] = 0x00;
        i = recv_data.indexOf(temp.mid(0, 2));
        if (i > 0 && checkdata(recv_data.mid(i - 2, 525))) {
            broad = (recv_data[i - 2] << 8) | recv_data[i - 1];
            channel = (recv_data[i + 2] << 8) | recv_data[i + 3];
            index = (recv_data[i + 4] << 24) | (recv_data[i + 5] << 16) |
                    (recv_data[i + 6] << 8) | recv_data[i + 7];
            emit adcdata(broad, channel, index, recv_data.mid(i + 8, 512));
            recv_data.remove(0, i + 523);
        }
        /* sd卡信息 */
        temp[1] = 0x13;
        temp[3] = 0x00;
        i = recv_data.indexOf(temp.mid(0, 2));
        if (i > 0 && checkdata(recv_data.mid(i - 2, 525))) {
            broad = (recv_data[i - 2] << 8) | recv_data[i - 1];
            parameter = (recv_data[i + 2] << 8) | recv_data[i + 3];
            index = (recv_data[i + 4] << 24) | (recv_data[i + 5] << 16) |
                    (recv_data[i + 6] << 8) | recv_data[i + 7];
            emit sddata(broad, parameter, index, recv_data.mid(i + 8, 512));
            recv_data.remove(0, i + 523);
        }
        /* 固件下载起始应答 */
        temp[1] = 0x20;
        temp[3] = 0x00;
        i = recv_data.indexOf(temp);
        if (i > 0 && checkdata(recv_data.mid(i - 2, 18))) {
            broad = (recv_data[i - 2] << 8) | recv_data[i - 1];
            emit fireware_write_start_ack(broad);
            recv_data.remove(0, i + 16);
        }
        /* 固件下载请求下一个包 */
        temp[1] = 0x21;
        temp[3] = 0x00;
        i = recv_data.indexOf(temp);
        if (i > 0 && checkdata(recv_data.mid(i - 2, 15))) {
            broad = (recv_data[i - 2] << 8) | recv_data[i - 1];
            index = (recv_data[i + 4] << 40) | (recv_data[i + 5] << 32) |
                    (recv_data[i + 6] << 24) | (recv_data[i + 7] << 16) |
                    (recv_data[i + 8] << 8) | (recv_data[i + 9]);
            emit fireware_next_package_request(broad, index);
            recv_data.remove(0, i + 13);
        }
        /* 固件下载重新请求包 */
        temp[1] = 0x22;
        temp[3] = 0x00;
        i = recv_data.indexOf(temp);
        if (i > 0 && checkdata(recv_data.mid(i - 2, 15))) {
            broad = (recv_data[i - 2] << 8) | recv_data[i - 1];
            index = (recv_data[i + 4] << 40) | (recv_data[i + 5] << 32) |
                    (recv_data[i + 6] << 24) | (recv_data[i + 7] << 16) |
                    (recv_data[i + 8] << 8) | (recv_data[i + 9]);
            emit fireware_repeat_package_request(broad, index);
            recv_data.remove(0, i + 13);
        }
        /* 错误信息 */
        temp[1] = 0x50;
        temp[3] = 0x00;
        i = recv_data.indexOf(temp.mid(0, 2));
        if (i > 0 && checkdata(recv_data.mid(i - 2, 8))) {
            broad = (recv_data[i - 2] << 8) | recv_data[i - 1];
            parameter = (recv_data[i + 2] << 8) | recv_data[i + 3];
            emit error(broad, cmd);
            recv_data.remove(0, i + 6);
        }
    }
    quit();
}

void udpthread::recv()
{
    while (udp->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udp->pendingDatagramSize());
        udp->readDatagram(datagram.data(), datagram.size());
        recv_data += datagram;
    }
}

void udpthread::stop()
{
    flag = false;
}

bool udpthread::checkdata(QByteArray data)
{
    return true;
}

