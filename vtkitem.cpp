#include "vtkitem.h"
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <vtkGenericOpenGLRenderWindow.h>
#include <VTKRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkOpenGLError.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkNew.h>
#include <QTimer>
#include <qguiapplication.h>
#include <QThread>
#include <vtkLight.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGenericRenderWindowInteractor.h>

#include <vtkTransform.h>
#include <vtkLinearTransform.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <vtkPointData.h>
#include <vtkPolyDataNormals.h>
#include <vtkFloatArray.h>
#include <vtkTransformPolyDataFilter.h>

#include <vtkUnsignedCharArray.h>
#include <vtkCellData.h>
#include <cstdlib>
#include <ctime>
#include <vector>

#include <QMouseEvent>
#include <QElapsedTimer>

/**
 * @class VTKRenderer2
 * @brief 自定义渲染器类，继承自QQuickFramebufferObject::Renderer
 *
 * 负责VTK场景的OpenGL渲染工作，与QML的FramebufferObject集成
 */
class VTKRenderer2 : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{
public:
    /**
     * @brief 构造函数
     * @param item 指向父VTKItem的指针
     */
    VTKRenderer2(VTKItem *item) : m_item(item), m_contextInitialized(false) {
        qDebug() << "[渲染器] VTKRenderer2构造函数被调用";
    }

    /**
     * @brief 重写渲染函数，执行每帧的OpenGL渲染
     */
    void render() override {
        qDebug() << "[渲染器] 开始渲染帧，关联的VTKItem:" << m_item;

        // 检查渲染窗口和初始化状态
        if (!m_item->m_renderWindow || !m_item->m_initialized) {
            qDebug() << "[渲染器] 渲染窗口未初始化，跳过渲染";
            return;
        }

        // 初始化OpenGL函数
        initializeOpenGLFunctions();
        glUseProgram(0); // 确保不使用着色器程序

        try {
            qDebug() << "[渲染器] 开始执行VTK渲染";
            // 调用VTK渲染窗口的渲染方法
            m_item->m_renderWindow->Render();
            qDebug() << "[渲染器] VTK渲染完成";

        } catch (const std::exception& e) {
            qWarning() << "[渲染器] 渲染过程中发生异常:" << e.what();
        }

        qDebug() << "[渲染器] 单帧渲染完成";
    }

private:
    VTKItem *m_item;        ///< 指向父VTKItem的指针
    bool m_contextInitialized; ///< OpenGL上下文初始化标志
};

/**
 * @brief VTKItem构造函数
 * @param parent 父QQuickItem
 *
 * 初始化VTKItem的所有成员变量，设置鼠标事件处理
 */
VTKItem::VTKItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , m_renderWindow(nullptr)
    , m_initialized(false)
    , m_renderer(nullptr)
    , m_viewType(0)
    , m_bounds()
    , m_center()
    , m_size()
    , m_loader(nullptr)
    , m_actor(nullptr)
    , m_mapper(nullptr)
    , m_mouseEnabled(false)
    , m_lastMousePos()
    , m_isRotating(false)
    , m_interactorStyle(nullptr)
    , m_interactor(nullptr)
{
    qDebug() << "[VTKItem] 构造函数开始执行";

    // 设置FramebufferObject垂直镜像（适应不同坐标系）
    setMirrorVertically(true);

    // 启用鼠标事件处理
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);

    qDebug() << "[VTKItem] 基本事件处理设置完成";

    // 监听组件尺寸变化信号
    connect(this, &QQuickItem::widthChanged, this, [this]() {
        qDebug() << "[VTKItem] 宽度发生变化，新宽度:" << width();
        if(m_renderWindow && m_initialized) {
            m_renderWindow->SetSize(width(), height());
            update(); // 触发重绘
            qDebug() << "[VTKItem] 渲染窗口尺寸已更新";
        }
    });

    connect(this, &QQuickItem::heightChanged, this, [this]() {
        qDebug() << "[VTKItem] 高度发生变化，新高度:" << height();
        if(m_renderWindow && m_initialized) {
            m_renderWindow->SetSize(width(), height());
            update(); // 触发重绘
            qDebug() << "[VTKItem] 渲染窗口尺寸已更新";
        }
    });

    qDebug() << "[VTKItem] 构造函数执行完成";
}

