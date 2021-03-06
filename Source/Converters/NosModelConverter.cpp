#include "NosModelConverter.h"
#include "LittleEndianConverter.h"
#include <QDebug>
#include <QtEndian>

NosModelConverter::NosModelConverter() {
    this->littleEndianConverter = new LittleEndianConverter();
}

Model *NosModelConverter::fromBinary(const QByteArray &obj) {
    auto *model = new Model();
    float uvScale = littleEndianConverter->fromFloat(obj.mid(0x30, 4));
    int verticesCount = littleEndianConverter->fromShort(obj.mid(0x34, 2));
    int offset = 0x36;
    model->uvScale = uvScale;
    model->vertices = readNosVertices(obj, offset, verticesCount);
    model->uv = readNosUV(obj, offset, verticesCount, uvScale);
    model->normals = readNosNormals(obj, offset, verticesCount);
    QVector<ModelGroup> faces = readNosFaces(obj, offset);
    model = readModelConstruction(obj, offset, model, faces);
    return model;
}

QByteArray NosModelConverter::toBinary(Model *model) {
    QByteArray newContent;
    newContent.append(littleEndianConverter->toFloat(model->uvScale));
    newContent.append(littleEndianConverter->toShort(model->vertices.size()));
    for (auto &vertex : model->vertices) {
        newContent.append(littleEndianConverter->toFloat(vertex.x()));
        newContent.append(littleEndianConverter->toFloat(vertex.y()));
        newContent.append(littleEndianConverter->toFloat(vertex.z()));
    }
    for (int i = 0; i < model->uv.size(); i++) {
        float u = model->uv[i].x();
        u /= model->uvScale;
        float v = model->uv[i].y();
        v = (1.0 - v) / model->uvScale;
        newContent.append(littleEndianConverter->toShort(u));
        newContent.append(littleEndianConverter->toShort(v));
    }
    for (auto &normal : model->normals) {
        uint8_t x = normal.x() * 0x7F;
        uint8_t y = normal.y() * 0x7F;
        uint8_t z = normal.z() * 0x7F;
        newContent.append(x);
        newContent.append(y);
        newContent.append(z);
    }
    newContent.append(littleEndianConverter->toShort(model->groups.size()));
    for (auto &group : model->groups) {
        newContent.append(littleEndianConverter->toShort(group.faces.size() * 3)); // GroupSize
        for (auto &face : group.faces) {
            newContent.append(littleEndianConverter->toShort(face.x()));
            newContent.append(littleEndianConverter->toShort(face.y()));
            newContent.append(littleEndianConverter->toShort(face.z()));
        }
    }
    newContent.append(littleEndianConverter->toShort(model->objects.size())); // ObjectCount
    for (int m = 0; m < model->objects.size(); m++) {
        newContent.append(littleEndianConverter->toFloat(model->objects[m].position.x())); // x
        newContent.append(littleEndianConverter->toFloat(model->objects[m].position.y())); // y
        newContent.append(littleEndianConverter->toFloat(model->objects[m].position.z())); // z

        newContent.append(littleEndianConverter->toShort(model->objects[m].rotation.x() * 0x7FFF)); // x rotation
        newContent.append(littleEndianConverter->toShort(model->objects[m].rotation.y() * 0x7FFF)); // y rotation
        newContent.append(littleEndianConverter->toShort(model->objects[m].rotation.z() * 0x7FFF)); // z rotation
        newContent.append(littleEndianConverter->toShort(model->objects[m].rotation.w() * 0x7FFF)); // w rotation

        newContent.append(littleEndianConverter->toFloat(model->objects[m].scale.x())); // x scale
        newContent.append(littleEndianConverter->toFloat(model->objects[m].scale.y())); // y scale
        newContent.append(littleEndianConverter->toFloat(model->objects[m].scale.z())); // z scale

        newContent.append(
                littleEndianConverter->toShort(model->objects[m].animationPositions.size())); // translationFrameCount
        for (auto &animationPosition : model->objects[m].animationPositions) {
            newContent.append(littleEndianConverter->toShort(animationPosition.timestamp));
            newContent.append(littleEndianConverter->toFloat(animationPosition.position.x()));
            newContent.append(littleEndianConverter->toFloat(animationPosition.position.y()));
            newContent.append(littleEndianConverter->toFloat(animationPosition.position.z()));
        }
        newContent.append(littleEndianConverter->toShort(model->objects[m].animationRotations.size())); // rotationFrameCount
        for (auto &animationRotation : model->objects[m].animationRotations) {
            newContent.append(littleEndianConverter->toShort(animationRotation.timestamp));
            newContent.append(littleEndianConverter->toShort(animationRotation.rotation.x() * 0x7FFF));
            newContent.append(littleEndianConverter->toShort(animationRotation.rotation.y() * 0x7FFF));
            newContent.append(littleEndianConverter->toShort(animationRotation.rotation.z() * 0x7FFF));
            newContent.append(littleEndianConverter->toShort(animationRotation.rotation.w() * 0x7FFF));
        }
        newContent.append(littleEndianConverter->toShort(model->objects[m].animationScales.size())); // scaleFrameCount
        for (auto &animationScale : model->objects[m].animationScales) {
            newContent.append(littleEndianConverter->toShort(animationScale.timestamp));
            newContent.append(littleEndianConverter->toFloat(animationScale.scale.x()));
            newContent.append(littleEndianConverter->toFloat(animationScale.scale.y()));
            newContent.append(littleEndianConverter->toFloat(animationScale.scale.z()));
        }
        newContent.append(littleEndianConverter->toShort(model->objects[m].groups.size()));
        for (int i = 0; i < model->objects[m].groups.size(); i++) {
            newContent.append(littleEndianConverter->toInt(model->groups[model->objects[m].groups[i]].texture));
            newContent.append((char) 0);                                                                    // bool
            newContent.append(littleEndianConverter->toShort(model->groups[model->objects[m].groups[i]].number)); // group
        }
        newContent.append(littleEndianConverter->toShort(0));
    }
    return newContent;
}

