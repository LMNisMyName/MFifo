#ifndef MFIFOSTATS_H
#define MFIFOSTATS_H
#include <QVector>
#include <QString>

#define MFIFOSTATS_MAXDEPTH 128

typedef struct{
    QString name;
    int     id;
    int     depth;
    int     unitSize;
    int     length;
    int     maxLength;
    int     overflow;
    int     wCount;//the count of the words written into the FIFO
    int     rCount;
    int     takeCount;
    int     removeCount;
}MFifoInfo;



class MFifoStats
{
public:
    MFifoStats();
    static QVector<MFifoInfo> stats;
    static std::atomic_flag conLockFlag;
};

#endif // MFIFOSTATS_H