/**
 * @brief 设置鼠标交互使能状态
 * @param enabled 是否启用鼠标交互
 */
void VTKItem::setMouseEnabled(bool enabled)
{
    qDebug() << "[VTKItem] 设置鼠标交互状态:" << (enabled ? "启用" : "禁用");

    if (m_mouseEnabled != enabled) {
        m_mouseEnabled = enabled;

        // 根据使能状态设置接受的鼠标按钮
        setAcceptedMouseButtons(enabled ? Qt::AllButtons : Qt::NoButton);
        setAcceptHoverEvents(enabled);

        // 发出状态改变信号
        emit mouseEnabledChanged();

        qDebug() << "[VTKItem] 鼠标交互状态已更新:" << (enabled ? "已启用" : "已禁用");
    }
}

/**
 * @brief 设置模型加载器
 * @param loader 指向VTKCTMLoader的指针
 */
void VTKItem::setLoader(VTKCTMLoader *loader)
{
    qDebug() << "[VTKItem] 设置模型加载器，原加载器:" << m_loader << "新加载器:" << loader;

    if (m_loader != loader) {
        // 断开原有加载器的信号连接
        if (m_loader) {
            disconnect(m_loader, &VTKCTMLoader::modelLoaded, this, &VTKItem::handleModelLoaded);
            qDebug() << "[VTKItem] 已断开与原加载器的信号连接";
        }

        // 更新加载器指针
        m_loader = loader;

        if (m_loader) {
            // 连接新加载器的模型加载完成信号
            connect(m_loader, &VTKCTMLoader::modelLoaded, this, &VTKItem::handleModelLoaded);
            qDebug() << "[VTKItem] 已连接新加载器的模型加载信号";

            // 立即处理可能已加载的模型
            qDebug() << "[VTKItem] 开始处理模型加载";
            handleModelLoaded();
        }

        // 发出加载器改变信号
        emit loaderChanged();
        qDebug() << "[VTKItem] 加载器设置完成";
    }
}

/**
 * @brief 获取当前模型加载器
 * @return 指向当前VTKCTMLoader的指针
 */
VTKCTMLoader* VTKItem::loader() const
{
    return m_loader;
}

/**
 * @brief 创建渲染器实例
 * @return 指向新创建的VTKRenderer2的指针
 *
 * 此函数由QQuickFramebufferObject在需要时调用
 */
QQuickFramebufferObject::Renderer *VTKItem::createRenderer() const
{
    qDebug() << "[VTKItem] 创建FramebufferObject渲染器";
    return new VTKRenderer2(const_cast<VTKItem*>(this));
}

/**
 * @brief VTKItem析构函数
 *
 * 清理所有VTK相关资源，防止内存泄漏
 */
VTKItem::~VTKItem()
{
    qDebug() << "[VTKItem] 析构函数开始执行，清理资源";

    // 注意：主要的资源清理应在VTK的智能指针或手动删除中完成
    qDebug() << "[VTKItem] 析构函数执行完成";
}

/**
 * @brief 组件完成初始化回调函数
 *
 * 在QML组件完全初始化后调用，用于延迟初始化VTK相关资源
 */
