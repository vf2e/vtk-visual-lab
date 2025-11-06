
# QT += quick core gui opengl
# CONFIG += c++11 opengl

# # You can make your code fail to compile if it uses deprecated APIs.
# # In order to do so, uncomment the following line.
# #DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# # 确保中文显示正常
# QMAKE_CXXFLAGS += /utf-8
# QMAKE_PROJECT_DEPTH = 0

# 基本配置
QT += quick core gui opengl
CONFIG += c++11 opengl
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 定义一个 CONFIG 选项，默认不启用
# CONFIG += arch_64bit

# 确保中文显示正常
DEFINES += UNICODE _UNICODE
QMAKE_CXXFLAGS += /utf-8
QMAKE_PROJECT_DEPTH = 0
QT_OPENGL = desktop

# CONFIG += arch_64bit

LIBS += -lopengl32 \        # OpenGL 库（包含 wgl 函数）
        -lglu32 \           # GLU 库
        -lgdi32 \           # 图形设备接口
        -luser32 \          # 用户界面 API
        -ladvapi32 \        # 高级 API（注册表等）
        -lshell32 \         # Shell API
        -lws2_32 \          # Windows Socket API
        -lcomdlg32 \        # 通用对话框
        -lcomctl32 \        # 通用控件
        -lole32 \           # OLE API
        -loleaut32 \        # OLE 自动化
        -luuid              # UUID 生成

#####加载vtk在次项目中需要用到的库  按需导入就行

INCLUDEPATH += $$PWD/3dLib/openctm
LIBS += -L$$PWD/3dLib/openctm -lopenctm

# INCLUDEPATH += "D:/project/lib/lib"
# LIBS += -L"D:/project/lib/lib" -lopenctm

VTK_DIR = $$PWD/3dLib/VTK64

VTK_VERSION = 8.0

INCLUDEPATH += $$VTK_DIR/include/vtk-$$VTK_VERSION

LIBS += -L$$VTK_DIR/lib

# # 核心必需模块（CTM加载最小集）
# LIBS += -lvtkCommonCore-$$VTK_VERSION           # 核心功能
# LIBS += -lvtkCommonDataModel-$$VTK_VERSION      # 数据模型（网格、点等）
# LIBS += -lvtkIOCore-$$VTK_VERSION               # IO核心
# LIBS += -lvtkIOGeometry-$$VTK_VERSION           # 几何文件IO（可能支持CTM）

# # 渲染显示模块（如果需要显示）
# LIBS += -lvtkRenderingCore-$$VTK_VERSION        # 渲染核心
# # LIBS += -lvtkRenderingOpenGL2-$$VTK_VERSION     # OpenGL渲染
# LIBS += -lvtkInteractionStyle-$$VTK_VERSION     # 交互支持

# LIBS += -lvtkCommonExecutionModel-$$VTK_VERSION

# LIBS += -lvtksys-$$VTK_VERSION
# # VTK库链接（根据实际安装的模块调整）
# LIBS += -lvtkCommonCore-$$VTK_VERSION
# LIBS += -lvtkCommonMath-$$VTK_VERSION
# LIBS += -lvtkCommonMisc-$$VTK_VERSION
# LIBS += -lvtkCommonExecutionModel-$$VTK_VERSION
# LIBS += -lvtkCommonSystem-$$VTK_VERSION
# LIBS += -lvtkCommonTransforms-$$VTK_VERSION
# LIBS += -lvtkCommonDataModel-$$VTK_VERSION
# LIBS += -lvtkFiltersSources-$$VTK_VERSION
# LIBS += -lvtkFiltersCore-$$VTK_VERSION
# LIBS += -lvtkFiltersExtraction-$$VTK_VERSION    # 添加 vtkFiltersExtraction（包含 vtkExtractSelectedFrustum）
# LIBS += -lvtkFiltersSources-$$VTK_VERSION
# LIBS += -lvtkChartsCore-$$VTK_VERSION           # 添加 vtkChartsCore（包含 vtkColorSeries）
# LIBS += -lvtkInteractionStyle-$$VTK_VERSION
# LIBS += -lvtkRenderingCore-$$VTK_VERSION
# LIBS += -lvtkRenderingImage-$$VTK_VERSION
# LIBS += -lvtkRenderingLabel-$$VTK_VERSION
# # LIBS += -lvtkRenderingOpenGL2-$$VTK_VERSION
# LIBS += -lvtkRenderingOpenGL-$$VTK_VERSION
# LIBS += -lvtkInteractionStyle-$$VTK_VERSION
# LIBS += -lvtkRenderingFreeType-$$VTK_VERSION
# LIBS += -lvtkIOLegacy-$$VTK_VERSION
# LIBS += -lvtkIOGeometry-$$VTK_VERSION
# # LIBS += -lvtkGUISupportQt-$$VTK_VERSION

