#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "mfifo.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr, MFifo<int>* fifo = nullptr);
    ~Widget();

private slots:
    void on_push_button_clicked();

    void on_pop_button_clicked();

private:
    Ui::Widget *ui;
    MFifo<int>* fifo;
};
#endif // WIDGET_H
