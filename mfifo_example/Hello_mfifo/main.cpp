#include "widget.h"
#include "mfifo.hpp"
#include "mfifostatsui.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MFifo<int> fifo(nullptr, 8);
    fifo.setName("mfifo_0");
    MFifoStatsUi fifo_stat_ui;
    Widget w(nullptr, &fifo);

    w.show();
    fifo_stat_ui.show();
    return a.exec();
}
