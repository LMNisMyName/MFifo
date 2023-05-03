#ifndef MFIFOSTATSUI_H
#define MFIFOSTATSUI_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QIcon>
#include "mfifo.hpp"

namespace Ui {
class MFifoStatsUi;
}

class MFifoStatsUi : public QWidget
{
    Q_OBJECT

public:
    explicit MFifoStatsUi(QWidget *parent = nullptr);
    ~MFifoStatsUi();

//    void updateMFifoTable(const QVector<MFifoInfo>& stats);

private:
    Ui::MFifoStatsUi *ui;
    QTimer updateTimer;
    QIcon iConZoomFit_gray;
    QIcon iConZoomFit_green;
    QIcon iConZoomIn;
    QIcon iConZoomOut;

    void buildMFifoTable(const QVector<MFifoInfo>& stats);

    double fifoUsage(int depth, int length)const;


private slots:
    void slot_updateTimeout();
    void on_pushButton_zoomFit_clicked();
    void on_pushButton_zoomOut_clicked();
    void on_pushButton_zoomIn_clicked();
};

#endif // MFIFOSTATSUI_H
