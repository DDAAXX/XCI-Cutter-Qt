#include "worker.h"
#include <QDebug>
#include <QThread>

Worker::Worker(QObject *qmlObj, QObject *parent) : QObject(parent)
{
    m_SourcePath = "";
    m_DestFolder = "";
    m_SourceFile = NULL;

    qmlObject = qmlObj;

    m_QuitPending = false;
    m_CheckFreeSpace = true;

    //qDebug()<<_4GBFILE;
}

/*!
 * \brief Worker::processClicked
 * \param src
 * \param dest
 * \param action
 */
void Worker::processClicked(ButtonType btn, ActionType action)
{
    qDebug()<<"clicked"<<btn<<action;

    switch (btn) {
    case BATCH:
        //TODO: open multi file selector
        break;
    case SOURCE:
    case DESTINATION:
        break;
    case PROCESS:
        //do the work
        executeWork(action);
        break;
    case QUIT:
        m_QuitPending = true;
        break;
    case EXIT:
        QCoreApplication::quit();
        break;
    default:
        break;
    }
}

void Worker::setSource(QString src)
{
    m_SourcePath = src;
    m_SourceFile = new XCIFile();
    connect(m_SourceFile, SIGNAL(dataAvailable()), this, SLOT(dataAvailable()));

    m_SourceFile->setInPath(m_SourcePath);
}

void Worker::dataAvailable()
{
    qDebug()<<"data available",
            QMetaObject::invokeMethod(qmlObject, "getData",
                                      Q_ARG(QVariant, m_SourceFile->getCartSize()),
                                      Q_ARG(QVariant, m_SourceFile->getDataSize()),
                                      Q_ARG(QVariant, m_SourceFile->getRealFileSize())
                                      );

    //If filesize is too small or too large, abort
    if (m_SourceFile->getRealFileSize() < m_SourceFile->getDataSize() or m_SourceFile->getRealFileSize() > m_SourceFile->getRealCartSize())
    {
        qDebug()<<"ROM is improperly trimmed or padded. Aborting";
        m_SourceFile = new XCIFile();
    }

    if (m_SourceFile->getDataSize() > _4GBFILE)
    {
        qDebug()<<"Data dimension:"<<m_SourceFile->getDataSize()<<"For FAT32, split the file also";
    }
}

void Worker::executeWork(ActionType act)
{
    quint64 i, remainder;
    QByteArray pad_check;
    QByteArray test;
    QByteArray pad_remainder;
    bool error = false;
    bool split4GB = false;

    switch (act) {
    case CUT_SPLIT:

        split4GB = true;

    case CUT:
        //Verify xci file
        pad_check.resize(ChunkSize);
        pad_check.fill(0xff);

        if (m_SourceFile->getDataSize() == m_SourceFile->getRealFileSize())
        {
            qDebug()<<"ROM is already trimmed";

            QMetaObject::invokeMethod(qmlObject, "setMsgBox",
                                      Q_ARG(QVariant,"ROM is already trimmed!")
                                      );

            QMetaObject::invokeMethod(qmlObject, "abortOp",
                                      Q_ARG(QVariant,false)
                                      );

            return;
        }

        i = (m_SourceFile->getRealCartSize() - m_SourceFile->getDataSize());
        remainder = i - (m_SourceFile->getChunkCount() * ChunkSize);
        pad_remainder.resize(remainder);
        pad_remainder.fill(0xff);

        m_SourceFile->OpenReader();
        m_SourceFile->setInPos(m_SourceFile->getDataSize());

        qDebug()<< m_SourceFile->getRealCartSize() << m_SourceFile->getDataSize();
        qDebug()<< m_SourceFile->getChunkCount();

        QMetaObject::invokeMethod(qmlObject, "setProgessMax",
                                  Q_ARG(QVariant,m_SourceFile->getChunkCount())
                                  );

        //Verify free space
        if (m_CheckFreeSpace)
        {
            for (quint64 y = 0; y < m_SourceFile->getChunkCount(); y++)
            {
                if (m_QuitPending)
                {
                    m_QuitPending = false;
                    return;
                }

                QMetaObject::invokeMethod(qmlObject, "setProgessVal",
                                          Q_ARG(QVariant,y)
                                          );
                qApp->processEvents();
                test.resize(pad_check.length());
                m_SourceFile->InfileStream->read(test.data(),test.length());
                m_SourceFile->setInPos(m_SourceFile->getDataSize() + ((y+1) * pad_check.length()));

                if (test != pad_check)
                {
                    error = true;
                    break;
                }
            }

            if (!error)
            {
                test.resize(pad_remainder.length());
                m_SourceFile->InfileStream->read(test.data(),test.length());
                //qDebug()<<test;
                if (test != pad_remainder)
                {
                    error = true;
                }
            }
        }

        if (error)
        {
            qDebug()<<"Found used space after gamedata! Aborting!";
            QMetaObject::invokeMethod(qmlObject, "setMsgBox",
                                      Q_ARG(QVariant,"Found used space after gamedata! Aborting!")
                                      );
            QMetaObject::invokeMethod(qmlObject, "abortOp",
                                      Q_ARG(QVariant,false)
                                      );
        }
        else
        {
            //Cut
            qDebug()<<"Ready to cut!";
            QMetaObject::invokeMethod(qmlObject, "setProgessVal",
                                      Q_ARG(QVariant,0)
                                      );

            QMetaObject::invokeMethod(qmlObject, "setMsgBox",
                                      Q_ARG(QVariant,"Trimming... Please wait...")
                                      );

            int a = 5;
            while (a > 0) {
                a--;
                qApp->processEvents();
                QThread::msleep(100);
            }


            QString filename = m_SourceFile->InfileStream->fileName();
            int slashPos = filename.lastIndexOf("/");
            filename = filename.mid(slashPos+1);
            filename.chop(4);

            if (split4GB && m_SourceFile->getDataSize() > _4GBFILE)
            {
                QMetaObject::invokeMethod(qmlObject, "setMsgBox",
                                          Q_ARG(QVariant,"Trimming and Splitting file... Please wait...")
                                          );

                int a = 5;
                while (a > 0) {
                    a--;
                    qApp->processEvents();
                    QThread::msleep(100);
                }

                int numFiles = m_SourceFile->getDataSize() / _4GBFILE;

                if (m_SourceFile->getDataSize() - (numFiles * _4GBFILE) > 0)
                {
                    numFiles++;
                }

                createFile(m_SourceFile->InfileStream, m_SourceFile->getDataSize(), numFiles);
            }
            else
            {
                createFile(m_SourceFile->InfileStream, m_SourceFile->getDataSize());
            }
        }


        break;
    case JOIN:
        //TODO
        break;
    default:
        break;
    }
}

