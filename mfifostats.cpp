#include "mfifostats.h"

std::atomic_flag MFifoStats::conLockFlag=ATOMIC_FLAG_INIT;

MFifoStats::MFifoStats()
{

}

QVector<MFifoInfo> MFifoStats::stats;
