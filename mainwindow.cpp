#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), channel_flag(0), wave_file(0), wave_offset(0),
    x_offset(0), x_times(1),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(paintwave()), this, SLOT(on_paintwave()));

    for(int i = 0; i < ADC_CHANNEL_NUM; i++) {
        file_index[i] = 0;
        adc_channel[i].resize(3 * WAVE_X_LENGTH);
        for (int j = 0; j < 3 * WAVE_X_LENGTH; j++)
            adc_channel[i][j] = 3 * cos(j);
//        adc_channel[i].fill(i / 2.0, 3 * WAVE_X_LENGTH);
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
    ui->wave_wdt->yAxis->setRange(-5, 5);

    ui->x_dial->setRange(-WAVE_X_LENGTH, WAVE_X_LENGTH);
    ui->x_dial->setNotchesVisible(true);
    ui->rate_dial->setRange(0, 5);
    ui->rate_dial->setNotchesVisible(true);
    ui->adc0_ck->setStyleSheet(QString("QCheckBox { color: red }"));
    ui->adc1_ck->setStyleSheet(QString("QCheckBox { color: green }"));
    ui->adc2_ck->setStyleSheet(QString("QCheckBox { color: blue }"));
    ui->adc3_ck->setStyleSheet(QString("QCheckBox { color: cyan }"));
    ui->adc4_ck->setStyleSheet(QString("QCheckBox { color: magenta }"));
    ui->adc5_ck->setStyleSheet(QString("QCheckBox { color: yellow }"));
    ui->adc0_y_offset->setRange(-5, 5);
    ui->adc1_y_offset->setRange(-5, 5);
    ui->adc2_y_offset->setRange(-5, 5);
    ui->adc3_y_offset->setRange(-5, 5);
    ui->adc4_y_offset->setRange(-5, 5);
    ui->adc5_y_offset->setRange(-5, 5);
    ui->adc0_y_offset->setTickPosition(QSlider::TicksRight);
    ui->adc1_y_offset->setTickPosition(QSlider::TicksRight);
    ui->adc2_y_offset->setTickPosition(QSlider::TicksRight);
    ui->adc3_y_offset->setTickPosition(QSlider::TicksRight);
    ui->adc4_y_offset->setTickPosition(QSlider::TicksRight);
    ui->adc5_y_offset->setTickPosition(QSlider::TicksRight);

    connect(&udp, SIGNAL(adcdata(unsigned short,unsigned short,uint,QByteArray)),
            this, SLOT(on_adc_data(unsigned short,unsigned short,uint,QByteArray)));
      //已设置默认值192.168.1.66：2:618
//    udp.set_host_ip(QHostAddress(192.168.1.166));
//    udp.set_port(62618);
    udp.start();
//    channel_flag = 0xFF;
    paintwave();
}

MainWindow::~MainWindow()
{
    udp.stop();
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
        channel.push_front(val[i] * 2.5 / 32768.0);
        channel.pop_back();
    }
    if (wave_offset - num < 0) {
       wave_file -= 1;
       wave_offset = FILE_SIZE + wave_offset - num;
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
    QVector <double> temp[ADC_CHANNEL_NUM];
    for(int i = 0; i < ADC_CHANNEL_NUM; i++) {
        if (channel_flag & (0x01 << i)) {   //设置需要显示的曲线
            ui->wave_wdt->graph(i)->setPen(QPen((enum Qt::GlobalColor)(Qt::red + i)));
            ui->wave_wdt->graph(i)->setName(QString("Channel%1").arg(i));
            /* 波形加上偏移值 */
            temp[i] = adc_channel[i].mid(WAVE_X_OFFSET + x_offset, x.size());
            for(int j = 0; j < temp[i].size(); j++) {
                temp[i].replace(j, temp[i][j] + y_offset[i]);
            }
            ui->wave_wdt->graph(i)->setData(x, temp[i]);
            ui->wave_wdt->graph(i)->setVisible(true);
        }else { //隐藏不显示的曲线
            ui->wave_wdt->graph(i)->setVisible(false);
        }
    }

    //设置坐标轴显示范围,否则我们只能看到默认的范围
    ui->wave_wdt->xAxis->setRange(0, x.size());
    ui->wave_wdt->replot();
}

