
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include "vtkctmloader.h"
#include "vtkitem.h"


#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL);

// 禁用 VTK 的弃用警告
#define VTK_LEGACY_SILENT

int main(int argc, char *argv[])
{

    qputenv("QT_OPENGL", "desktop"); // 强制使用桌面OpenGL
    qputenv("QT_ANGLE_PLATFORM", "d3d11"); // 或者尝试使用ANGLE的D3D11

    // QGuiApplication app(argc, argv);

    // 设置应用程序信息（修复警告的关键代码）
    QCoreApplication::setOrganizationName("yrd");
    QCoreApplication::setOrganizationDomain("yrd.com");
    QCoreApplication::setApplicationName("testVtk");


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);



    // 注册 VTK CTM 加载器到 QML
    qmlRegisterType<VTKCTMLoader>("com.yrd.VTKCTMLoader", 1, 0, "VTKCTMLoader");

    //注册场景
    qmlRegisterType<VTKItem>("com.yrd.VTKItem", 1, 0, "VTKItem");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
