#pragma once

#include <QObject>
#include <QVector3D>
#include <openctm.h>

// 前向声明VTK类
class vtkPolyData;
class vtkPoints;
class vtkCellArray;
class vtkFloatArray;

class VTKCTMLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool hasNormals READ hasNormals NOTIFY modelLoaded)
    Q_PROPERTY(bool hasTexCoords READ hasTexCoords NOTIFY modelLoaded)
    Q_PROPERTY(int vertexCount READ vertexCount NOTIFY modelLoaded)
    Q_PROPERTY(int triangleCount READ triangleCount NOTIFY modelLoaded)

public:
    explicit VTKCTMLoader(QObject *parent = nullptr);
    ~VTKCTMLoader();

    QString source() const;
    void setSource(const QString &source);

    bool hasNormals() const;
    bool hasTexCoords() const;
    int vertexCount() const;
    int triangleCount() const;

    Q_INVOKABLE bool loadModel();
    vtkPolyData* getPolyData() const;

    void initHead();

signals:
    void sourceChanged();
    void modelLoaded();
    void errorOccurred(const QString &message);

private:
    QString m_source;
    vtkPolyData* m_polyData;
    bool m_hasNormals;
    bool m_hasTexCoords;
};
