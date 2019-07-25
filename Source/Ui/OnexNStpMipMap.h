#ifndef ONEXNSTPMAPMAP_H
#define ONEXNSTPMAPMAP_H
#include "OnexTreeImage.h"

class OnexNStpMipMap : public OnexTreeImage {
    Q_OBJECT
private:
    int width;
    int height;
    int format;

public:
    OnexNStpMipMap(QByteArray header, QString name, QByteArray content, int width, int height, int format,
                   NosZlibOpener *opener, int id, int creationDate, bool compressed);

    virtual QImage getImage() override;
    virtual ImageResolution getResolution() override;
    QWidget *getInfos() override;
    int getWidth();
    int getHeight();
    int getFormat();
    QByteArray getContent();
    virtual ~OnexNStpMipMap();

public slots:
    virtual int onReplace(QString directory) override;
    virtual void setWidth(int width);
    virtual void setHeight(int height);
    virtual void setFormat(uint8_t format);
    virtual void setFormat(QString format);
};

#endif // ONEXNSTPMAPMAP_H
