#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string.h>
#include <QFileDialog>
#include <QTextStream>


unsigned char flag = 0;
int length = 0;
int length_ch1=0;
int length_ch2=0;
float dada_volt=0;
unsigned char save_flag = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->bt_cbx->setCurrentIndex(0);  //设置波特率下拉菜单默认显示第二项(115200)
    ui->start_btn->setEnabled(false);
    ui->ch1_num_lb->setText(QString::number(length_ch1));
    ui->ch2_num_lb->setText(QString::number(length_ch2));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_clear_1_clicked()
{
    length_ch1 = 0;
    ui->ch_text1->clear();   //清除文本框中1号通道数据
    ui->ch1_num_lb->setText(QString::number(length_ch1));
}

void MainWindow::on_clear_2_clicked()
{
     length_ch2 = 0;
    ui->ch_text2->clear();
    ui->ch2_num_lb->setText(QString::number(length_ch2));
}


void MainWindow::Read_Data()   //串口数据接收程序
{
   int num,i=0;
   QByteArray buf;
   unsigned short int data1=0,data2=0;
   short int data=0;
   std::cout<<myCom->bytesAvailable()<<std::endl;  //查看缓冲区有多少字节数据
   num = myCom->bytesAvailable();
   //myCom->readData(&buf,1);
   buf = myCom->readAll();
    if(!buf.isEmpty() && num >= 7)
    {

        while(i<num)
        {
            if((buf[i]==0xff && buf[i+1]==0xee && i+6<=num-1 && buf[i+6] == 0x11))
            {
                data1 = (buf[i+2]&0xff)<<8|(buf[i+3]&0xff);  //buf的元素是char类型
                data2 = (buf[i+4]&0xff)<<8|(buf[i+5]&0xff);
                data = data1-data2;
                dada_volt = data*1.0/1023*5.4;
                ui->ch_text1->append(QString::number(dada_volt));
                ui->ch_text1->moveCursor(QTextCursor::End);  //将光标移到本行的最后位置
                length_ch1++;
                ui->ch1_num_lb->setText(QString::number(length_ch1));
                i=i+6;
            }
            else if((buf[i]==0xdd && buf[i+1]==0xcc && i+6<=num-1 && buf[i+6] == 0x22))
            {
                data1 = (buf[i+2]&0xff)<<8|(buf[i+3]&0xff);  //buf的元素是char类型
                data2 = (buf[i+4]&0xff)<<8|(buf[i+5]&0xff);
                data = data1-data2;
                dada_volt = data*1.0/1023*5.4;
                ui->ch_text2->append(QString::number(dada_volt));
                ui->ch_text2->moveCursor(QTextCursor::End);  //将光标移到本行的最后位置
                length_ch2++;
                ui->ch2_num_lb->setText(QString::number(length_ch2));
                i=i+6;
            }
            i++;
        }
#if 0
        if((buf[0]==0xff) && (buf[6]==0x11))
        {
            data1 = (buf[2]&0xff)<<8|(buf[3]&0xff);  //buf的元素是char类型
            data2 = (buf[4]&0xff)<<8|(buf[5]&0xff);
            data = data1-data2;
            dada_volt = data*1.0/1024*5;
            ui->ch_text1->append(QString::number(dada_volt));
           // ui->ch_text1->append(buf.toHex());
            ui->ch_text1->moveCursor(QTextCursor::End);  //将光标移到本行的最后位置
            length_ch1++;
            ui->ch1_num_lb->setText(QString::number(length_ch1));
        }
        if((buf[0]==0xdd) && (buf[6]==0x22))
        {
            data1 = (buf[2]&0xff)<<8|(buf[3]&0xff);
            data2 = (buf[4]&0xff)<<8|(buf[5]&0xff);
            data = data1-data2;
             dada_volt = data*1.0/1024*5;
            ui->ch_text2->append(QString::number(dada_volt));
            ui->ch_text2->moveCursor(QTextCursor::End);  //将光标移到本行的最后位置
            length_ch2++;
            ui->ch2_num_lb->setText(QString::number(length_ch2));
        }
#endif
    }
}