# 核心基础模块
LIBS += -lvtksys-$$VTK_VERSION
LIBS += -lvtkCommonCore-$$VTK_VERSION
LIBS += -lvtkCommonMath-$$VTK_VERSION
LIBS += -lvtkCommonMisc-$$VTK_VERSION
LIBS += -lvtkCommonSystem-$$VTK_VERSION
LIBS += -lvtkCommonTransforms-$$VTK_VERSION
LIBS += -lvtkCommonDataModel-$$VTK_VERSION
LIBS += -lvtkCommonExecutionModel-$$VTK_VERSION

# 过滤器模块
LIBS += -lvtkFiltersCore-$$VTK_VERSION
LIBS += -lvtkFiltersExtraction-$$VTK_VERSION
LIBS += -lvtkFiltersGeneral-$$VTK_VERSION
LIBS += -lvtkFiltersGeometry-$$VTK_VERSION
LIBS += -lvtkFiltersSources-$$VTK_VERSION

# exists($${VTK_DIR}/lib/vtkChartsCore-8.0.lib) {
#     message("vtkChartsCore library found")
#     LIBS += -lvtkChartsCore-8.0
# } else {
#     error("vtkChartsCore library not found at $${VTK_DIR}/lib/vtkChartsCore-8.0.lib")
# }

# 可视化模块
LIBS += -lvtkChartsCore-$$VTK_VERSION

# 渲染模块
# LIBS += -lvtkRenderingCore-$$VTK_VERSION
# # LIBS += -lvtkRenderingOpenGL-$$VTK_VERSION
# LIBS += -lvtkRenderingOpenGL2-$$VTK_VERSION
# LIBS += -lvtkInteractionStyle-$$VTK_VERSION