void Worker::createFile(QFile *src, quint64 dataSize, qint8 numFiles)
{
    // File open a success
    if (src->isOpen() || src->open(QIODevice::ReadOnly)) {
        QString destName = src->fileName();
        destName = m_DestFolder + destName.mid(destName.lastIndexOf("/"));
        destName.chop(4);

        qDebug() << destName;

        if (numFiles == 1)
        {
            destName.append("_trimmed.xci");
        }
        else
        {
            destName.append("_trimmed.xc");
        }

        for (int i = 0; i < numFiles; i++)
        {
            QFile out;
            if (numFiles > 1)
            {
                out.setFileName(QString("%1%2").arg(destName).arg(i));
            }
            else
            {
                out.setFileName(destName);
            }

            if (out.open(QIODevice::WriteOnly))
            {
                qint64 sz;
                if (numFiles > 1)
                {
                    if ((qint64)dataSize - (_4GBFILE * (i+1)) > 0)
                    {
                        sz = _4GBFILE;
                    }
                    else
                    {
                        sz = dataSize - (_4GBFILE * (i));
                    }
                }
                else
                {
                    sz = dataSize;
                }
                qDebug()<<dataSize<<sz;
                //continue; test, skip write

                quint64 chunkWrite = (sz/ChunkSize);
                quint64 remain = (sz - (chunkWrite * ChunkSize));
                quint64 ii;

                QMetaObject::invokeMethod(qmlObject, "setProgessMax",
                                          Q_ARG(QVariant,chunkWrite)
                                          );

                for (ii = 0; ii < chunkWrite; ii++)
                {
                    if (m_QuitPending)
                    {
                        m_QuitPending = false;

                        out.close();

                        return;
                    }

                    out.write((char*) src->map(0 + (_4GBFILE * i) + (ii*ChunkSize), ChunkSize), ChunkSize); //Copies all data
                    QMetaObject::invokeMethod(qmlObject, "setProgessVal",
                                              Q_ARG(QVariant,ii)
                                              );
                    qApp->processEvents();
                }

                if (remain > 0)
                {
                    out.write((char*) src->map(0 + (_4GBFILE * i) + (ii*ChunkSize), remain), remain); //Copies all data
                }

                out.close();
            }
            else
            {
                qDebug()<<"unable to open file";
                QMetaObject::invokeMethod(qmlObject, "setMsgBox",
                                          Q_ARG(QVariant,"unable to open destination file!")
                                          );
                QMetaObject::invokeMethod(qmlObject, "abortOp",
                                          Q_ARG(QVariant,false)
                                          );
                return;
            }
        }

        QMetaObject::invokeMethod(qmlObject, "endProcess");

        src->close();
    }
}
