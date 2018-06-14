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
            QString newNamePath = m_DestFolder.append("/").append(filename).append("_trimmed.xci");

            //TODO: manage image split 4GB if split4GB set

            out = new QFile(newNamePath);

            if(out->open(QIODevice::WriteOnly))
            {
                qDebug()<<m_SourceFile->getDataSize();

                quint64 wChunks = m_SourceFile->getDataSize() / ChunkSize;
                quint64 remain = m_SourceFile->getDataSize() - (ChunkSize * wChunks);

                QMetaObject::invokeMethod(qmlObject, "setProgessMax",
                                          Q_ARG(QVariant,wChunks)
                                          );

                quint64 x;
                for (x = 0; x < wChunks; x++)
                {
                    if (m_QuitPending)
                    {
                        m_QuitPending = false;

                        out->close();
                        out->remove();

                        out->deleteLater();

                        return;
                    }

                    out->write((char*) m_SourceFile->InfileStream->map(0 + (x*ChunkSize), ChunkSize), ChunkSize); //Copies all data
                    QMetaObject::invokeMethod(qmlObject, "setProgessVal",
                                              Q_ARG(QVariant,x)
                                              );
                    qApp->processEvents();
                }

                out->write((char*) m_SourceFile->InfileStream->map(0 + (x*ChunkSize), remain), remain); //Copies all data

                out->close();

                out->deleteLater();

                QMetaObject::invokeMethod(qmlObject, "endProcess");
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
