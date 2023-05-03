#include "mfifostatsui.h"
#include "ui_mfifostatsui.h"

MFifoStatsUi::MFifoStatsUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MFifoStatsUi)
{
    ui->setupUi(this);

    iConZoomFit_gray. addFile(QStringLiteral(":/icon_zoomFit_gray_16px.ico"), QSize(), QIcon::Normal, QIcon::Off);
    iConZoomFit_green.addFile(QStringLiteral(":/icon_zoomFit_green_16px.ico"), QSize(), QIcon::Normal, QIcon::Off);
    iConZoomIn       .addFile(QStringLiteral(":/icon_zoomIn_16px.ico" ),              QSize(), QIcon::Normal, QIcon::Off);
    iConZoomOut      .addFile(QStringLiteral(":/icon_zoomOut_16px.ico" ),             QSize(), QIcon::Normal, QIcon::Off);

    ui->pushButton_zoomFit->setText("zoom fit");
    ui->pushButton_zoomIn->setText("zoom in");
    ui->pushButton_zoomOut->setText("zoom out");
//    ui->pushButton_zoomFit->setIcon(iConZoomFit_gray);
//    ui->pushButton_zoomIn ->setIcon(iConZoomIn);
//    ui->pushButton_zoomOut->setIcon(iConZoomOut);
    ui->pushButton_zoomFit->setToolTip ("Resize width to contents");
    ui->pushButton_zoomIn ->setToolTip ("Increase table column width");
    ui->pushButton_zoomOut->setToolTip ("Decrease table column width");


    buildMFifoTable(MFifoStats::stats);
    connect(&updateTimer,SIGNAL(timeout()),this,SLOT(slot_updateTimeout()));
    updateTimer.start(500);
}

MFifoStatsUi::~MFifoStatsUi()
{
    delete ui;
}

//void MFifoStatsUi::updateMFifoTable(const QVector<MFifoInfo>& stats){
//    int i;
//    int n;
//    for(i=0;i<stats.length();i++){
//        n=0;
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].id));
//        ui->tableWidget->item(i,n++)->setText(stats[i].name);
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].depth));
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].unitSize));
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].wCount));
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].rCount));
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].takeCount));
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].removeCount));
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].length));
//        ui->tableWidget->item(i,n++)->setText(QString::number(stats[i].overflow));
//        ui->tableWidget->item(i,n++)->setText(QString::number(fifoUsage(stats[i].depth,stats[i].length))+"%");
//        ui->tableWidget->item(i,n++)->setText(QString::number(fifoUsage(stats[i].depth,stats[i].maxLength))+"%");
//    }
//}

void MFifoStatsUi::slot_updateTimeout(){
    while (MFifoStats::conLockFlag.test_and_set()) {}
    int i,col,n;
    for(i=0;i<MFifoStats::stats.length();i++){
        //insert row in table
        if(i<MFIFOSTATS_MAXDEPTH){
            if(ui->tableWidget->rowCount()<MFifoStats::stats.length()){
                ui->tableWidget->setRowCount(i+1);
                for(col=0;col<12;col++){
                    ui->tableWidget->setItem(i,col,new QTableWidgetItem());
                }
            }
        }
        else{
            qDebug()<<"MFifoStatsUi::slot_updateTimeout()=Table overflow!";
            return;
        }
        //update row
        n=0;
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].id));
        ui->tableWidget->item(i,n++)->setText(MFifoStats::stats[i].name);
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].depth));
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].unitSize));
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].wCount));
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].rCount));
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].takeCount));
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].removeCount));
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].length));
        ui->tableWidget->item(i,n++)->setText(QString::number(MFifoStats::stats[i].overflow));
        ui->tableWidget->item(i,n++)->setText(QString::number(fifoUsage(MFifoStats::stats[i].depth,MFifoStats::stats[i].length))+"%");
        ui->tableWidget->item(i,n++)->setText(QString::number(fifoUsage(MFifoStats::stats[i].depth,MFifoStats::stats[i].maxLength))+"%");
    }
    MFifoStats::conLockFlag.clear();
}

void MFifoStatsUi::buildMFifoTable(const QVector<MFifoInfo>& stats){
    int i,n;
    int tableRowNum,tableColNum;
    QStringList headers;
    headers << "ID" << "Name" << "Depth" <<"Unit size"<<"W-Count"<<"R-Count"<<"Token" << "Removed" <<"Length"<< "Overflow" <<"Usage"<<"Max Usage";


    tableRowNum=stats.length();
    tableColNum=12;

    ui->tableWidget->setColumnCount(tableColNum);
    ui->tableWidget->setRowCount(tableRowNum);

    for(i=0;i<tableColNum;i++){
        for(n=0;n<tableRowNum;n++){
            ui->tableWidget->setItem(n,i,new QTableWidgetItem());
        }
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);

}

double MFifoStatsUi::fifoUsage(int depth, int length) const
{
    int usageInt;
    double usage;

    usage=length;
    usage/=depth;
    usageInt=usage*1000000;
    usage=usageInt;
    usage/=10000;
    return usage;
}



void MFifoStatsUi::on_pushButton_zoomFit_clicked()
{
    int col;
    if(ui->tableWidget->horizontalHeader()->sectionResizeMode(0)==QHeaderView::ResizeToContents){

        ui->pushButton_zoomFit->setIcon (iConZoomFit_gray);
        ui->pushButton_zoomIn->setEnabled(true);
        ui->pushButton_zoomOut->setEnabled(true);

        for(col=0;col<ui->tableWidget->columnCount();col++){
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Interactive);
        }
    }
    else{


        ui->pushButton_zoomFit->setIcon (iConZoomFit_green);
        ui->pushButton_zoomIn->setEnabled(false);
        ui->pushButton_zoomOut->setEnabled(false);

        for(col=0;col<ui->tableWidget->columnCount();col++){
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
        }
    }
}

void MFifoStatsUi::on_pushButton_zoomOut_clicked()
{
    int i=0;
    int width=0;
    for(i=0;i<ui->tableWidget->columnCount();i++){
        width=ui->tableWidget->columnWidth(i);
        width-=20;
        ui->tableWidget->setColumnWidth(i,(width>16)?width:16);
    }
}

void MFifoStatsUi::on_pushButton_zoomIn_clicked()
{
    int i=0;
    int width=0;
    for(i=0;i<ui->tableWidget->columnCount();i++){
        width=ui->tableWidget->columnWidth(i);
         if(width+20<512)ui->tableWidget->setColumnWidth(i,width+20);
    }
}

//void MFifoStatsUi::setTableItemColor(int index, int color){
//    int tableRaw,tableCol;
//    tableRaw=index/8;
//    tableCol=7-(index%8);

//    if(1==color){
//        ui->tableWidget->item(tableRaw,tableCol)->setBackground(QBrush(Qt::white));
//    }
//    else if(2==color){
//        ui->tableWidget->item(tableRaw,tableCol)->setBackground(QBrush(*color_mistyrose));//lavenderblush
//    }
//    else if(3==color){
//        ui->tableWidget->item(tableRaw,tableCol)->setBackground(QBrush(*color_lavender));//lavender
//    }

//    //QColor(252,250,204)  lemonchiffon
//    //QColor(252,242,244)  lavenderblush
//    //QColor(228,230,252)  lavender
//    //QColor(252,230,228)  mistyrose




//}




