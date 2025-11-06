#include "vtkctmloader.h"
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <QDebug>
#include <QFileInfo>


#include <QTimer>

VTKCTMLoader::VTKCTMLoader(QObject *parent)
    : QObject(parent)
    , m_polyData(vtkPolyData::New())
    , m_hasNormals(false)
    , m_hasTexCoords(false)
{
    qDebug() << "数据加载比较慢";

    // m_source = "/D:/project/aimq模型及仿真数据(1)/aimq模型及仿真数据/八拍.ctm";
    // loadModel();



}

VTKCTMLoader::~VTKCTMLoader()
{
    if (m_polyData) {
        m_polyData->Delete();
    }
}

QString VTKCTMLoader::source() const
{
    return m_source;
}

void VTKCTMLoader::setSource(const QString &source)
{

    if (m_source != source)
    {
        m_source = source;
        emit sourceChanged();
    }
}

bool VTKCTMLoader::hasNormals() const { return m_hasNormals; }
bool VTKCTMLoader::hasTexCoords() const { return m_hasTexCoords; }
int VTKCTMLoader::vertexCount() const { return m_polyData ? m_polyData->GetNumberOfPoints() : 0; }
int VTKCTMLoader::triangleCount() const { return m_polyData ? m_polyData->GetNumberOfCells() : 0; }

vtkPolyData* VTKCTMLoader::getPolyData() const
{
    return m_polyData;
}

bool VTKCTMLoader::loadModel()
{
    qDebug() << "初始化渲染区" << m_source;
    if (m_source.isEmpty()) {
        emit errorOccurred("No source file specified");
        return false;
    }

    // 加载CTM文件
    QString filePath = m_source.toUtf8().constData();
    filePath = filePath.right(filePath.length() - 1);
    qDebug() << "Loading file:" << filePath;


    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    CTMcontext context = ctmNewContext(CTM_IMPORT);
    ctmLoadCustom(context, [](void *buf, CTMuint size, void *userData) -> CTMuint {
        QByteArray *data = static_cast<QByteArray*>(userData);
        CTMuint bytesToRead = qMin(size, static_cast<CTMuint>(data->size()));
        memcpy(buf, data->constData(), bytesToRead);
        data->remove(0, bytesToRead);
        return bytesToRead;
    }, &data);


    // 获取顶点数据
    CTMuint vertexCount = ctmGetInteger(context, CTM_VERTEX_COUNT);
    const CTMfloat *vertices = ctmGetFloatArray(context, CTM_VERTICES);

    vtkPoints* points = vtkPoints::New();
    points->SetNumberOfPoints(vertexCount);
    for (CTMuint i = 0; i < vertexCount; ++i) {
        points->SetPoint(i,
                         vertices[i * 3],
                         vertices[i * 3 + 1],
                         vertices[i * 3 + 2]
                         );
    }

    // 获取索引数据
    CTMuint triangleCount = ctmGetInteger(context, CTM_TRIANGLE_COUNT);
    const CTMuint *indices = ctmGetIntegerArray(context, CTM_INDICES);

    vtkCellArray* triangles = vtkCellArray::New();
    for (CTMuint i = 0; i < triangleCount; ++i) {
        vtkIdType pts[3];
        pts[0] = indices[i * 3];
        pts[1] = indices[i * 3 + 1];
        pts[2] = indices[i * 3 + 2];
        triangles->InsertNextCell(3, pts);
    }

    // qDebug() << "Vertex count:" << vertexCount;
    // qDebug() << "Triangle count:" << triangleCount;

    // 设置几何数据
    m_polyData->SetPoints(points);
    m_polyData->SetPolys(triangles);

    points->Delete();
    triangles->Delete();

    // 获取法线数据
    m_hasNormals = ctmGetInteger(context, CTM_HAS_NORMALS);
    if (m_hasNormals) {
        const CTMfloat *normals = ctmGetFloatArray(context, CTM_NORMALS);
        vtkFloatArray* normalArray = vtkFloatArray::New();
        normalArray->SetNumberOfComponents(3);
        normalArray->SetName("Normals");
        for (CTMuint i = 0; i < vertexCount; ++i) {
            normalArray->InsertNextTuple3(
                normals[i * 3],
                normals[i * 3 + 1],
                normals[i * 3 + 2]
                );
        }
        m_polyData->GetPointData()->SetNormals(normalArray);
        normalArray->Delete();
    }

    // 获取纹理坐标
    m_hasTexCoords = ctmGetInteger(context, CTM_UV_MAP_COUNT) > 0;
    if (m_hasTexCoords) {
        const CTMfloat *texCoords = ctmGetFloatArray(context, CTM_UV_MAP_1);
        vtkFloatArray* texCoordArray = vtkFloatArray::New();
        texCoordArray->SetNumberOfComponents(2);
        texCoordArray->SetName("TextureCoordinates");
        for (CTMuint i = 0; i < vertexCount; ++i) {
            texCoordArray->InsertNextTuple2(
                texCoords[i * 2],
                texCoords[i * 2 + 1]
                );
        }
        m_polyData->GetPointData()->SetTCoords(texCoordArray);
        texCoordArray->Delete();
    }

    ctmFreeContext(context);

    qDebug() << "modelLoaded";
    emit modelLoaded();

    return true;
}

