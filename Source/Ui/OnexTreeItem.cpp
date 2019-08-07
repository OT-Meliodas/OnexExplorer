#include "OnexTreeItem.h"

OnexTreeItem::OnexTreeItem(QString name, INosFileOpener *opener, QByteArray content)
    : name(name), opener(opener), content(content) {
    this->setText(0, name);
}

QByteArray OnexTreeItem::getContent() {
    return content;
}

bool OnexTreeItem::hasParent() {
    return QTreeWidgetItem::parent();
}

short OnexTreeItem::fromLittleEndianToShort(QByteArray array) {
    return qFromLittleEndian<qint16>(reinterpret_cast<const uchar *>(array.data()));
}

int OnexTreeItem::fromLittleEndianToInt(QByteArray array) {
    return qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(array.data()));
}

float OnexTreeItem::fromLittleEndianToFloat(QByteArray array) {
    return qFromLittleEndian<float>(reinterpret_cast<const uchar *>(array.data()));
}

QByteArray OnexTreeItem::fromShortToLittleEndian(short number) {
    QByteArray writeArray;
    writeArray.resize(2);
    qToLittleEndian<qint16>(number, reinterpret_cast<uchar *>(writeArray.data()));
    return writeArray;
}

QByteArray OnexTreeItem::fromIntToLittleEndian(int number) {
    QByteArray writeArray;
    writeArray.resize(4);
    qToLittleEndian<qint32>(number, reinterpret_cast<uchar *>(writeArray.data()));
    return writeArray;
}

QByteArray OnexTreeItem::fromFloatToLittleEndian(float number) {
    QByteArray writeArray;
    writeArray.resize(4);
    qToLittleEndian<float>(number, reinterpret_cast<uchar *>(writeArray.data()));
    return writeArray;
}

int OnexTreeItem::getContentSize() {
    return content.size();
}

QString OnexTreeItem::getName() {
    return name;
}

void OnexTreeItem::setName(QString name) {
    this->name = name;
}

void OnexTreeItem::setContent(QByteArray content) {
    this->content = content;
}

QString OnexTreeItem::getExportExtension() {
    return "";
}

OnexTreeItem::~OnexTreeItem() {
}

int OnexTreeItem::onExport(QString directory) {
    QMessageBox::warning(NULL, tr("Not yet"), tr("This isn't implemented yet"));
    return 0;
}

int OnexTreeItem::onExportRaw(QString directory) {
    QString path = "";
    if (!directory.endsWith(".bin"))
        path = directory + this->getName() + ".bin";
    else
        path = directory;

    QFile file(path);
    file.open(QIODevice::WriteOnly);
    if (file.write(this->getContent()) == -1) {
        return 0;
    }
    file.close();
    return 1;
}

int OnexTreeItem::onExportAsOriginal(QString path) {
    if (path.isEmpty())
        return 0;

    if (!path.endsWith(".NOS"))
        path += ".NOS";

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(NULL, "Woops", "Couldn't open this file for writing");
        return 0;
    }

    if (file.write(opener->encrypt(this)) == -1) {
        QMessageBox::critical(NULL, "Woops", "Couldn't write to this file");
        return 0;
    }

    file.close();
    QMessageBox::information(NULL, "Yeah", "File exported into .NOS");

    return 1;
}

int OnexTreeItem::onReplace(QString directory) {
    if (this->childCount() > 0) {
        int count = 0;
        for (int i = 0; i < this->childCount(); i++) {
            OnexTreeItem *item = static_cast<OnexTreeItem *>(this->child(i));
            count += item->onReplace(directory);
        }
        return count;
    } else {
        QString path;
        if (!directory.split(".").size() == 0)
            path = directory + this->getName();
        else
            path = directory;
        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
            this->content = file.readAll();
        else {
            QMessageBox::critical(NULL, "Woops", "Couldn't open " + path);
            return 0;
        }
        return 1;
    }
}

int OnexTreeItem::onReplaceRaw(QString directory) {
    QString path;
    if (!directory.endsWith(".bin"))
        path = directory + this->getName() + ".bin";
    else
        path = directory;
    QFile file(path);

    if (file.open(QIODevice::ReadOnly)) {
        content = file.readAll();
        file.close();
        return 1;
    }
    return 0;
}

void OnexTreeItem::onDelete() {
    delete this;
}

void OnexTreeItem::actionClose() {
    QList<QTreeWidgetItem *> selectedItems = this->treeWidget()->selectedItems();

    foreach (auto &item, selectedItems)
        delete item;
}