void VTKItem::componentComplete()
{
    qDebug() << "[VTKItem] 组件开始完成初始化";

    // 延迟初始化，确保OpenGL上下文就绪
    QTimer::singleShot(10, this, [this]() {
        qDebug() << "[VTKItem] 开始延迟初始化VTK资源";

        // 调用父类的组件完成方法
        QQuickFramebufferObject::componentComplete();

        // 创建VTK渲染窗口
        m_renderWindow = vtkGenericOpenGLRenderWindow::New();
        qDebug() << "[VTKItem] VTK渲染窗口创建完成";

        // 创建VTK渲染器
        m_renderer = vtkRenderer::New();
        qDebug() << "[VTKItem] VTK渲染器创建完成";

        // 配置渲染窗口为离屏渲染模式
        m_renderWindow->SetOffScreenRendering(true);
        m_renderWindow->SetMapped(1);
        qDebug() << "[VTKItem] 渲染窗口配置为离屏渲染模式";

        // 将渲染器添加到渲染窗口
        m_renderWindow->AddRenderer(m_renderer);
        qDebug() << "[VTKItem] 渲染器已添加到渲染窗口";

        // 设置渲染器背景色（深灰色）
        m_renderer->SetBackground(0.1, 0.1, 0.1);
        m_renderer->SetGradientBackground(false); // 禁用渐变背景
        qDebug() << "[VTKItem] 渲染器背景设置完成";

        qDebug() << "[VTKItem] 当前组件尺寸 - 宽度:" << width() << "高度:" << height();

        // 设置渲染窗口初始尺寸
        m_renderWindow->SetSize(width(), height());
        qDebug() << "[VTKItem] 渲染窗口尺寸设置完成";

        // 标记初始化完成
        m_initialized = true;
        qDebug() << "[VTKItem] VTK初始化完成标志已设置";

        // 设置交互器
        setupInteractor();
        qDebug() << "[VTKItem] 交互器设置完成";

        // 设置初始相机位置
        setCamera();
        qDebug() << "[VTKItem] 初始相机设置完成";

        qDebug() << "[VTKItem] 延迟初始化全部完成";
    });

    qDebug() << "[VTKItem] 组件初始化完成信号已发出";
}

/**
 * @brief 切换到正视图
 */
void VTKItem::setFrontView()
{
    qDebug() << "[VTKItem] 切换到正视图";
    m_viewType = 0;  // 设置视图类型为正视图
    setCamera();      // 更新相机位置
    update();         // 触发界面更新
    qDebug() << "[VTKItem] 正视图切换完成";
}

/**
 * @brief 根据当前视图类型设置相机参数
 */
void VTKItem::setCamera() {
    qDebug() << "[VTKItem] 开始设置相机参数，当前视图类型:" << m_viewType;

    // 获取当前激活的相机
    vtkCamera* camera = m_renderer->GetActiveCamera();
    camera->SetViewAngle(45); // 设置视角为45度
    qDebug() << "[VTKItem] 相机视角已设置为45度";

    // 根据视图类型设置相机参数
    switch(m_viewType) {
    case 0: // 正视图 (Front View)
        qDebug() << "[相机设置] 配置正视图";
        camera->SetPosition(m_center[0], m_center[1] + m_size[1] * 2, m_center[2]);
        camera->SetFocalPoint(m_center[0], m_center[1], m_center[2]);
        camera->SetViewUp(0, 0, 1); // Z轴朝上
        camera->SetClippingRange(0.1, m_size[1] * 2);
        qDebug() << "[相机设置] 正视图配置完成";
        break;

    case 1: // 俯视图 (Top View)
        qDebug() << "[相机设置] 配置俯视图";
        camera->SetPosition(m_center[0], m_center[1], m_center[2] - m_size[2] * 2);
        camera->SetFocalPoint(m_center[0], m_center[1], m_center[2]);
        camera->SetViewUp(0, 1, 0); // Y轴朝上
        camera->SetClippingRange(0.1, m_size[2] * 2);
        qDebug() << "[相机设置] 俯视图配置完成";
        break;

    case 2: // 左视图 (Left View)
        qDebug() << "[相机设置] 配置左视图";
        camera->SetPosition(m_center[0] - m_size[0] * 2, m_center[1], m_center[2]);
        camera->SetFocalPoint(m_center[0], m_center[1], m_center[2]);
        camera->SetViewUp(0, 0, 1); // Z轴朝上
        camera->SetClippingRange(0.1, m_size[0] * 2);
        qDebug() << "[相机设置] 左视图配置完成";
        break;

    case 3: // 右视图 (Right View)
        qDebug() << "[相机设置] 配置右视图";
        camera->SetPosition(m_center[0] + m_size[0] * 2, m_center[1], m_center[2]);
        camera->SetFocalPoint(m_center[0], m_center[1], m_center[2]);
        camera->SetViewUp(0, 0, 1); // Z轴朝上
        camera->SetClippingRange(0.1, m_size[0] * 2);
        qDebug() << "[相机设置] 右视图配置完成";
        break;

    default:
        qWarning() << "[相机设置] 未知视图类型:" << m_viewType << "使用默认正视图";
        camera->SetPosition(m_center[0], m_center[1] + m_size[1] * 2, m_center[2]);
        camera->SetFocalPoint(m_center[0], m_center[1], m_center[2]);
        camera->SetViewUp(0, 0, 1);
        camera->SetClippingRange(0.1, m_size[1] * 2);
        qDebug() << "[相机设置] 默认正视图配置完成";
    }

    // 统一重置相机裁剪范围
    m_renderer->ResetCameraClippingRange();
    qDebug() << "[相机设置] 相机裁剪范围已重置";
    qDebug() << "[VTKItem] 相机设置全部完成";
}

