#ifndef VTKITEM_H
#define VTKITEM_H

// #pragma once
// // 在你的 .cpp 文件最开头添加
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions>
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkctmloader.h"
#include <vtkRenderWindowInteractor.h>

#include <QObject>

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;
class vtkActor;
class vtkPolyDataMapper;


class VTKItem : public QQuickFramebufferObject, protected QOpenGLFunctions
{
    Q_OBJECT

    Q_PROPERTY(VTKCTMLoader* loader READ loader WRITE setLoader NOTIFY loaderChanged)

    Q_PROPERTY(bool mouseEnabled READ mouseEnabled WRITE setMouseEnabled NOTIFY mouseEnabledChanged FINAL)

public:
    VTKItem(QQuickItem *parent = nullptr);
    ~VTKItem();

    /* ================初始化运行的声明周期函数================= */
    Renderer *createRenderer() const override;
    void componentComplete() override;
    /* ================初始化运行的声明周期函数================= */

    VTKCTMLoader* loader() const;
    void setLoader(VTKCTMLoader *loader);


    // 鼠标操作属性
    bool mouseEnabled() const { return m_mouseEnabled; }
    void setMouseEnabled(bool enabled);
 // Q_INVOKABLE void zoomIn();



public:
    vtkGenericOpenGLRenderWindow* m_renderWindow;
    VTKCTMLoader* m_loader;
    bool m_initialized;
    vtkRenderer* m_renderer;

    int m_shiTuType;
    double m_bounds[6];
    double m_center[3];
    double m_size[3];

    vtkRenderWindowInteractor* m_interactor = nullptr;


    //鼠标事件
    void setupInteractor();
    void mousePressEvent(QMouseEvent *event) override;
    void handleLeftButtonPress(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void handleModelLoaded();




public slots:
    void zhengshitu();

    void setCamera();

signals:
     void loaderChanged();

    void mouseEnabledChanged();

private:

    vtkActor* m_actor;
    vtkPolyDataMapper* m_mapper;
    bool m_showWireframe;

    bool m_mouseEnabled;
    QPoint m_lastMousePos;
    bool m_isRotating = false;

    vtkInteractorStyleTrackballCamera* m_interactorStyle = nullptr;


    void handleLeftButtonMove(QMouseEvent *event);

};

#endif // VTKITEM_H
