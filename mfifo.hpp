/******** ******** ******** ******** ******** ******** ********  ******** ******** ******** ****************
0    Revision history
******** ******** ******** ******** ******** ******** ******** ********/
// YYYY-MM-DD
// 2020-03-06   By PengMiao @ RuDong GuangRong
//			    1st version
//
// 2020-06-19   By PengMiao @ USTC
//              add public function:  bool write(const T* tPtr, int count=1);
//              add private function: int spareSpace();
//                                    int rAddrNext();
//                                    int wAddrNext();
//
// 2020-07-06   By PengMiao @ USTC
//              add MFifo name
//              add signal: sig_MFifoOverflow(const QString& name)
//              add public function: deleteFirst();
//
// 2020-09-09   By PengMiao @ USTC
//              add public function: int  indexOf(const T* tPtr, int len, int from=0);
//                                   void resize(int depth=MFIFO_DEFAULT_DEPTH);
//                                   void removeBefore(int pos);
//                                   QVector<T> *takeBefore(int pos);
//
// 2020-09-11   By PengMiao @ USTC
//              add mFifoStats
//              add public function: int  indexOf(const QString& mask, const T* tPtr, int len, int from=0, int& nextFrom=tempInt)const;
//
// 2020-10-02   By PengMiao @ USTC
//              MFifoStats add record for takeBefore() and removeBefore()
//              MFifoStats add unitSize
//              MFifiStatsUi add Zoom-in Zoom-out Zoom-fit functions
//
// 2020-10-11   By PengMiao @ USTC
//              MFifo add std::shared_ptr<> supported
//              MFifo add Thread protection for read() and
//
// 2020-10-18  By PengMiao @ USTC
//             add public function: bool pop();
//
// 2020-11-20  By PengMiao @ USTC
//             MFifo add Thread protection for Constructor
//             Bug fixed for void slot_updateTimeout() in mfifostatsui.cpp
//
// 2020-12-06  By PengMiao @ USTC
//             add public function: void clear(bool overwriteData=false);  T& last();  void popLast();
//
// 2021-01-14  By PengMiao @ USTC
//             add public function: T & at(int i);

/******** ******** ******** ******** ******** ******** ********  ******** ******** ******** ******** ********
1    Introduction of the library
 ******** ******** ******** ******** ******** ******** ********  ******** ******** ******** ******** ********/
//MFifo is the basic fifo for QT


/******** ******** ******** ******** ******** ******** ********  ******** ******** ******** ******** ********
 ******** ******** ******** ******** ******** ******** ********  ******** ******** ******** ******** ********/



/******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ********
 * MFifoSig for signal/slot solution
 ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef MFIFOSIG_H
#define MFIFOSIG_H
#include <QObject>
#include <QWidget>

class MFifoSig : public QObject
{
    Q_OBJECT
public:
    explicit MFifoSig(QObject *parent = 0):QObject(parent){}

signals:
    void sig_MFifoNotEmpty();
    void sig_MFifoOverflow();
    void sig_MFifoOverflow(const QString& name);

};

#endif




/******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ********
 * MFifo declaration
 ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef MFIFO_H
#define MFIFO_H

#include <memory>
#include <atomic>
#include <QVector>
#include <QString>
#include <QDebug>
#include "mfifostats.h"

#define MFIFO_DEFAULT_DEPTH 1024



template <typename T>
class MFifo : public MFifoSig
{

public:
    enum Mask{
        NotCare='x',
        Equal='1',
        NotEqual='0'
    };

    //mFifoStats 需要显式维护的函数
    MFifo(QObject *parent = 0, int depth=MFIFO_DEFAULT_DEPTH);
    bool read(T& tRef);    //并发安全！@2020-10-11
    bool read(T* tPtr, int len);
    bool pop();            //并发安全！@2020-10-18
    bool write(const T& t);//并发安全！@2020-10-11
    bool write(const T* tPtr, int count=1);//并发安全！@2020-10-11

    void clearOverflow();
    bool removeBefore(int pos);//只能由读取一方进行调用
    void reset();
    void resize(int depth=MFIFO_DEFAULT_DEPTH);
    void setName(const QString name);

    //mFifoStats 不需要显式维护的函数
    int  indexOf(const T* tPtr, int len, int from=0, int& nextFrom=tempInt)const;//return -1 if not found
    int  indexOf(const QString& mask, const T* tPtr, int len, int from=0, int& nextFrom=tempInt)const;//return -1 if not found
    bool isEmpty()const;
    bool isFull()const;
    int  isOverflow()const;    
    int  depth()const;
    int  length() const;
    void clear(bool overwriteData=false);//并发安全！@2020-12-06

    /*!
     * takeBefore
     * \param pos 有效取值范围为0~mFifo.length()。超出此范围将返回nullptr
     * \return Return 新建的子串。
     */
    std::shared_ptr<QVector<T>> takeBefore(int pos);//调用了removeBefore()  //只能由读取一方进行调用  //并发安全,但是强烈不建议并发使用！@2020-10-11
    std::shared_ptr<QVector<T>> takeAll();//调用了takeBefore()

