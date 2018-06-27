#ifndef XCIFILE_H
#define XCIFILE_H

#include <QObject>
#include <QFile>
#include <QDataStream>

#define ChunkSize  (quint64)(((8192 * 1024 ^ 3) - 1))

class XCIFile : public QObject
{
    Q_OBJECT
public:
    explicit XCIFile(QObject *parent = nullptr);
    QFile *InfileStream;
    QFile *OutfileStream;

signals:
    void dataAvailable();

public slots:
    bool getFileOK() {return pFileOK;}
    QString getInPath(){return pInPath;}
    void setInPath(QString val);

    quint64 getDataSize(){return pDataSize;}
    quint64 getCartSize(){return pCartSize;}
    quint64 getRealCartSize(){return ((pCartSize * 1024 - (pCartSize * 0x48)) * 1024 * 1024);}
    quint64 getRealFileSize(){return pRealfileSize;}

    quint64 getChunkCount(){return pChunkCount;}

    bool OpenReader();
    void CloseReaders();
    void setInPos(quint64 pos) {
        if (InfileStream->isOpen() && InfileStream->isReadable())
        {
            InfileStream->seek(pos);
        }
    }

private:
    void ReadSizes();

    quint64 pDataSize, pCartSize, pRealfileSize;
    QString pInPath;
    bool pFileOK;

    quint64 pChunkCount;

};

#endif // XCIFILE_H
