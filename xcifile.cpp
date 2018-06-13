#include "xcifile.h"
#include <QDebug>
#include <QtEndian>

XCIFile::XCIFile(QObject *parent) : QObject(parent)
{
    pFileOK = true;
    pInPath = "";//in.replace("file://","");
//    pOutPath = "";//out.replace("file://","");
    pDataSize = 0;
    pCartSize = 0;
    pRealfileSize = 0;

    InfileStream = new QFile();
//    OutfileStream = new QFile();
}

/*!
 * \brief XCIFile::setOutPath
 * \param val
 */
//void XCIFile::setOutPath(QString val)
//{
//    pOutPath = val.replace("file://","");

//    OutfileStream = new QFile(pOutPath);
//    if (OutfileStream->open(QIODevice::WriteOnly))
//        bw = new QDataStream(OutfileStream);
//}

/*!
 * \brief XCIFile::setInPath
 * \param val
 */
void XCIFile::setInPath(QString val)
{
    pInPath = val.replace("file://","");

    InfileStream = new QFile(pInPath);
//    if (InfileStream->open(QIODevice::ReadOnly))
//        br = new QDataStream(InfileStream);

    ReadSizes();
}

/*!
 * \brief XCIFile::ReadSizes
 */
void XCIFile::ReadSizes()
{
    if (OpenReader())
    {
        InfileStream->seek(269);

        char cartType;
        InfileStream->read(&cartType,1);

        qDebug()<<cartType;

        switch ((quint8)cartType) {
        case 248:
            pCartSize = 1904;
            break;
        case 240:
            pCartSize = 3808;
            break;
        case 224:
            pCartSize = 7616;
            break;
        case 225:
            pCartSize = 15232;
            break;
        case 226:
            pCartSize = 30464;
            break;
        default:
            qDebug()<<"The source file doesn't look like an XCI file", "Can't determine cartridge size!";
            break;
        }

        qDebug()<<pCartSize;

        InfileStream->seek(280);
        char dataSizeArray[4];
        InfileStream->read(&dataSizeArray[0],4);

        QByteArray test(dataSizeArray);

        quint32 ds = qFromLittleEndian<quint32>(test.data());

        pDataSize = 512 + (ds * 512);
        pChunkCount = pDataSize / ChunkSize + 1;

        qDebug()<<pDataSize<<pChunkCount;

        pRealfileSize = QFile(pInPath).size();
        qDebug()<<pRealfileSize;

        if (pInPath.endsWith("xc0"))
        {
            for (quint32 i=0; i < ChunkSize; i++)
            {
                QString fname = QString("%1%2").arg(pInPath.left(pInPath.length() - 1)).arg(i);
                if ( QFile(fname).exists() )
                {
                    pRealfileSize += QFile(fname).size();
                }
                else
                {
                    qDebug()<<"Make sure that all parts of the dump are accessible. Dump incomplete!";
                    pDataSize = 0;
                    pCartSize = 0;
                    pRealfileSize = 0;
                    pFileOK = false;
                }
            }
        }

        if (pRealfileSize < 32*1024)
        {
            qDebug()<<"The source file doesn't look like an XCI file, File to small!";
            pDataSize = 0;
            pCartSize = 0;
            pRealfileSize = 0;
            pFileOK = false;
        }

        CloseReaders();
    }
    else
    {
        qDebug()<<"Cannot open file readers!";
        pDataSize = 0;
        pCartSize = 0;
        pRealfileSize = 0;
        pFileOK = false;
    }

    emit dataAvailable();
}

/*!
 * \brief XCIFile::OpenReader
 */
bool XCIFile::OpenReader()
{
    bool ret1 = true;
//    bool ret2 = true;

    if (InfileStream->isOpen() == false && QFile(pInPath).exists())
    {
        InfileStream = new QFile(pInPath);
        ret1 = InfileStream->open(QIODevice::ReadOnly);
//        if (ret1)
//            br = new QDataStream(InfileStream);
    }

//    if (OutfileStream->isOpen() == false && pOutPath != "")
//    {
//        OutfileStream = new QFile(pOutPath);
//        ret2 = OutfileStream->open(QIODevice::WriteOnly);
//        if (ret2)
//            bw = new QDataStream(OutfileStream);
//    }

    return ret1 /*&& ret2*/;
}

/*!
 * \brief XCIFile::CloseReader
 */
void XCIFile::CloseReaders()
{
    InfileStream->close();
    //OutfileStream->close();
}
