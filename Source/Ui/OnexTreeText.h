#ifndef ONEXTREETEXT_H
#define ONEXTREETEXT_H
#include "OnexTreeItem.h"

class OnexTreeText : public OnexTreeItem
{
public:
    OnexTreeText(QString name, int fileNumber = 0, int isDat = 0, QByteArray content = QByteArray());
    virtual QWidget* onClicked() override;
};

#endif // ONEXTREETEXT_H
