#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

#define ADC_CHANNEL_NUM 6
#define WAVE_X_LENGTH 512

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
    void on_paintwave(void);

signals:
    void paintwave(void);

private:
    Ui::MainWindow *ui;
    QVector<double> adc_channel[ADC_CHANNEL_NUM];
    QVector<double> x;
    unsigned char channel_flag;

    void wave_push_back(QVector<double> &channel, short val);
    void wave_push_front(QVector<double> &channel, short val);
    void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
