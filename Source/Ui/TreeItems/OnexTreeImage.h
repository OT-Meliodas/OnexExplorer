#ifndef ONEXTREEIMAGE_H
#define ONEXTREEIMAGE_H

#include "OnexTreeZlibItem.h"
#include "../Previews/SingleImagePreview.h"
#include "../../Converters/ImageConverter.h"

struct ImageResolution {
    int x;
    int y;
};

class OnexTreeImage : public OnexTreeZlibItem {
Q_OBJECT
public:
    OnexTreeImage(QString name, NosZlibOpener *opener, QByteArray content = QByteArray(), int id = -1, int creationDate = 0, bool compressed = false);
    QWidget *getPreview() override;
    bool isEmpty() override;
    QString getExportExtension() override;
    virtual QImage getImage() = 0;
    virtual ImageResolution getResolution() = 0;
public slots:
    int onReplaceRaw(QString directory) override;
    int afterReplace(QByteArray content) override;
    virtual int afterReplace(QImage image) = 0;
    virtual void setWidth(int width, bool update = false) = 0;
    virtual void setHeight(int height, bool update = false) = 0;
signals:
    void replaceSignal(QImage newImage);
protected:
    static ImageConverter *imageConverter;
    FileInfo *generateInfos() override;
    int saveAsFile(const QString &path, QByteArray content = QByteArray()) override;
    virtual bool hasGoodResolution(int x, int y);
};

#endif // ONEXTREEIMAGE_H