void MainWindow::on_uart_btn_clicked()   //单击打开串口按键
{
    enum BaudRateType BAUD;
    if(ui->uart_btn->text() == tr("open uart"))
    {
        myCom = new Win_QextSerialPort(ui->uart_cbx->currentText(),QextSerialBase::EventDriven);
        myCom->open(QIODevice::ReadWrite);   //打开模式
        if(ui->bt_cbx->currentIndex() == 0)
            BAUD = BAUD9600;
        else if(ui->bt_cbx->currentIndex() == 1)
            BAUD = BAUD115200;

        myCom->setBaudRate(BAUD);
        myCom->setDataBits(DATA_8);    //数据位设置为8位数据
        myCom->setParity(PAR_NONE);    //无校验
        myCom->setStopBits(STOP_1);    //停止位设置,我们设置为1位停止位
        myCom->setFlowControl(FLOW_OFF);  //关闭控制流
        if(!myCom->isOpen())
        {
            QMessageBox::critical(this,tr("error"),tr("uart fail"),QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
            std::cout<<"uart failed"<<std::endl;
            return;
        }
        //关闭设置菜单使能
        ui->bt_cbx->setEnabled(false);
        ui->uart_cbx->setEnabled(false);
        ui->uart_btn->setText(tr("close uart"));
        //EventDriven模式下才能触发readyRead()信号
        connect(myCom,SIGNAL(readyRead()),this,SLOT(Read_Data()));  //连接信号槽
        ui->start_btn->setEnabled(true);
        //ui->uart_btn->setEnabled(false);
        ui->stop_btn->setEnabled(false);
    }
    else
    {
        //关闭串口
        myCom->close();
        myCom->deleteLater();
        //恢复设置使能
        ui->bt_cbx->setEnabled(true);
        ui->uart_cbx->setEnabled(true);
        ui->uart_btn->setText(tr("open uart"));
    }
}

//使用一个QFile类对象来指向要保存的文件，然后使用文本写入方式将其打开。打开后再使用QTextStream文本流将编辑器中的内容写入到文件中
bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);   //QFile类是操作文件的输入输出设备类
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        //%1和%2分别对应后面arg两个参数, /n起换行的作用
        QMessageBox::warning(this,tr("多文档编辑器"),tr("无法写入文件 %1: /n %2")
                              .arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    if(save_flag == 1)
        out<<ui->ch_text1->toPlainText();

    if(save_flag == 2)
        out<<ui->ch_text2->toPlainText();
    QMessageBox::warning(this,tr("success"),tr("save success"),QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
    save_flag = 0;
    return true;
}

void MainWindow::on_save_1_clicked()
{
    if(ui->ch_text1->document()->isEmpty())
    {
        QMessageBox::critical(this,tr("error"),tr("txt is empty, can not save"),QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    //使用QFileDialog来实现一个另存为对话框,并且获得了文件的路径
     QString fileName = QFileDialog::getSaveFileName(this,tr("save as"),tr("no_name.txt"));
     if(fileName.isEmpty())
     {
         return;
     }
     save_flag = 1;
     saveFile(fileName);
}

void MainWindow::on_save_2_clicked()
{
    if(ui->ch_text2->document()->isEmpty())
    {
        QMessageBox::critical(this,tr("error"),tr("txt is empty, can not save"),QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    //使用QFileDialog来实现一个另存为对话框,并且获得了文件的路径
     QString fileName = QFileDialog::getSaveFileName(this,tr("save as"),tr("no_name.txt"));
     if(fileName.isEmpty())
     {
         return;
     }
     save_flag = 2;
     saveFile(fileName);
}

void MainWindow::on_start_btn_clicked()   //发送开始传输命令
{
    QByteArray ba;
    ba[0] = 'a';
    ui->start_btn->setEnabled(false);
    ui->stop_btn->setEnabled(true);
    myCom->write(ba);  //.toLatin1(将QString转为QByteArray)
}

void MainWindow::on_stop_btn_clicked()   //发送停止传输命令
{
    QByteArray ba;
    ba[0] = 's';
    ui->start_btn->setEnabled(true);
    ui->stop_btn->setEnabled(false);
    myCom->write(ba);  //.toLatin1(将QString转为QByteArray)
}