void VTKCTMLoader::initHead()
{
    // 加载CTM文件
    QString filePath = "E:/data/aimq模型及仿真数据/标准头.ctm";
    // filePath = filePath.right(filePath.length() - 1);
    qDebug() << "Loading file:" << filePath;

    //filePath = m_source;

    QFile file(filePath);
    //qDebug() << file.fileName();
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << filePath;
        return ;
    }

    QByteArray data = file.readAll();
    file.close();

    CTMcontext context = ctmNewContext(CTM_IMPORT);
    ctmLoadCustom(context, [](void *buf, CTMuint size, void *userData) -> CTMuint {
        QByteArray *data = static_cast<QByteArray*>(userData);
        CTMuint bytesToRead = qMin(size, static_cast<CTMuint>(data->size()));
        memcpy(buf, data->constData(), bytesToRead);
        data->remove(0, bytesToRead);
        return bytesToRead;
    }, &data);


    // 获取顶点数据
    CTMuint vertexCount = ctmGetInteger(context, CTM_VERTEX_COUNT);
    const CTMfloat *vertices = ctmGetFloatArray(context, CTM_VERTICES);

    vtkPoints* points = vtkPoints::New();
    points->SetNumberOfPoints(vertexCount);
    for (CTMuint i = 0; i < vertexCount; ++i) {
        points->SetPoint(i,
                         vertices[i * 3],
                         vertices[i * 3 + 1],
                         vertices[i * 3 + 2]
                         );
    }

    // 获取索引数据
    CTMuint triangleCount = ctmGetInteger(context, CTM_TRIANGLE_COUNT);
    const CTMuint *indices = ctmGetIntegerArray(context, CTM_INDICES);

    vtkCellArray* triangles = vtkCellArray::New();
    for (CTMuint i = 0; i < triangleCount; ++i) {
        vtkIdType pts[3];
        pts[0] = indices[i * 3];
        pts[1] = indices[i * 3 + 1];
        pts[2] = indices[i * 3 + 2];
        triangles->InsertNextCell(3, pts);
    }

    // qDebug() << "Vertex count:" << vertexCount;
    // qDebug() << "Triangle count:" << triangleCount;

    // 设置几何数据
    m_polyData->SetPoints(points);
    m_polyData->SetPolys(triangles);

    points->Delete();
    triangles->Delete();

    // 获取法线数据
    m_hasNormals = ctmGetInteger(context, CTM_HAS_NORMALS);
    if (m_hasNormals) {
        const CTMfloat *normals = ctmGetFloatArray(context, CTM_NORMALS);
        vtkFloatArray* normalArray = vtkFloatArray::New();
        normalArray->SetNumberOfComponents(3);
        normalArray->SetName("Normals");
        for (CTMuint i = 0; i < vertexCount; ++i) {
            normalArray->InsertNextTuple3(
                normals[i * 3],
                normals[i * 3 + 1],
                normals[i * 3 + 2]
                );
        }
        m_polyData->GetPointData()->SetNormals(normalArray);
        normalArray->Delete();
    }

    // 获取纹理坐标
    m_hasTexCoords = ctmGetInteger(context, CTM_UV_MAP_COUNT) > 0;
    if (m_hasTexCoords) {
        const CTMfloat *texCoords = ctmGetFloatArray(context, CTM_UV_MAP_1);
        vtkFloatArray* texCoordArray = vtkFloatArray::New();
        texCoordArray->SetNumberOfComponents(2);
        texCoordArray->SetName("TextureCoordinates");
        for (CTMuint i = 0; i < vertexCount; ++i) {
            texCoordArray->InsertNextTuple2(
                texCoords[i * 2],
                texCoords[i * 2 + 1]
                );
        }
        m_polyData->GetPointData()->SetTCoords(texCoordArray);
        texCoordArray->Delete();
    }

    ctmFreeContext(context);
    emit modelLoaded();

    return;
}
