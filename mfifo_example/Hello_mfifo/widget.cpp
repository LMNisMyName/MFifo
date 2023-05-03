#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent, MFifo<int>* fifo)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , fifo(fifo)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_push_button_clicked()
{
    fifo->write(ui->push_target->value());
}


void Widget::on_pop_button_clicked()
{
    int val;
    if (fifo->read(val))
        ui->pop_value->setNum(val);
    else
        ui->pop_value->setText("FIFO is empty!");
}