QVector<QVector3D> NosModelConverter::readNosVertices(const QByteArray &obj, int &offset, int verticesCount) {
    QVector<QVector3D> vertices;
    for (int i = 0; i < verticesCount; i++) {
        float x = littleEndianConverter->fromFloat(obj.mid(offset, 4));
        float y = littleEndianConverter->fromFloat(obj.mid(offset + 4, 4));
        float z = littleEndianConverter->fromFloat(obj.mid(offset + 8, 4));
        vertices.append(QVector3D(x, y, z));
        offset += 12;
    }
    return vertices;
}

QVector<QVector3D> NosModelConverter::readNosNormals(const QByteArray &obj, int &offset, int verticesCount) {
    QVector<QVector3D> normals;
    for (int i = 0; i < verticesCount; i++) {
        float x = (float) obj.at(offset) / 0x7F;
        float y = (float) obj.at(offset + 1) / 0x7F;
        float z = (float) obj.at(offset + 2) / 0x7F;
        normals.append(QVector3D(x, y, z));
        offset += 3;
    }
    return normals;
}

QVector<QVector2D> NosModelConverter::readNosUV(const QByteArray &obj, int &offset, int verticesCount, float uvScale) {
    QVector<QVector2D> uv;
    for (int i = 0; i < verticesCount; i++) {
        float u = uvScale * littleEndianConverter->fromShort(obj.mid(offset, 2));
        float v = 1.0 - (uvScale * littleEndianConverter->fromShort(obj.mid(offset + 2, 2)));
        uv.append(QVector2D(u, v));
        offset += 4;
    }
    return uv;
}

QVector<ModelGroup> NosModelConverter::readNosFaces(const QByteArray &obj, int &offset) {
    QVector<ModelGroup> groups;
    int groupCount = littleEndianConverter->fromShort(obj.mid(offset, 2));
    offset += 2;
    for (int g = 0; g < groupCount; g++) {
        ModelGroup group;
        group.number = g;
        int points = (uint16_t) littleEndianConverter->fromShort(obj.mid(offset, 2));
        offset += 2;
        for (int i = 0; i < points / 3; i++) {
            int p1 = littleEndianConverter->fromShort(obj.mid(offset, 2));
            int p2 = littleEndianConverter->fromShort(obj.mid(offset + 2, 2));
            int p3 = littleEndianConverter->fromShort(obj.mid(offset + 4, 2));
            group.faces.append(QVector3D(p1, p2, p3));
            offset += 6;
        }
        groups.append(group);
    }
    return groups;
}

