#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), channel_flag(0), wave_file(0), wave_offset(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(paintwave()), this, SLOT(on_paintwave()));

    for(int i = 0; i < ADC_CHANNEL_NUM; i++) {
        file_index[i] = 0;
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

void MainWindow::wave_push_back(QVector<double> &channel, short val[], int num)
{
    for (int i = 0; i < num; i++) {
        channel.push_back(val[i] * 2.5 /32768);
        channel.pop_front();
    }
    if (num +wave_offset > FILE_SIZE) {
       wave_file += 1;
       wave_offset =wave_offset + num - FILE_SIZE;
    }else {
       wave_offset += num;
    }
    paintwave();
}

void MainWindow::wave_push_front(QVector<double> &channel, short val[], int num)
{
    for (int i = 0; i < num; i++) {
        channel.push_front(val * 2.5 /32768);
        channel.pop_back();
    }
    if (offset - num < 0) {
       wave_file -= 1;
       wave_offset = FILE_SIZE +wave_offset - num;
    } else {
       wave_offset -= num;
    }
    paintwave();
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

void MainWindow::file_write(unsigned char channel, short val[], int num)
{
    if (channel >= ADC_CHANNEL_NUM && num <= 0)
        return;
    if (!file[channel].isOpen()) {
        file[channel] = QFile(QString("channel_%1_%2").arg(channel).arg(file_index[channel]));
        if (!file[channel].open(QIODevice::ReadOnly)) {
            QMessageBox msg(QMessageBox::Warning, "Error", "Open file failed");
            msg.exec();
            return;
        }
    }

    if (file[channel]->size() + (2 * num) >= FILE_SIZE) {
        int count = FILE_SIZE - file[channel]->size();
        file[channel]->write((const char*)val, count);
        file[channel]->save();
        file[channel]->close();
        file_index[channel]++;
        file[channel] = new QFile(QString("channel_%1_%2").arg(channel).arg(file_index[channel]));
        if (!file[channel]) {
            QMessageBox msg(QMessageBox::Warning, "Error", "Create file failed");
            msg.exec();
            return;
        }
        file[channel]->write((const char*)val, (2 * num) - count);
    } else {
        file[channel]->write((const char*)val, 2 * num);
    }
}

void MainWindow::file_read(unsigned char channel, short val[], int num)
{
    if (channel >= ADC_CHANNEL_NUM && num <= 0)
        return;

    QFile temp = QFile(QString("channel_%1_%2").arg(channel).arg(wave_file));
        if (!file[channel]) {
            QMessageBox msg(QMessageBox::Warning, "Error", "Create file failed");
            msg.exec();
            return;
        }
    }

    if (file[channel]->size() + (2 * num) >= FILE_SIZE) {
        int count = FILE_SIZE - file[channel]->size();
        file[channel]->write((const char*)val, count);
        file[channel]->save();
        file_index[channel]++;
        file[channel] = new QFile(QString("channel_%1_%2").arg(channel).arg(file_index[channel]));
        if (!file[channel]) {
            QMessageBox msg(QMessageBox::Warning, "Error", "Create file failed");
            msg.exec();
            return;
        }
        file[channel]->write((const char*)val, (2 * num) - count);
    } else {
        file[channel]->write((const char*)val, 2 * num);
    }
}