/**
 * @brief 处理模型加载完成的回调函数
 *
 * 当VTKCTMLoader完成模型加载后调用，负责创建可视化管线
 */
void VTKItem::handleModelLoaded()
{
    qDebug() << "[模型加载] 开始处理模型加载完成信号";
    qDebug() << "[模型加载] 当前状态 - 加载器:" << m_loader << "初始化状态:" << m_initialized
             << "Mapper:" << m_mapper << "Actor:" << m_actor;

    // 检查前置条件
    if (!m_loader || !m_initialized) {
        qDebug() << "[模型加载] 加载器未就绪或VTK未初始化，退出处理";
        return;
    }

    qDebug() << "[模型加载] 开始清理现有可视化资源";
    // 清除现有actor
    if (m_actor) {
        qDebug() << "[模型加载] 移除现有Actor";
        m_renderer->RemoveActor(m_actor);
        m_actor->Delete();
        m_actor = nullptr;
        qDebug() << "[模型加载] 现有Actor已删除";
    }

    // 清除现有mapper
    if (m_mapper) {
        qDebug() << "[模型加载] 删除现有Mapper";
        m_mapper->Delete();
        m_mapper = nullptr;
        qDebug() << "[模型加载] 现有Mapper已删除";
    }

    // 检查模型数据有效性
    if( m_loader->getPolyData() == nullptr) {
        qDebug() << "[错误] 加载器的PolyData为空，无法继续处理";
        return;
    }

    // 获取模型数据
    vtkPolyData* polyData = m_loader->getPolyData();
    qDebug() << "[模型加载] 已获取PolyData指针:" << polyData;

    // 验证模型数据
    if (!polyData || polyData->GetNumberOfPoints() == 0) {
        qDebug() << "[错误] 模型数据无效或点数为零";
        return;
    }

    qDebug() << "[模型加载] 开始分析模型数据";
    // 详细检查数据
    qDebug() << "[模型数据] 顶点数量:" << polyData->GetNumberOfPoints();
    qDebug() << "[模型数据] 单元数量:" << polyData->GetNumberOfCells();
    qDebug() << "[模型数据] 多边形数量:" << polyData->GetNumberOfPolys();

    // 检查模型边界 - 重要：用于确定相机位置和缩放
    double bounds[6];
    polyData->GetBounds(bounds);
    qDebug() << "[模型边界] X轴范围: [" << bounds[0] << " 至 " << bounds[1] << "]";
    qDebug() << "[模型边界] Y轴范围: [" << bounds[2] << " 至 " << bounds[3] << "]";
    qDebug() << "[模型边界] Z轴范围: [" << bounds[4] << " 至 " << bounds[5] << "]";

    // 计算模型中心点
    double center[3] = {
        (bounds[0] + bounds[1]) / 2.0,
        (bounds[2] + bounds[3]) / 2.0,
        (bounds[4] + bounds[5]) / 2.0
    };
    qDebug() << "[几何计算] 模型中心点: (" << center[0] << ", " << center[1] << ", " << center[2] << ")";

    // 计算模型尺寸
    double size[3] = {
        bounds[1] - bounds[0],  // X轴尺寸
        bounds[3] - bounds[2],  // Y轴尺寸
        bounds[5] - bounds[4]   // Z轴尺寸
    };
    qDebug() << "[几何计算] 模型尺寸: X=" << size[0] << " Y=" << size[1] << " Z=" << size[2];

    // 保存几何信息到成员变量
    qDebug() << "[模型加载] 开始保存几何信息到成员变量";
    m_bounds[0] = bounds[0]; m_bounds[1] = bounds[1];
    m_bounds[2] = bounds[2]; m_bounds[3] = bounds[3];
    m_bounds[4] = bounds[4]; m_bounds[5] = bounds[5];

    m_center[0] = center[0]; m_center[1] = center[1]; m_center[2] = center[2];
    m_size[0] = size[0]; m_size[1] = size[1]; m_size[2] = size[2];
    qDebug() << "[模型加载] 几何信息保存完成";

    qDebug() << "[模型加载] 开始为每个面片设置随机颜色";

    // 为每个面片设置随机颜色
    setPredefinedColorsForFaces(polyData);

    qDebug() << "[模型加载] 开始创建可视化管线";
    // 创建mapper和actor
    m_mapper = vtkPolyDataMapper::New();
    m_mapper->SetInputData(polyData);
    m_mapper->SetScalarModeToUseCellData(); // 使用单元数据作为标量
    m_mapper->SetScalarVisibility(true);    // 启用标量颜色映射
    qDebug() << "[可视化] Mapper创建完成，已启用单元颜色映射";

    m_actor = vtkActor::New();
    m_actor->SetMapper(m_mapper);
    m_actor->SetVisibility(true);
    qDebug() << "[可视化] Actor创建完成";

    // 设置actor视觉属性
    qDebug() << "[可视化] 设置Actor材质属性";
    // 注意：这里不再设置整体颜色，因为使用面片颜色
    m_actor->GetProperty()->SetOpacity(0.8);          // 设置透明度
    m_actor->GetProperty()->SetSpecular(0.5);         // 设置镜面反射强度
    m_actor->GetProperty()->SetSpecularPower(30);     // 设置镜面反射指数
    m_actor->GetProperty()->SetInterpolationToFlat(); // 设置为平面着色，显示面片边界
    qDebug() << "[可视化] Actor材质属性设置完成";

    // 获取并记录actor的初始位置
    double currentPos[3];
    m_actor->GetPosition(currentPos);
    qDebug() << "[可视化] Actor初始位置: (" << currentPos[0] << ", "
             << currentPos[1] << ", " << currentPos[2] << ")";

    // 将actor添加到渲染器
    m_renderer->AddActor(m_actor);
    qDebug() << "[可视化] Actor已添加到渲染器";

    // 触发界面更新
    update();
    qDebug() << "[界面] 更新信号已发出";

    // 切换到正视图
    setFrontView();
    qDebug() << "[视图] 已切换到正视图";

    qDebug() << "[模型加载] 模型加载处理全部完成";
}