# LIBS += -lvtkalglib-$$VTK_VERSION
LIBS += -lvtkChartsCore-$$VTK_VERSION
LIBS += -lvtkCommonCore-$$VTK_VERSION
LIBS += -lvtkCommonColor-$$VTK_VERSION
LIBS += -lvtkCommonComputationalGeometry-$$VTK_VERSION
LIBS += -lvtkCommonDataModel-$$VTK_VERSION
LIBS += -lvtkCommonExecutionModel-$$VTK_VERSION
LIBS += -lvtkCommonMath-$$VTK_VERSION
LIBS += -lvtkCommonMisc-$$VTK_VERSION
LIBS += -lvtkCommonSystem-$$VTK_VERSION
LIBS += -lvtkCommonTransforms-$$VTK_VERSION
LIBS += -lvtkDICOMParser-$$VTK_VERSION
LIBS += -lvtkDomainsChemistry-$$VTK_VERSION
# LIBS += -lvtkexoIIc-$$VTK_VERSION
LIBS += -lvtkexpat-$$VTK_VERSION
LIBS += -lvtkFiltersAMR-$$VTK_VERSION
LIBS += -lvtkFiltersCore-$$VTK_VERSION
LIBS += -lvtkFiltersExtraction-$$VTK_VERSION
LIBS += -lvtkFiltersFlowPaths-$$VTK_VERSION
LIBS += -lvtkFiltersGeneral-$$VTK_VERSION
LIBS += -lvtkFiltersGeneric-$$VTK_VERSION
LIBS += -lvtkFiltersGeometry-$$VTK_VERSION
LIBS += -lvtkFiltersHybrid-$$VTK_VERSION
LIBS += -lvtkFiltersHyperTree-$$VTK_VERSION
LIBS += -lvtkFiltersImaging-$$VTK_VERSION
LIBS += -lvtkFiltersModeling-$$VTK_VERSION
LIBS += -lvtkFiltersParallel-$$VTK_VERSION
LIBS += -lvtkFiltersParallelImaging-$$VTK_VERSION
LIBS += -lvtkFiltersProgrammable-$$VTK_VERSION
LIBS += -lvtkFiltersSelection-$$VTK_VERSION
LIBS += -lvtkFiltersSMP-$$VTK_VERSION
LIBS += -lvtkFiltersSources-$$VTK_VERSION
LIBS += -lvtkFiltersStatistics-$$VTK_VERSION
LIBS += -lvtkFiltersTexture-$$VTK_VERSION
LIBS += -lvtkFiltersVerdict-$$VTK_VERSION
LIBS += -lvtkfreetype-$$VTK_VERSION
LIBS += -lvtkGeovisCore-$$VTK_VERSION
LIBS += -lvtkhdf5-$$VTK_VERSION
LIBS += -lvtkhdf5_hl-$$VTK_VERSION
LIBS += -lvtkImagingColor-$$VTK_VERSION
LIBS += -lvtkImagingCore-$$VTK_VERSION
LIBS += -lvtkImagingFourier-$$VTK_VERSION
LIBS += -lvtkImagingGeneral-$$VTK_VERSION
LIBS += -lvtkImagingHybrid-$$VTK_VERSION
LIBS += -lvtkImagingMath-$$VTK_VERSION
LIBS += -lvtkImagingMorphological-$$VTK_VERSION
LIBS += -lvtkImagingSources-$$VTK_VERSION
LIBS += -lvtkImagingStatistics-$$VTK_VERSION
LIBS += -lvtkImagingStencil-$$VTK_VERSION
LIBS += -lvtkInfovisCore-$$VTK_VERSION
LIBS += -lvtkInfovisLayout-$$VTK_VERSION
LIBS += -lvtkInteractionImage-$$VTK_VERSION
LIBS += -lvtkInteractionStyle-$$VTK_VERSION
LIBS += -lvtkInteractionWidgets-$$VTK_VERSION
LIBS += -lvtkIOAMR-$$VTK_VERSION
LIBS += -lvtkIOCore-$$VTK_VERSION
LIBS += -lvtkIOEnSight-$$VTK_VERSION
LIBS += -lvtkIOExodus-$$VTK_VERSION
LIBS += -lvtkIOExport-$$VTK_VERSION
LIBS += -lvtkIOGeometry-$$VTK_VERSION
LIBS += -lvtkIOImage-$$VTK_VERSION
LIBS += -lvtkIOImport-$$VTK_VERSION
LIBS += -lvtkIOInfovis-$$VTK_VERSION
LIBS += -lvtkIOLegacy-$$VTK_VERSION
LIBS += -lvtkIOLSDyna-$$VTK_VERSION
LIBS += -lvtkIOMINC-$$VTK_VERSION
LIBS += -lvtkIOMovie-$$VTK_VERSION
LIBS += -lvtkIONetCDF-$$VTK_VERSION
LIBS += -lvtkIOParallel-$$VTK_VERSION
LIBS += -lvtkIOParallelXML-$$VTK_VERSION
LIBS += -lvtkIOPLY-$$VTK_VERSION
LIBS += -lvtkIOSQL-$$VTK_VERSION
LIBS += -lvtkIOVideo-$$VTK_VERSION
LIBS += -lvtkIOXML-$$VTK_VERSION
LIBS += -lvtkIOXMLParser-$$VTK_VERSION
LIBS += -lvtkjpeg-$$VTK_VERSION
LIBS += -lvtkjsoncpp-$$VTK_VERSION
LIBS += -lvtklibxml2-$$VTK_VERSION
LIBS += -lvtkmetaio-$$VTK_VERSION
LIBS += -lvtkNetCDF-$$VTK_VERSION
# LIBS += -lvtkoggtheora-$$VTK_VERSION
LIBS += -lvtkParallelCore-$$VTK_VERSION
LIBS += -lvtkpng-$$VTK_VERSION
# LIBS += -lvtkproj4-$$VTK_VERSION
LIBS += -lvtkRenderingAnnotation-$$VTK_VERSION
LIBS += -lvtkRenderingContext2D-$$VTK_VERSION
LIBS += -lvtkRenderingContextOpenGL-$$VTK_VERSION
# LIBS += -lvtkRenderingContextOpenGL2-$$VTK_VERSION
LIBS += -lvtkRenderingCore-$$VTK_VERSION
LIBS += -lvtkRenderingFreeType-$$VTK_VERSION
LIBS += -lvtkRenderingImage-$$VTK_VERSION
LIBS += -lvtkRenderingLabel-$$VTK_VERSION
LIBS += -lvtkRenderingLOD-$$VTK_VERSION
LIBS += -lvtkRenderingOpenGL-$$VTK_VERSION
# LIBS += -lvtkRenderingOpenGL2-$$VTK_VERSION
LIBS += -lvtkRenderingVolume-$$VTK_VERSION
LIBS += -lvtkRenderingVolumeOpenGL-$$VTK_VERSION
# LIBS += -lvtkRenderingVolumeOpenGL2-$$VTK_VERSION
LIBS += -lvtksqlite-$$VTK_VERSION
LIBS += -lvtksys-$$VTK_VERSION
LIBS += -lvtktiff-$$VTK_VERSION
LIBS += -lvtkverdict-$$VTK_VERSION
LIBS += -lvtkViewsContext2D-$$VTK_VERSION
LIBS += -lvtkViewsCore-$$VTK_VERSION
LIBS += -lvtkViewsInfovis-$$VTK_VERSION
LIBS += -lvtkzlib-$$VTK_VERSION

# exists($$VTK_DIR/include/vtk-$$VTK_VERSION/vtkCTMReader.h) {
#     message("✓ VTK支持CTM格式")
#     LIBS += -lvtkIOCTM-$$VTK_VERSION
# } else {
#     message("✗ VTK不支持CTM格式，需要额外处理")
# }

# Qt集成（如果在Qt中显示）
# LIBS += -lvtkGUISupportQt-$$VTK_VERSION

#####加载vtk在次项目中需要用到的库  按需导入就行

exists($$VTK_DIR/include/vtk-$$VTK_VERSION/vtkPolyData.h) {
    message("vtkPolyData.h success")
} else {
    error("loaded vtkPolyData.h Fial")
}

exists($$VTK_DIR/include/vtk-$$VTK_VERSION/vtkAbstractPolyDataReader.h) {
    message("vtkAbstractPolyDataReader.h success")
} else {
    error("vtkAbstractPolyDataReader.h Fial")
}

# ================== VTK 模块 ================== #

HEADERS += \
    vtkitem.h \
    vtkctmloader.h

SOURCES += \
        main.cpp \
        vtkitem.cpp \
        vtkctmloader.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
