#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFile>
#include "udpthread.h"

#define ADC_CHANNEL_NUM 6
#define ADC_CHANNEL(val)   ((0x01) << (val))
#define WAVE_X_LENGTH 1000
#define WAVE_X_OFFSET (WAVE_X_LENGTH)
#define FILE_SIZE   (10 * 1024 * 1024)

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_paintwave(void);    //画波形函数
    void on_x_dial_valueChanged(int value);

    void on_adc0_y_offset_valueChanged(int value);

    void on_adc1_y_offset_valueChanged(int value);

    void on_adc5_y_offset_valueChanged(int value);

    void on_adc4_y_offset_valueChanged(int value);

    void on_adc3_y_offset_valueChanged(int value);

    void on_adc2_y_offset_valueChanged(int value);

    void on_adc0_ck_clicked(bool checked);

    void on_adc1_ck_clicked(bool checked);

    void on_adc2_ck_clicked(bool checked);

    void on_adc3_ck_clicked(bool checked);

    void on_adc4_ck_clicked(bool checked);

    void on_adc5_ck_clicked(bool checked);

    void on_rate_dial_valueChanged(int value);

    /* 处理UDP接收到的数据 */
    void on_adc_data(unsigned short broadid, unsigned short channel,
                     unsigned int index, QByteArray data);
    void on_broadinfo(unsigned short broadid, QByteArray info);
    void on_configinfo(unsigned short broadid);
    void on_sddata(unsigned short broadid, unsigned short parameter,
                unsigned short pagecnt, QByteArray data);
    void on_fireware_write_start_ack(unsigned short broadid);
    void on_fireware_next_package_request(unsigned short broadid, unsigned int index);
    void on_fireware_repeat_package_request(unsigned short broadid, unsigned int index);
    void on_error(unsigned short broadid, unsigned short cmd);

signals:
    void paintwave(void);   //画波形图信号


private:
    Ui::MainWindow *ui;
    QFile file[ADC_CHANNEL_NUM];
    QVector<double> adc_channel[ADC_CHANNEL_NUM];   //显示Y轴坐标
    QVector<double> x;          //显示X轴坐标
    udpthread udp;
    int x_offset;               //水平偏移
    int x_times;                //放大倍数
    int y_offset[ADC_CHANNEL_NUM];//垂直偏移
    unsigned char channel_flag; //显示通道MASK
    long wave_file;    //当前显示数据的文件索引
    long wave_offset;  //当前显示数据的文件内偏移
    unsigned long file_index[ADC_CHANNEL_NUM];         //当前存储文件编号

    /* 向波形后面添加数据 */
    void wave_push_back(QVector<double> &channel, short val[], int num);
    /* 向波形前面添加数据 */
    void wave_push_front(QVector<double> &channel, short val[], int num);
    /* 存储数据到文件 */
    void file_write(unsigned char broadid, unsigned char channel, short val[], int num);
    /* 从文件中读取数据 */
    void file_read(unsigned char broadid, unsigned char channel, short val[], int num);
    void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