/**
 * @brief 为模型的每个面片设置随机颜色
 * @param polyData 模型数据
 */
void VTKItem::setRandomColorsForFaces(vtkPolyData* polyData)
{
    if (!polyData) {
        qDebug() << "[颜色设置] 模型数据为空，无法设置颜色";
        return;
    }

    // 获取面片数量
    vtkIdType numCells = polyData->GetNumberOfCells();
    qDebug() << "[颜色设置] 开始为" << numCells << "个面片设置随机颜色";

    if (numCells == 0) {
        qDebug() << "[颜色设置] 模型没有面片，无法设置颜色";
        return;
    }

    // 创建颜色数组 - 使用传统方式创建
    vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
    colors->SetName("FaceColors");
    colors->SetNumberOfComponents(3); // RGB
    colors->SetNumberOfTuples(numCells);

    // 设置随机种子
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // 为每个面片生成随机颜色
    for (vtkIdType i = 0; i < numCells; i++) {
        // 生成随机RGB颜色
        unsigned char r = static_cast<unsigned char>(std::rand() % 256);
        unsigned char g = static_cast<unsigned char>(std::rand() % 256);
        unsigned char b = static_cast<unsigned char>(std::rand() % 256);

        // 设置颜色
        colors->SetTuple3(i, r, g, b);

        // 每1000个面片输出一次进度
        if (i % 1000 == 0 && i > 0) {
            qDebug() << "[颜色设置] 已处理" << i << "/" << numCells << "个面片";
        }
    }

    qDebug() << "[颜色设置] 随机颜色生成完成";

    // 将颜色数组添加到面片数据中
    polyData->GetCellData()->SetScalars(colors);

    // 释放颜色数组（引用计数减1，VTK会管理剩余的生命周期）
    colors->Delete();

    qDebug() << "[颜色设置] 颜色数据已添加到模型";
}