void MainWindow::file_write(unsigned char broadid, unsigned char channel, short val[], int num)
{
    if (channel >= ADC_CHANNEL_NUM && num <= 0)
        return;
    if (!file[channel].isOpen()) {
        file[channel].setFileName("broad_%1_%2_%3").arg(broadid).arg(channel).arg(file_index[channel]);
        if (!file[channel].open(QIODevice::WriteOnly)) {
            QMessageBox msg(QMessageBox::Warning, "Error", "Create file failed");
            msg.exec();
            return;
        }
    }

    if (file[channel].size() + (2 * num) >= FILE_SIZE) {
        int count = FILE_SIZE - file[channel].size();
        file[channel].write((const char*)val, count);
        file[channel].close();
        file_index[channel]++;
        file[channel].setFileName(QString("channel_%1_%2").arg(channel).arg(file_index[channel]));
        if (!file[channel].open(QIODevice::WriteOnly)) {
            QMessageBox msg(QMessageBox::Warning, "Error", "Create file failed");
            msg.exec();
            return;
        }
        file[channel].write((const char*)val, (2 * num) - count);
    } else {
        file[channel].write((const char*)val, 2 * num);
    }
}

void MainWindow::file_read(unsigned char broadid, unsigned char channel, short val[], int num)
{
    if (channel >= ADC_CHANNEL_NUM && num <= 0 && wave_file < 0)
        return;

    QFile temp(QString("broad_%1_%2_%3").arg(broadid).arg(channel).arg(wave_file));
    if (!temp.open(QIODevice::ReadOnly)) {
        QMessageBox msg(QMessageBox::Warning, "Error", "Open file failed");
        msg.exec();
        return;
    }

    if (wave_offset - (2 * num) < 0) {
        int count = (2 * num) - wave_offset;
        temp.read((char*)val, wave_offset);
        wave_file--;
        if (wave_file < 0)
            return;
        temp.setFileName(QString("channel_%1_%2").arg(channel).arg(wave_file));
        if (!temp.open(QIODevice::ReadOnly)) {
            QMessageBox msg(QMessageBox::Warning, "Error", "Open file failed");
            msg.exec();
            return;
        }
        wave_offset = FILE_SIZE - count;
        temp.seek(wave_offset);
        temp.read((char*)val, count);
    } else {
        wave_offset -= 2 * num;
        temp.seek(wave_offset);
        temp.read((char*)val, 2 * num);
    }
}

void MainWindow::on_x_dial_valueChanged(int value)
{
    x_offset = value;
    paintwave();
}

void MainWindow::on_adc0_y_offset_valueChanged(int value)
{
    y_offset[0] = value;
    paintwave();
}

void MainWindow::on_adc1_y_offset_valueChanged(int value)
{
    y_offset[1] = value;
    paintwave();
}

void MainWindow::on_adc5_y_offset_valueChanged(int value)
{
    y_offset[5] = value;
    paintwave();
}

void MainWindow::on_adc4_y_offset_valueChanged(int value)
{
    y_offset[4] = value;
    paintwave();
}

void MainWindow::on_adc3_y_offset_valueChanged(int value)
{
    y_offset[3] = value;
    paintwave();
}

void MainWindow::on_adc2_y_offset_valueChanged(int value)
{
    y_offset[2] = value;
    paintwave();
}

void MainWindow::on_adc0_ck_clicked(bool checked)
{
    if (checked)
        channel_flag |= 0x01 << 0;
    else
        channel_flag &= ~(0x01 << 0);
    paintwave();
}

void MainWindow::on_adc1_ck_clicked(bool checked)
{
    if (checked)
        channel_flag |= 0x01 << 1;
    else
        channel_flag &= ~(0x01 << 1);
    paintwave();
}

void MainWindow::on_adc2_ck_clicked(bool checked)
{
    if (checked)
        channel_flag |= 0x01 << 2;
    else
        channel_flag &= ~(0x01 << 2);
    paintwave();
}

void MainWindow::on_adc3_ck_clicked(bool checked)
{
    if (checked)
        channel_flag |= 0x01 << 3;
    else
        channel_flag &= ~(0x01 << 3);
    paintwave();
}

void MainWindow::on_adc4_ck_clicked(bool checked)
{
    if (checked)
        channel_flag |= 0x01 << 4;
    else
        channel_flag &= ~(0x01 << 4);
    paintwave();
}

void MainWindow::on_adc5_ck_clicked(bool checked)
{
    if (checked)
        channel_flag |= 0x01 << 5;
    else
        channel_flag &= ~(0x01 << 5);
    paintwave();
}
/* 放大和缩小 */
void MainWindow::on_rate_dial_valueChanged(int value)
{

}

void MainWindow::on_adc_data(unsigned short broadid, unsigned short channel,
                 unsigned int index, QByteArray data)
{
    file_write(broadid, channel, (short *)data.data(), data.size() / 2);
    wave_push_back(adc_channel[channel], (short *)data.data(), data.size() / 2);
    paintwave();
}
