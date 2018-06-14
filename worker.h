#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QCoreApplication>

#include "xcifile.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject * qmlObj = nullptr, QObject *parent = nullptr);

    enum ActionType {
        NO_ACT = 0,
        CUT,
        CUT_SPLIT,
        JOIN
    };
    Q_ENUMS(ActionType)

    enum ButtonType {
        BATCH = 0,
        SOURCE,
        DESTINATION,
        PROCESS,
        QUIT,
        EXIT
    };
    Q_ENUMS(ButtonType)

signals:

public slots:
    void processClicked(ButtonType btn, ActionType action = NO_ACT);
    void setSource(QString src);
    void setDest(QString dst){m_DestFolder = dst;}
    bool isSrcDstSet(){return (m_SourcePath.isEmpty() == false && m_DestFolder.isEmpty() == false);}
    void setCheckFreeSpace(bool val) {m_CheckFreeSpace = val;}

    void dataAvailable();

private:
    void executeWork(ActionType act);
    QString m_SourcePath;
    QString m_DestFolder;

    XCIFile *m_SourceFile;

    QObject * qmlObject;
    bool m_QuitPending;
    bool m_CheckFreeSpace;

    QFile *out;
};

Q_DECLARE_METATYPE(Worker::ActionType)
Q_DECLARE_METATYPE(Worker::ButtonType)
#endif // WORKER_H