//    void deleteFirst();//is not the same as removeBefore(1): deleteFirst()=read(data)+ delete data. VS  removeBefore(1)=read(data).

    T &	        at(int i) ;//ring mode
    T &	        last();//ring mode
    const T &	operator[](int i) const;//ring mode
    T &	        operator[](int i) ;//ring mode
    const T 	takeAt(int i);//ring mode  this will remove MFifo[i]

    //堆模式
    void popLast();//移除最后写入的一个单位

private:
    //Circular Buffer
    QVector<T> tVec;
    int  fifoDepth;
    int  wAddr;//address of next T to write
    int  rAddr;//address of next T to read
    int  overflow;
    QString fifoName;

    int spareSpace() const;
    int rAddrNext() const;
    int wAddrNext() const;
    int innerLength_WSide() const;//和length()函数完全相同，但是只允许内部调用
    int innerLength_RSide() const;//和length()函数完全相同，但是只允许内部调用
    void resetMFifoInfo()const;

    //mFifoStats related
    int mFifoStatsId;

    //
    std::atomic_flag wLockFlag = ATOMIC_FLAG_INIT;
    std::atomic_flag rLockFlag = ATOMIC_FLAG_INIT;

    static int tempInt;
    static T nullT;
};



/******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ********
 * MFifo implementation
 ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ******** ********/

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
int MFifo<T>::tempInt;

template <typename T> T MFifo<T>::nullT;