Model *NosModelConverter::readModelConstruction(const QByteArray &obj, int &offset, Model *model, QVector<ModelGroup> groups) {
    int parts = littleEndianConverter->fromShort(obj.mid(offset, 2));
    offset += 2;
    for (int p = 0; p < parts; p++) {
        ModelObject modelPart;
        float x = littleEndianConverter->fromFloat(obj.mid(offset, 4));
        float y = littleEndianConverter->fromFloat(obj.mid(offset + 4, 4));
        float z = littleEndianConverter->fromFloat(obj.mid(offset + 8, 4));
        modelPart.position = QVector3D(x, y, z);
        offset += 12;
        float rotation_x = (float) littleEndianConverter->fromShort(obj.mid(offset, 2)) / 0x7FFF;
        float rotation_y = (float) littleEndianConverter->fromShort(obj.mid(offset + 2, 2)) / 0x7FFF;
        float rotation_z = (float) littleEndianConverter->fromShort(obj.mid(offset + 4, 2)) / 0x7FFF;
        float rotation_w = (float) littleEndianConverter->fromShort(obj.mid(offset + 6, 2)) / 0x7FFF;
        modelPart.rotation = QVector4D(rotation_x, rotation_y, rotation_z, rotation_w);
        offset += 8;
        float scale_x = littleEndianConverter->fromFloat(obj.mid(offset, 4));
        float scale_y = littleEndianConverter->fromFloat(obj.mid(offset + 4, 4));
        float scale_z = littleEndianConverter->fromFloat(obj.mid(offset + 8, 4));
        modelPart.scale = QVector3D(scale_x, scale_y, scale_z);
        offset += 12;
        int translationFrameCount = littleEndianConverter->fromShort(obj.mid(offset, 2));
        offset += 2;
        for (int i = 0; i < translationFrameCount; i++) {
            short time = littleEndianConverter->fromShort(obj.mid(offset, 2));
            float frame_x = littleEndianConverter->fromFloat(obj.mid(offset + 2, 4));
            float frame_y = littleEndianConverter->fromFloat(obj.mid(offset + 6, 4));
            float frame_z = littleEndianConverter->fromFloat(obj.mid(offset + 10, 4));
            ModelAnimationPositionFrame aPos;
            aPos.timestamp = time;
            aPos.position = QVector3D(frame_x, frame_y, frame_z);
            modelPart.animationPositions.append(aPos);
            offset += 14;
        }
        int rotationFrameCount = littleEndianConverter->fromShort(obj.mid(offset, 2));
        offset += 2;
        for (int i = 0; i < rotationFrameCount; i++) {
            short time = littleEndianConverter->fromShort(obj.mid(offset, 2));
            float frame_rotation_x = (float) littleEndianConverter->fromShort(obj.mid(offset + 2, 2)) / 0x7FFF;
            float frame_rotation_y = (float) littleEndianConverter->fromShort(obj.mid(offset + 4, 2)) / 0x7FFF;
            float frame_rotation_z = (float) littleEndianConverter->fromShort(obj.mid(offset + 6, 2)) / 0x7FFF;
            float frame_rotation_w = (float) littleEndianConverter->fromShort(obj.mid(offset + 8, 2)) / 0x7FFF;
            ModelAnimationRotationFrame aPos;
            aPos.timestamp = time;
            aPos.rotation = QVector4D(frame_rotation_x, frame_rotation_y, frame_rotation_z, frame_rotation_w);
            modelPart.animationRotations.append(aPos);
            offset += 10;
        }
        int scaleFrameCount = littleEndianConverter->fromShort(obj.mid(offset, 2));
        offset += 2;
        for (int i = 0; i < scaleFrameCount; i++) {
            short time = littleEndianConverter->fromShort(obj.mid(offset, 2));
            float frame_scale_x = littleEndianConverter->fromFloat(obj.mid(offset + 2, 4));
            float frame_scale_y = littleEndianConverter->fromFloat(obj.mid(offset + 6, 4));
            float frame_scale_z = littleEndianConverter->fromFloat(obj.mid(offset + 10, 4));
            ModelAnimationScaleFrame aPos;
            aPos.timestamp = time;
            aPos.scale = QVector3D(frame_scale_x, frame_scale_y, frame_scale_z);
            modelPart.animationScales.append(aPos);
            offset += 14;
        }
        int c = littleEndianConverter->fromShort(obj.mid(offset, 2));
        offset += 2;
        for (int i = 0; i < c; i++) {
            int img = littleEndianConverter->fromInt(obj.mid(offset, 4));
            offset += 4;
            bool smooth = obj.at(offset); // maybe means something else
            offset++;
            int grp = littleEndianConverter->fromShort(obj.mid(offset, 2)); // equals i?
            groups[grp].texture = img;
            model->groups.append(groups[grp]);
            modelPart.groups.append(grp);
            offset += 2;
        }
        littleEndianConverter->fromShort(obj.mid(offset, 2)); // seems to be always 0?
        offset += 2;
        model->objects.append(modelPart);
    }
    return model;
}
