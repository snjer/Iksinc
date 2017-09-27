#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFile>

#define ADC_CHANNEL_NUM 6
#define ADC_CHANNEL(val)   ((0x01) << (val))
#define WAVE_X_LENGTH 1000
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

signals:
    void paintwave(void);   //画波形图信号

private:
    Ui::MainWindow *ui;
    QFile file[ADC_CHANNEL_NUM];
    QVector<double> adc_channel[ADC_CHANNEL_NUM];   //显示Y轴坐标
    QVector<double> x;          //显示X轴坐标
    unsigned char channel_flag; //显示通道MASK
    unsigned long wave_file;    //当前显示数据的文件索引
    unsigned long wave_offset;  //当前显示数据的文件内偏移
    unsigned long file_index[ADC_CHANNEL_NUM];         //当前存储文件编号

    /* 向波形后面添加数据 */
    void wave_push_back(QVector<double> &channel, short val[], int num);
    /* 向波形前面添加数据 */
    void wave_push_front(QVector<double> &channel, short val[], int num);
    /* 存储数据到文件 */
    void file_write(unsigned char channel, short val[], int num);
    /* 从文件中读取数据 */
    void file_read(unsigned char channel, short val[], int num);
    void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