template <typename T>
MFifo<T>::MFifo(QObject *parent, int depth) : MFifoSig(parent)
{
    while (MFifoStats::conLockFlag.test_and_set()) {}
    fifoDepth=depth;
    wAddr=0;
    rAddr=0;
    tVec.resize(depth);
    overflow=0;
    fifoName="Unnamed mfifo";

    //mFifoStats
    MFifoInfo info;
    mFifoStatsId=MFifoStats::stats.length();    
    MFifoStats::stats.append(info);
    resetMFifoInfo();
    MFifoStats::conLockFlag.clear();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
////////private functions: spareSpace()  rAddrNext()  wAddrNext()
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
int MFifo<T>::spareSpace() const{
    return (this->fifoDepth-1-this->innerLength_WSide());
}

template <typename T>
int MFifo<T>::wAddrNext() const{
    int Buf_wAddr_Next;
    Buf_wAddr_Next=this->wAddr+1;
    if(this->fifoDepth==Buf_wAddr_Next)Buf_wAddr_Next=0;
    return Buf_wAddr_Next;
}

template<typename T>
int MFifo<T>::innerLength_WSide() const
{
    int Buf_wAddr=0;
    int Buf_rAddr=0;
    Buf_wAddr=this->wAddr;
    Buf_rAddr=this->rAddr;
    if(Buf_wAddr>=Buf_rAddr){
        return Buf_wAddr-Buf_rAddr;
    }
    else{
        return this->fifoDepth-Buf_rAddr+Buf_wAddr;
    }
}

template<typename T>
int MFifo<T>::innerLength_RSide() const
{
    int Buf_wAddr=0;
    int Buf_rAddr=0;
    Buf_wAddr=this->wAddr;
    Buf_rAddr=this->rAddr;
    if(Buf_wAddr>=Buf_rAddr){
        return Buf_wAddr-Buf_rAddr;
    }
    else{
        return this->fifoDepth-Buf_rAddr+Buf_wAddr;
    }
}

template<typename T>
void MFifo<T>::resetMFifoInfo() const
{
    MFifoStats::stats[mFifoStatsId].name=fifoName;
    MFifoStats::stats[mFifoStatsId].id=mFifoStatsId;
    MFifoStats::stats[mFifoStatsId].depth=fifoDepth;
    MFifoStats::stats[mFifoStatsId].unitSize=sizeof(T);
    MFifoStats::stats[mFifoStatsId].length=0;
    MFifoStats::stats[mFifoStatsId].maxLength=0;
    MFifoStats::stats[mFifoStatsId].overflow=0;
    MFifoStats::stats[mFifoStatsId].wCount=0;
    MFifoStats::stats[mFifoStatsId].rCount=0;
    MFifoStats::stats[mFifoStatsId].takeCount=0;
    MFifoStats::stats[mFifoStatsId].removeCount=0;
}


template <typename T>
int MFifo<T>::rAddrNext() const{
    int Buf_rAddr_Next;
    Buf_rAddr_Next=this->rAddr+1;
    if(this->fifoDepth==Buf_rAddr_Next)Buf_rAddr_Next=0;
    return Buf_rAddr_Next;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////write
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
bool MFifo<T>::write(const T& t){
    while (wLockFlag.test_and_set()) {}
    int currentLen;
    //step1:Space checking
    if(1>this->spareSpace()){//Buffer doesn't have enough space for new data (len)
//        qDebug()<<"MFifo" << this->fifoName << "'s spareSpace = " << this->spareSpace() << " is empty";
        (this->overflow)++;
        emit sig_MFifoOverflow();
        emit sig_MFifoOverflow(fifoName);
        MFifoStats::stats[mFifoStatsId].overflow=overflow;
        wLockFlag.clear();
        return false;
    }

    //step2:save NewData into the Buffer
    this->tVec[this->wAddr]=t;

    //step3:update the wAddr
    this->wAddr=this->wAddrNext();

    //step4:update the mFifoStats
    MFifoStats::stats[mFifoStatsId].length=this->innerLength_WSide();
    MFifoStats::stats[mFifoStatsId].wCount++;
    currentLen=this->innerLength_WSide();
    if(currentLen>MFifoStats::stats[mFifoStatsId].maxLength)MFifoStats::stats[mFifoStatsId].maxLength=currentLen;
    emit sig_MFifoNotEmpty();
    wLockFlag.clear();
    return true;
}



template <typename T>
bool MFifo<T>::write(const T* tPtr, int count){
    while (wLockFlag.test_and_set()) {}
    int i;
    int currentLen;
    if((count<=0)||(NULL==tPtr)){
        wLockFlag.clear();
        return false;
    }

    //step1:Space checking
    if(count>this->spareSpace()){//Buffer doesn't have enough space for new data (len)
        (this->overflow)+=count;
        MFifoStats::stats[mFifoStatsId].overflow=overflow;
        emit sig_MFifoOverflow();
        emit sig_MFifoOverflow(fifoName);
        qDebug()<<"MFifo("<<this->fifoName<<")::write("<<count<<") overflow!";
        wLockFlag.clear();
        return false;
    }

    //step2:save NewData into the Buffer
    for(i=0;i<count;i++){
        this->tVec[this->wAddr]=tPtr[i];
        this->wAddr=this->wAddrNext(); //update the wAddr
    }

    //step4:update the mFifoStats
    MFifoStats::stats[mFifoStatsId].length=this->innerLength_WSide();
    MFifoStats::stats[mFifoStatsId].wCount+=count;
    currentLen=this->innerLength_WSide();
    if(currentLen>MFifoStats::stats[mFifoStatsId].maxLength)MFifoStats::stats[mFifoStatsId].maxLength=currentLen;
    emit sig_MFifoNotEmpty();
    wLockFlag.clear();
    return true;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
////////read
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
bool MFifo<T>::read(T &tRef){
    while (rLockFlag.test_and_set()) {}
    int Buf_rAddr=0;

    //step1:check if Queue is empty
    if(this->wAddr==this->rAddr){
//        qDebug()<<"MFifo("<<this->fifoName<<")::read() while empty!";
        rLockFlag.clear();
        return false;
    }

    //step2:get read address in the Buffer
    Buf_rAddr=this->rAddr;

    //step3:copy data
    tRef=this->tVec[Buf_rAddr];
    this->tVec[Buf_rAddr]=nullT;//support std::shared_ptr<>

    //step4:update the Fifo
    this->rAddr=this->rAddrNext();
    //NOTE:update the Fifo_rAddr at last, in order to make sure it is thread-safe.

    //step5:update the mFifoStats
    MFifoStats::stats[mFifoStatsId].length=this->innerLength_RSide();
    MFifoStats::stats[mFifoStatsId].rCount++;
    rLockFlag.clear();
    return true;
}

template<typename T>
bool MFifo<T>::read(T *tPtr, int len)
{
    while (rLockFlag.test_and_set()) {}
    int i=0;
    int Buf_rAddr=0;
    //step1:check if len is valid
    if((len<=0)||(len>this->innerLength_RSide())){
        rLockFlag.clear();
        return false;
    }

    //step2:copy data
    for(i=0;i<len;i++){
        Buf_rAddr=this->rAddr;//get read address in the Buffer
        tPtr[i]=this->tVec[Buf_rAddr];
        this->tVec[Buf_rAddr]=nullT;//support std::shared_ptr<>
        this->rAddr=this->rAddrNext();//update the Fifo rAddr
    }

    //step3:update the mFifoStats
    MFifoStats::stats[mFifoStatsId].length=this->innerLength_RSide();
    MFifoStats::stats[mFifoStatsId].rCount+=len;
    rLockFlag.clear();
    return true;
}

template<typename T>
bool MFifo<T>::pop()
{
    T temp;
    return this->read(temp);
}


//template <typename T>
//bool MFifo<std::shared_ptr<T>>::read(std::shared_ptr<T>& tRef){

//}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////indexOf
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
int  MFifo<T>::indexOf(const T* tPtr, int len, int from, int& nextFrom) const{//return -1 if not found
    int i=0,n,k;
    int fifoLen;
    bool result;
    fifoLen=this->innerLength_RSide();

    if(0==tPtr)return -1;

    for(i=from;i+len<=fifoLen;i++){
        result=true;
        for(n=0;result&&(n<len);n++){
            if(tPtr[n]!=(*this)[i+n])result=false;
        }
        if(result){
            nextFrom=i+1;
            return i;
        }
    }

    //尽量比对片段，在tPtr中找剩下的mFifo数据段
    for(k=i;k<fifoLen;k++){
        result=true;
        for(n=0;result&&(n<len)&&(n<fifoLen-k);n++){
            if(tPtr[n]!=(*this)[k+n])result=false;
        }
        if(result){
            nextFrom=k;
            return -1;
        }
    }
    nextFrom=k;
    return -1;
}

template <typename T>
int  MFifo<T>::indexOf(const QString& mask, const T* tPtr, int len, int from, int& nextFrom) const{//return -1 if not found
    int i=0,n,k;
    int fifoLen;
    bool result;
    fifoLen=this->innerLength_RSide();

    if(0==tPtr)return -1;
    if(mask.length()!=len)return -1;

    for(i=from;i+len<=fifoLen;i++){
        result=true;
        for(n=0;result&&(n<len);n++){
            if(MFifo<T>::NotCare!=mask[n]){
                if(MFifo<T>::Equal==mask[n]){
                    if(tPtr[n]!=(*this)[i+n])result=false;
                }
                else{
                    if(tPtr[n]==(*this)[i+n])result=false;
                }
            }
        }
        if(result){
            nextFrom=i+1;
            return i;
        }
    }

    //尽量比对片段，在tPtr中找剩下的mFifo数据段
    for(k=i;k<fifoLen;k++){
        result=true;
        for(n=0;result&&(n<len)&&(n<fifoLen-k);n++){
            if(MFifo<T>::NotCare!=mask[n]){
                if(MFifo<T>::Equal==mask[n]){
                    if(tPtr[n]!=(*this)[k+n])result=false;
                }
                else{
                    if(tPtr[n]==(*this)[k+n])result=false;
                }
            }
        }
        if(result){
            nextFrom=k;
            return -1;
        }
    }
    nextFrom=k;
    return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////isEmpty
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
bool MFifo<T>::isEmpty() const{
    if(this->wAddr==this->rAddr){
        return true;
    }
    else{
        return false;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////isFull
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
bool MFifo<T>::isFull() const{
    if(1>this->spareSpace()){//Buffer doesn't have enough space for new data (len)
        return true;
    }
    else{
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////isOverflow
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
int  MFifo<T>::isOverflow() const{
    return this->overflow;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////clearOverflow
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void MFifo<T>::clearOverflow(){
    this->overflow=0;
    MFifoStats::stats[mFifoStatsId].overflow=this->overflow;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////depth
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
int MFifo<T>::depth() const{
    return this->fifoDepth;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////removeBefore
//////////////////////////////////////////////////////////////////////////////////////////////////////
///不提供removeBefore（-1）之类的全部清空函数，原因是这种不指定确定位置的情况是跨线程不安全的。
template <typename T>
bool MFifo<T>::removeBefore(int pos){
    int addr;
    if((pos<=0)||(pos>this->innerLength_RSide())){
        return false;
    }

    addr=pos+this->rAddr;
    addr%=this->fifoDepth;
    this->rAddr=addr;

    //update the mFifoStats
    MFifoStats::stats[mFifoStatsId].length=this->innerLength_RSide();
    MFifoStats::stats[mFifoStatsId].rCount+=pos;
    MFifoStats::stats[mFifoStatsId].removeCount+=pos;
    return true;
}

//template <typename T>
//QVector<T>* MFifo<T>::takeBefore(int pos){
//    QVector<T>* newVec;
//    int i;
//    if(pos<0){
//        qDebug()<<"MFifo<T>::takeBefore()=failed! pos<=0!";
//        return nullptr;
//    }
//    if(0==pos){
//        qDebug()<<"MFifo<T>::takeBefore()=failed! pos==0!";
//        return nullptr;
//    }
//    if(pos>this->innerLength_RSide()){
//        qDebug()<<"MFifo<T>::takeBefore()=failed! pos=="<<pos<<";length=="<<this->innerLength_RSide();
//        return nullptr;
//    }

//    newVec=new QVector<T>();
//    for(i=0;i<pos;i++){
//        newVec->append((*this)[i]);
//    }
//    MFifoStats::stats[mFifoStatsId].takeCount+=pos;
//    if(removeBefore(pos)){//因为调用的removeBefore已经维护了mFifoStats，所以不再需要update the mFifoStats
//        MFifoStats::stats[mFifoStatsId].removeCount-=pos;
//    }
//    return newVec;
//}
template<typename T>
std::shared_ptr<QVector<T> > MFifo<T>::takeBefore(int pos)
{
    while (rLockFlag.test_and_set()) {}
    auto newVec=std::make_shared<QVector<T>>();
    int i;
    if((pos<0)||(0==pos)){
        qDebug()<<"MFifo<T>::takeBefore()=failed! pos<=0!";
        rLockFlag.clear();
        return nullptr;
    }
    if(pos>this->innerLength_RSide()){
        qDebug()<<"MFifo<T>::takeBefore()=failed! pos=="<<pos<<";length=="<<this->innerLength_RSide();
        rLockFlag.clear();
        return nullptr;
    }

    for(i=0;i<pos;i++){
        newVec->append(takeAt(i));
    }
    MFifoStats::stats[mFifoStatsId].takeCount+=pos;
    if(removeBefore(pos)){//因为调用的removeBefore已经维护了mFifoStats，所以不再需要update the mFifoStats
        MFifoStats::stats[mFifoStatsId].removeCount-=pos;
    }
    rLockFlag.clear();
    return newVec;
}

template<typename T>
std::shared_ptr<QVector<T> > MFifo<T>::takeAll()
{
    return this->takeBefore(this->innerLength_RSide());
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
////////clear
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
void MFifo<T>::clear(bool overwriteData)
{
    while (rLockFlag.test_and_set()) {}
    while (wLockFlag.test_and_set()) {}
    reset();
    int i;
    if(overwriteData){
        for(i=0;i<this->tVec.length();i++){
            this->tVec[i]=nullT;
        }
    }
    rLockFlag.clear();
    wLockFlag.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////reset
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void MFifo<T>::reset(){
    wAddr=0;
    rAddr=0;
    overflow=0;
    resetMFifoInfo();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////resize
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void MFifo<T>::resize(int depth){
    if(depth<=0)return;
    if(this->depth()==depth)return;
    tVec.resize(depth);
    fifoDepth=depth;
    MFifoStats::stats[mFifoStatsId].depth=this->depth();
    this->reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////length
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
int MFifo<T>::length()const{
    int Buf_wAddr=0;
    int Buf_rAddr=0;
    Buf_wAddr=this->wAddr;
    Buf_rAddr=this->rAddr;
    if(Buf_wAddr>=Buf_rAddr){
        return Buf_wAddr-Buf_rAddr;
    }
    else{
        return this->fifoDepth-Buf_rAddr+Buf_wAddr;
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
////////FIFO name
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void MFifo<T>::setName(const QString name){
    fifoName=name;
    MFifoStats::stats[mFifoStatsId].name=fifoName;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
////////[]  (ring-mode)
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
const T &	MFifo<T>::operator[](int i) const{
    int addr;
//    if(i<0)return NULL;
//    if(i>=this->length())return NULL;
    addr=i+this->rAddr;
    addr%=this->fifoDepth;
    return this->tVec[addr];
}

template <typename T>
T &	MFifo<T>::operator[](int i){
    int addr;
//    if(i<0)return NULL;
//    if(i>=this->length())return NULL;
    addr=i+this->rAddr;
    addr%=this->fifoDepth;
    return this->tVec[addr];
}

template<typename T>
T &MFifo<T>::at(int i)
{
    return (*this)[i];
}

template<typename T>
T &MFifo<T>::last()
{
    return (*this)[this->length()-1];
}

template<typename T>
const T MFifo<T>::takeAt(int i)
{
    int addr;
    T temp;
    addr=i+this->rAddr;
    addr%=this->fifoDepth;
    temp=this->tVec[addr];
    this->tVec[addr]=nullT;
    return temp;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////// (堆-mode)
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
void MFifo<T>::popLast()
{
    if(this->isEmpty())return;
    while (rLockFlag.test_and_set()) {}
    while (wLockFlag.test_and_set()) {}
    int Buf_wAddr_Next;
    Buf_wAddr_Next=this->wAddr-1;
    this->tVec[this->wAddr]=nullT;
    if(0>Buf_wAddr_Next)Buf_wAddr_Next=this->fifoDepth-1;
    this->wAddr=Buf_wAddr_Next;

    //step2:update the mFifoStats
    MFifoStats::stats[mFifoStatsId].length=this->innerLength_RSide();
    MFifoStats::stats[mFifoStatsId].rCount++;
    wLockFlag.clear();
    rLockFlag.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////deleteFirst
//////////////////////////////////////////////////////////////////////////////////////////////////////
//template <typename T>
//void MFifo<T>::deleteFirst(){
//    T* ptr;
//    if(this->isEmpty())return;
//    read(ptr);
//    delete ptr;
//}

#endif // MFIFO_H