/**
 * @brief 使用预定义颜色方案为面片着色（可选）
 * @param polyData 模型数据
 */
void VTKItem::setPredefinedColorsForFaces(vtkPolyData* polyData)
{
    if (!polyData) {
        return;
    }

    vtkIdType numCells = polyData->GetNumberOfCells();
    qDebug() << "[预定义颜色] 开始为" << numCells << "个面片设置预定义颜色";

    // 创建颜色数组 - 使用传统方式创建
    vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
    colors->SetName("FaceColors");
    colors->SetNumberOfComponents(3);
    colors->SetNumberOfTuples(numCells);

    // 预定义的颜色方案
    std::vector<std::vector<unsigned char>> colorSchemes = {
        {255, 0, 0},     // 红色
        {0, 255, 0},     // 绿色
        {0, 0, 255},     // 蓝色
        {255, 255, 0},   // 黄色
        {255, 0, 255},   // 洋红色
        {0, 255, 255},   // 青色
        {255, 165, 0},   // 橙色
        {128, 0, 128},   // 紫色
        {165, 42, 42},   // 棕色
        {0, 128, 0}      // 深绿色
    };

    // 为每个面片分配颜色
    for (vtkIdType i = 0; i < numCells; i++) {
        int colorIndex = i % static_cast<int>(colorSchemes.size()); // 修复类型转换警告
        colors->SetTuple3(i,
                          colorSchemes[colorIndex][0],
                          colorSchemes[colorIndex][1],
                          colorSchemes[colorIndex][2]);
    }

    // 将颜色数组添加到面片数据中
    polyData->GetCellData()->SetScalars(colors);

    // 释放颜色数组
    colors->Delete();

    qDebug() << "[预定义颜色] 预定义颜色设置完成";
}

/**
 * @brief 鼠标按下事件处理
 * @param event 鼠标事件对象
 */
void VTKItem::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "[鼠标事件] 鼠标按下事件，按钮:" << event->button()
             << "位置: (" << event->pos().x() << "," << event->pos().y() << ")";

    // 检查鼠标交互是否启用
    if (!m_mouseEnabled || !m_initialized) {
        qDebug() << "[鼠标事件] 鼠标交互未启用或VTK未初始化，忽略事件";
        event->ignore();
        return;
    }

    // 记录鼠标位置
    m_lastMousePos = event->pos();
    setFocus(true); // 获取焦点
    qDebug() << "[鼠标事件] 已记录鼠标位置并获取焦点";

    // 根据鼠标按钮类型处理
    if (event->button() == Qt::LeftButton) {
        handleLeftButtonPress(event);
    }

    event->accept(); // 接受事件处理
    qDebug() << "[鼠标事件] 鼠标按下事件处理完成";
}

/**
 * @brief 处理左键按下事件
 * @param event 鼠标事件对象
 */
