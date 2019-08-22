#ifndef ONEXTREETEXT_H
#define ONEXTREETEXT_H

#include "../OnexTreeItem.h"

class NosTextOpener;

class OnexTreeText : public OnexTreeItem {
Q_OBJECT
public:
    OnexTreeText(const QString &name, NosTextOpener *opener, int fileNumber = 0, int isCompressed = 0,
                 QByteArray content = QByteArray());
    ~OnexTreeText() override;
    QWidget *getPreview() override;
    QString getExportExtension() override;
    int getFileNumber() const;
    int getIsDat() const;
public slots:
    int afterReplace(QByteArray content) override;
    void setFileNumber(int number, bool update = false);
    void setIsDat(bool isCompressed, bool update = false);
    void setTime(QString time, bool update = false);
signals:
    void replaceSignal(QByteArray text);
protected:
    int fileNumber;
    int isCompressed;
    QString time;
    FileInfo *generateInfos() override;
};

#endif // ONEXTREETEXT_H
