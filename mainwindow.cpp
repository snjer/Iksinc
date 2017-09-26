#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), channel_flag(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(paintwave()), this, SLOT(on_paintwave()));

    for(int i = 0; i < ADC_CHANNEL_NUM; i++) {
        adc_channel[i].resize(WAVE_X_LENGTH);
        adc_channel[i].fill(i / 2.0, WAVE_X_LENGTH);
        //向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
        ui->wave_wdt->addGraph();
    }

    for(int i=0; i < WAVE_X_LENGTH; i++)
    {
        x.append(i);
    }

    //设置坐标轴标签名称
    ui->wave_wdt->xAxis->setLabel("T");
    ui->wave_wdt->yAxis->setLabel("V");
    ui->wave_wdt->yAxis->setRange(-3, 3);

    channel_flag = 0xFF;
    paintwave();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::wave_push_back(QVector<double> &channel, short val)
{
    channel.push_back(val * 2.5 /32768);
    channel.pop_front();
}

void MainWindow::wave_push_front(QVector<double> &channel, short val)
{
    channel.push_front(val * 2.5 /32768);
    channel.pop_back();
}

void MainWindow::paintEvent(QPaintEvent *event)
{

}

void MainWindow::on_paintwave(void)
{
    for(int i = 0; i < ADC_CHANNEL_NUM; i++) {
        if (channel_flag & (0x01 << i)) {
            ui->wave_wdt->graph(i)->setPen(QPen((enum Qt::GlobalColor)(Qt::red + i)));
            ui->wave_wdt->graph(i)->setName(QString("Channel%1").arg(i));
            ui->wave_wdt->graph(i)->setData(x, adc_channel[i]);
        }
    }

    //设置坐标轴显示范围,否则我们只能看到默认的范围
    ui->wave_wdt->xAxis->setRange(0, x.size());
    ui->wave_wdt->replot();
}
