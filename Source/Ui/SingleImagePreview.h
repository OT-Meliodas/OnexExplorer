#ifndef SINGLEIMAGEPREVIEW_H
#define SINGLEIMAGEPREVIEW_H

#include <QWidget>
#include <QtEndian>
#include <QDebug>
#include <iostream>

namespace Ui {
class SingleImagePreview;
}

class SingleImagePreview : public QWidget
{
    Q_OBJECT

public:
    explicit SingleImagePreview(QByteArray content, QWidget *parent = 0);
    ~SingleImagePreview();
private:
    Ui::SingleImagePreview *ui;
    qint16 byteArrayToShort(QByteArray array);
    QImage createQImage(int w, int h, qint8 formatType, QByteArray &content);
};

#endif // SINGLEIMAGEPREVIEW_H