void VTKItem::handleLeftButtonPress(QMouseEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "[鼠标事件] 左键按下 - 开始旋转模式";

    m_isRotating = true; // 设置旋转标志

    // 通知VTK交互器左键按下事件
    if (m_interactor) {
        m_interactor->SetEventInformationFlipY(m_lastMousePos.x(), m_lastMousePos.y());
        m_interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent);
        qDebug() << "[鼠标事件] 已通知VTK交互器左键按下事件";
    }
}

/**
 * @brief 鼠标释放事件处理
 * @param event 鼠标事件对象
 */
void VTKItem::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "[鼠标事件] 鼠标释放事件";

    if (!m_mouseEnabled) {
        qDebug() << "[鼠标事件] 鼠标交互未启用，忽略事件";
        event->ignore();
        return;
    }

    // 清除旋转标志
    m_isRotating = false;
    qDebug() << "[鼠标事件] 旋转模式已结束";

    event->accept(); // 接受事件处理
}

/**
 * @brief 设置VTK交互器
 *
 * 创建和配置VTK的鼠标交互功能
 */
void VTKItem::setupInteractor()
{
    qDebug() << "[交互器] 开始设置VTK交互器";

    if (!m_renderWindow || !m_initialized) {
        qDebug() << "[交互器] 渲染窗口未就绪，无法设置交互器";
        return;
    }

    // 创建交互器实例
    if (!m_interactor) {
        qDebug() << "[交互器] 创建通用渲染窗口交互器";
        m_interactor = vtkGenericRenderWindowInteractor::New();
        m_interactor->SetRenderWindow(m_renderWindow);
        m_interactor->Initialize();
        qDebug() << "[交互器] 交互器创建并初始化完成";
    }

    // 创建交互样式（轨迹球相机样式）
    if (!m_interactorStyle) {
        qDebug() << "[交互器] 创建轨迹球相机交互样式";
        m_interactorStyle = vtkInteractorStyleTrackballCamera::New();
        m_interactor->SetInteractorStyle(m_interactorStyle);
        qDebug() << "[交互器] 交互样式设置完成";
    }

    qDebug() << "[交互器] 设置交互器尺寸:" << width() << "x" << height();
    // 设置交互器尺寸
    m_interactor->SetSize(width(), height());

    qDebug() << "[交互器] VTK交互器设置全部完成";
}

/**
 * @brief 鼠标移动事件处理
 * @param event 鼠标事件对象
 */
void VTKItem::mouseMoveEvent(QMouseEvent *event)
{
    // 移除所有调试输出，减少性能开销
    if (!m_mouseEnabled || !m_initialized || !m_isRotating) {
        event->ignore();
        return;
    }

    // 关键修复：限制事件频率
    static QElapsedTimer moveTimer;
    if (moveTimer.isValid() && moveTimer.elapsed() < 16) { // 约60fps
        m_lastMousePos = event->pos();
        event->accept();
        return;
    }
    moveTimer.start();

    // 处理左键拖动
    if (event->buttons() & Qt::LeftButton) {
        handleLeftButtonMove(event);
    }

    m_lastMousePos = event->pos();
    event->accept();
}

/**
 * @brief 处理左键移动事件（旋转操作）
 * @param event 鼠标事件对象
 */
void VTKItem::handleLeftButtonMove(QMouseEvent *event)
{
    qDebug() << "[鼠标事件] 左键移动 - 执行旋转操作";

    if (m_interactor && m_isRotating) {
        // 设置事件信息（翻转Y坐标以适应不同坐标系）
        m_interactor->SetEventInformationFlipY(event->pos().x(), event->pos().y());

        // 触发鼠标移动事件
        m_interactor->InvokeEvent(vtkCommand::MouseMoveEvent);
        qDebug() << "[鼠标事件] 已通知VTK交互器鼠标移动事件";

        // 延迟触发界面更新
        QTimer::singleShot(0, this, [this]() {
            update();
            qDebug() << "[鼠标事件] 界面更新已触发";
        });
    }
}
