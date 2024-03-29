#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Databinding.h"
#include "SupportCanvas3D.h"
#include "CS123XmlSceneParser.h"
#include "scenegraph/RayScene.h"
#include "scenegraph/ShapesScene.h"
#include "scenegraph/SceneviewScene.h"
#include "scenegraph/PoolScene.h"
#include "camera/CamtransCamera.h"
#include "CS123XmlSceneParser.h"
#include <math.h>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    // Make sure the settings are loaded before the UI
    settings.loadSettingsOrDefaults();

    QGLFormat qglFormat;
    qglFormat.setVersion(4, 3);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    qglFormat.setSampleBuffers(true);
    ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(ui->canvas3D);
    m_canvas3D = new SupportCanvas3D(qglFormat, this);
    gridLayout->addWidget(m_canvas3D, 0, 1);
    ui->tabWidget->setCurrentWidget(ui->tab2D);
    ui->tabWidget->setCurrentWidget(ui->tab3D);

    // Restore the UI settings
    QSettings qtSettings("CS123", "CS123");
    restoreGeometry(qtSettings.value("geometry").toByteArray());
    restoreState(qtSettings.value("windowState").toByteArray());

    // Resize the window because the window is huge since all docks were visible.
    resize(1000, 600);

    // Bind the UI elements to their properties in the global Settings object, using binding
    // objects to simplify the code.  Each binding object connects to its UI elements and keeps
    // the UI and its setting in sync.

    QList<QAction *> actions;

#define SETUP_ACTION(dock, key)                      \
    actions.push_back(ui->dock->toggleViewAction()); \
    actions.back()->setShortcut(QKeySequence(key));

    SETUP_ACTION(shapesDock, "CTRL+3");
    SETUP_ACTION(camtransDock, "CTRL+4");
    SETUP_ACTION(rayDock, "CTRL+5");

    ui->menuToolbars->addActions(actions);
#undef SETUP_ACTION
    tabifyDockWidget(ui->shapesDock, ui->camtransDock);
    tabifyDockWidget(ui->shapesDock, ui->rayDock);
    ui->shapesDock->raise();

    dataBind();

    // Hide the "stop rendering" button until we need it
    ui->rayStopRenderingButton->setHidden(true);

    // Hide shoot/exit shoot mode button until we need it
//    ui->shootButton->setHidden(true);
    ui->setShootMode->setHidden(true);
    ui->exitShootMode->setHidden(true);

    // Reset the contents of both canvas widgets (make a new 500x500 image for the 2D one)
    fileNew();

    // Make certain radio buttons switch to the 2D canvas when clicked.
    QList<QRadioButton *> a;
    foreach (QRadioButton *rb, a)
        connect(rb, SIGNAL(clicked()), this, SLOT(activateCanvas2D()));

    a.clear();
//    a += ui->shapeTypeCone;
//    a += ui->shapeTypeCube;
//    a += ui->shapeTypeCylinder;
//    a += ui->shapeTypeTorus;
//    a += ui->shapeTypeSpecial1;
//    a += ui->shapeTypeSpecial2;
    foreach (QRadioButton *rb, a)
        connect(rb, SIGNAL(clicked()), this, SLOT(activateCanvas3D()));

    QWidget *widget = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->tab3D);
    show();
    ui->tabWidget->setCurrentWidget(widget);
    show();
}

MainWindow::~MainWindow()
{
    foreach (DataBinding *b, m_bindings)
        delete b;
    foreach (QButtonGroup *bg, m_buttonGroups)
        delete bg;
    delete ui;
}

void MainWindow::dataBind()
{
#define BIND(b)                                                                    \
    {                                                                              \
        DataBinding *_b = (b);                                                     \
        m_bindings.push_back(_b);                                                  \
        assert(connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged()))); \
    }
    QButtonGroup *shapesButtonGroup = new QButtonGroup;
    m_buttonGroups.push_back(shapesButtonGroup);

    // Shapes dock
    BIND(BoolBinding::bindCheckbox(ui->showSceneviewInstead, settings.useSceneviewScene))
//    BIND(ChoiceBinding::bindRadioButtons(
//        shapesButtonGroup,
//        NUM_SHAPE_TYPES,
//        settings.shapeType,
//        ui->shapeTypeCube,
//        ui->shapeTypeCone,
//        ui->shapeTypeSphere,
//        ui->shapeTypeCylinder,
//        ui->shapeTypeTorus,
//        ui->shapeTypeSpecial1,
//        ui->shapeTypeSpecial2))
//    BIND(IntBinding::bindSliderAndTextbox(
//        ui->shapeParameterSlider1, ui->shapeParameterTextbox1, settings.shapeParameter1, 1.f, 100.f))
//    BIND(IntBinding::bindSliderAndTextbox(
//        ui->shapeParameterSlider2, ui->shapeParameterTextbox2, settings.shapeParameter2, 1.f, 100.f))
//    BIND(FloatBinding::bindSliderAndTextbox(
//        ui->shapeParameterSlider3, ui->shapeParameterTextbox3, settings.shapeParameter3, 1.f, 100.f))
    BIND(BoolBinding::bindCheckbox(ui->useLightingCheckbox, settings.useLighting))
    BIND(BoolBinding::bindCheckbox(ui->drawWireframeCheckbox, settings.drawWireframe))
    BIND(BoolBinding::bindCheckbox(ui->drawNormalsCheckbox, settings.drawNormals))

    // Camtrans dock
    BIND(BoolBinding::bindCheckbox(ui->cameraOrbitCheckbox, settings.useOrbitCamera))
    BIND(FloatBinding::bindDial(ui->transX, settings.cameraPosX, -2, 2, true))
    BIND(FloatBinding::bindDial(ui->transY, settings.cameraPosY, -2, 2, true))
    BIND(FloatBinding::bindDial(ui->transZ, settings.cameraPosZ, -2, 2, true))
    BIND(FloatBinding::bindDial(ui->rotU, settings.cameraRotU, -20, 20, true))
    BIND(FloatBinding::bindDial(ui->rotV, settings.cameraRotV, -20, 20, true))
    BIND(FloatBinding::bindDial(ui->rotW, settings.cameraRotN, -180, 180, false))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->cameraFovSlider, ui->cameraFovTextbox, settings.cameraFov, 1, 179))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->cameraNearSlider, ui->cameraNearTextbox, settings.cameraNear, 0.1, 50))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->cameraFarSlider, ui->cameraFarTextbox, settings.cameraFar, 0.1, 50))

    // Ray dock
    BIND(BoolBinding::bindCheckbox(ui->raySuperSamping, settings.useSuperSampling))
    BIND(IntBinding::bindTextbox(ui->raySuperSamplesTextbox, settings.numSuperSamples))
    BIND(BoolBinding::bindCheckbox(ui->rayAntiAliasing, settings.useAntiAliasing))
    BIND(BoolBinding::bindCheckbox(ui->rayShadows, settings.useShadows))
    BIND(BoolBinding::bindCheckbox(ui->rayTextureMapping, settings.useTextureMapping))
    BIND(BoolBinding::bindCheckbox(ui->rayReflection, settings.useReflection))
    BIND(BoolBinding::bindCheckbox(ui->rayRefraction, settings.useRefraction))
    BIND(BoolBinding::bindCheckbox(ui->rayPointLights, settings.usePointLights))
    BIND(BoolBinding::bindCheckbox(ui->rayDirectionalLights, settings.useDirectionalLights))
    BIND(BoolBinding::bindCheckbox(ui->raySpotLights, settings.useSpotLights))
    BIND(BoolBinding::bindCheckbox(ui->rayMultiThreading, settings.useMultiThreading))
    BIND(BoolBinding::bindCheckbox(ui->rayUseKDTree, settings.useKDTree))

    BIND(ChoiceBinding::bindTabs(ui->tabWidget, settings.currentTab))

    // Pool dock
            BIND(FloatBinding::bindDial(ui->dir_dial, settings.angle, 0.f, 2*M_PI, true))
            BIND(FloatBinding::bindSliderAndTextbox(
                ui->vel_slide, ui->vel_box, settings.vel, 0.f, 5.f))
#undef BIND

    // make sure the aspect ratio updates when m_canvas3D changes size
    connect(m_canvas3D, SIGNAL(aspectRatioChanged()), this, SLOT(updateAspectRatio()));
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e); // allow the superclass to handle this for the most part...

    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save the settings before we quit
    settings.saveSettings();
    QSettings qtSettings("CS123", "CS123");
    qtSettings.setValue("geometry", saveGeometry());
    qtSettings.setValue("windowState", saveState());

    // Stop any raytracing, otherwise the thread will hang around until done
    ui->canvas2D->cancelRender();

    QMainWindow::closeEvent(event);
}

void MainWindow::updateAspectRatio()
{
    // Update the aspect ratio text so the aspect ratio will be correct even if the
    // 3D canvas isn't visible (the 3D canvas isn't resized when it isn't visible)
    QSize activeTabSize = ui->tabWidget->currentWidget()->size();
    float aspectRatio = static_cast<float>(activeTabSize.width()) / static_cast<float>(activeTabSize.height());
    ui->cameraAspectRatio->setText(QString("Aspect ratio: %1").arg(aspectRatio));
}

void MainWindow::settingsChanged()
{
    ui->canvas2D->settingsChanged();
    m_canvas3D->settingsChanged();
}

void MainWindow::setAllRayFeatures(bool checked)
{
    ui->raySuperSamping->setChecked(checked);
    ui->rayAntiAliasing->setChecked(checked);
    ui->rayShadows->setChecked(checked);
    ui->rayTextureMapping->setChecked(checked);
    ui->rayReflection->setChecked(checked);
    ui->rayRefraction->setChecked(checked);
    ui->rayMultiThreading->setChecked(checked);

    // We don't want to affect the light checkboxes...
    //ui->rayPointLights->setChecked(checked);
    //ui->rayDirectionalLights->setChecked(checked);
    //ui->raySpotLights->setChecked(checked);
}

void MainWindow::fileCopy3Dto2D()
{
    // Make sure OpenGL gets a chance to redraw
    ui->tabWidget->setCurrentIndex(TAB_3D);
    m_canvas3D->update();
    QApplication::processEvents();

    // Resize the 2D canvas to the size of the 3D canvas and copy the pixels over.
    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    ui->canvas2D->resize(m_canvas3D->width() * ratio, m_canvas3D->height() * ratio);
    m_canvas3D->copyPixels(ui->canvas2D->width(), ui->canvas2D->height(), ui->canvas2D->data());
    ui->tabWidget->setCurrentIndex(TAB_2D);
}

void MainWindow::fileNew()
{
    ui->canvas2D->newImage();
}

void MainWindow::fileOpen()
{
    // This opens the 3D tab to initialize OGL so parsing
    // the scene doesn't crash. If you can find a better solution
    // feel free to change this.
    activateCanvas3D();
    QString file = QFileDialog::getOpenFileName(this, QString(), "/course/cs123/data/");
    if (!file.isNull())
    {
        if (file.endsWith(".xml"))
        {
            CS123XmlSceneParser parser(file.toLatin1().data());
            if (parser.parse())
            {
                m_canvas3D->loadSceneviewSceneFromParser(parser);
                ui->showSceneviewInstead->setChecked(true);

                // Set the camera for the new scene
                CS123SceneCameraData camera;
                if (parser.getCameraData(camera))
                {
                    camera.pos[3] = 1;
                    camera.look[3] = 0;
                    camera.up[3] = 0;

                    CamtransCamera *cam = m_canvas3D->getCamtransCamera();
                    cam->orientLook(camera.pos, camera.look, camera.up);
                    cam->setHeightAngle(camera.heightAngle);
                }

                if (settings.useOrbitCamera)
                {
                    ui->cameraOrbitCheckbox->setChecked(false);
                }

                activateCanvas3D();
            }
            else
            {
                QMessageBox::critical(this, "Error", "Could not load scene \"" + file + "\"");
            }
        }
        else
        {
            if (!ui->canvas2D->loadImage(file))
            {
                QMessageBox::critical(this, "Error", "Could not load image \"" + file + "\"");
            }
            else
            {
                activateCanvas2D();
            }
        }
    }
}

void MainWindow::fileOpenPool()
{
    // This opens the 3D tab to initialize OGL so parsing
    // the scene doesn't crash. If you can find a better solution
    // feel free to change this.
    activateCanvas3D();
    //QString file = QFileDialog::getOpenFileName(this, QString(), "/course/cs123/data/");
    QString file = "/Users/justinzhang/Documents/Brown/Senior/CS1230/cs123-final/data/justin_pool.xml";
    if (!file.isNull())
    {
        if (file.endsWith(".xml"))
        {
            CS123XmlSceneParser parser(file.toLatin1().data());
            if (parser.parse())
            {
                m_canvas3D->loadSceneviewSceneFromParser(parser);
                ui->showSceneviewInstead->setChecked(true);

                // Set the camera for the new scene
                CS123SceneCameraData camera;
                if (parser.getCameraData(camera))
                {
                    camera.pos[3] = 1;
                    camera.look[3] = 0;
                    camera.up[3] = 0;

                    CamtransCamera *cam = m_canvas3D->getCamtransCamera();
                    cam->orientLook(camera.pos, camera.look, camera.up);
                    cam->setHeightAngle(camera.heightAngle);
                }

                if (settings.useOrbitCamera)
                {
                    ui->cameraOrbitCheckbox->setChecked(false);
                }

                activateCanvas3D();
            }
            else
            {
                QMessageBox::critical(this, "Error", "Could not load scene \"" + file + "\"");
            }
        }
        else
        {
            if (!ui->canvas2D->loadImage(file))
            {
                QMessageBox::critical(this, "Error", "Could not load image \"" + file + "\"");
            }
            else
            {
                activateCanvas2D();
            }
        }
    }
}

void MainWindow::fileSave()
{
    if (settings.currentTab == TAB_2D)
        ui->canvas2D->saveImage();
}

void MainWindow::checkAllRayFeatures()
{
    setAllRayFeatures(true);
}

void MainWindow::uncheckAllRayFeatures()
{
    setAllRayFeatures(false);
}

void MainWindow::renderImage()
{
    // Make sure OpenGL gets a chance to update the OrbitCamera, which can only be done when
    // that tab is active (because it needs the OpenGL context for its matrix transforms)
    ui->tabWidget->setCurrentIndex(TAB_3D);
    m_canvas3D->update();
    QApplication::processEvents();

    ui->tabWidget->setCurrentIndex(TAB_2D);

    OpenGLScene *glScene = m_canvas3D->getScene();
    if (glScene)
    {
        // TODO: Set up RayScene from glScene and call ui->canvas2D->setScene
        RayScene *rayscene = new RayScene(*glScene);
        ui->canvas2D->setScene(rayscene);

        // Disable the UI so the user can't interfere with the raytracing
        setAllEnabled(false);

        // Swap the "render" button for the "stop rendering" button
        ui->rayRenderButton->setHidden(true);
        ui->rayStopRenderingButton->setHidden(false);

        // Render the image
        QSize activeTabSize = ui->tabWidget->currentWidget()->size();
        ui->canvas2D->renderImage(m_canvas3D->getCamera(), activeTabSize.width(), activeTabSize.height());

        // Swap the "stop rendering" button for the "render" button
        ui->rayRenderButton->setHidden(false);
        ui->rayStopRenderingButton->setHidden(true);

        // Enable the UI again
        setAllEnabled(true);
    }
}

void MainWindow::setAllEnabled(bool enabled)
{
    QList<QWidget *> widgets;
    widgets += ui->shapesDock;
    widgets += ui->camtransDock;
    widgets += ui->rayAllOrNone;
    widgets += ui->rayFeatures;
    widgets += ui->rayLighting;
    widgets += ui->rayRenderButton;
    widgets += ui->setShootMode;
    widgets += ui->exitShootMode;
    widgets += ui->shootButton;

    QList<QAction *> actions;
    actions += ui->actionNew;
    actions += ui->actionOpen;
    actions += ui->actionSave;
    actions += ui->actionRevert;
    actions += ui->actionCopy3Dto2D;
    actions += ui->actionClear;
    actions += ui->actionQuit;

    foreach (QWidget *widget, widgets)
        widget->setEnabled(enabled);
    foreach (QAction *action, actions)
        action->setEnabled(enabled);
}

void MainWindow::activateCanvas2D()
{
    ui->tabWidget->setCurrentWidget(ui->tab2D);
}

void MainWindow::activateCanvas3D()
{
    ui->tabWidget->setCurrentWidget(ui->tab3D);
}

void MainWindow::clearImage()
{
    ui->canvas2D->clearImage();
}

void MainWindow::revertImage()
{
    ui->canvas2D->revertImage();
}

void MainWindow::setCameraAxisX()
{
    m_canvas3D->setCameraAxisX();
}

void MainWindow::setCameraAxisY()
{
    m_canvas3D->setCameraAxisY();
}

void MainWindow::setCameraAxisZ()
{
    m_canvas3D->setCameraAxisZ();
}

void MainWindow::updateCameraTranslation()
{
    m_canvas3D->updateCameraTranslation();
}

void MainWindow::updateCameraRotationN()
{
    m_canvas3D->updateCameraRotationN();
}

void MainWindow::updateCameraRotationV()
{
    m_canvas3D->updateCameraRotationV();
}

void MainWindow::updateCameraRotationU()
{
    m_canvas3D->updateCameraRotationU();
}

void MainWindow::resetUpVector()
{
    m_canvas3D->resetUpVector();
}

void MainWindow::resetSliders()
{
    ui->cameraFovTextbox->setText(QString::number(55, 'f', 1));
    ui->cameraNearTextbox->setText(QString::number(.1, 'f', 1));
    ui->cameraFarTextbox->setText(QString::number(50, 'f', 1));
    update();
}

void MainWindow::updateCameraClip()
{
    m_canvas3D->updateCameraClip();
}

void MainWindow::updateCameraHeightAngle()
{
    m_canvas3D->updateCameraHeightAngle();
}

void MainWindow::setCameraAxonometric()
{
    m_canvas3D->setCameraAxonometric();
}

void MainWindow::setShootMode()
{
    // Make sure OpenGL gets a chance to update the OrbitCamera, which can only be done when
    // that tab is active (because it needs the OpenGL context for its matrix transforms)
    ui->tabWidget->setCurrentIndex(TAB_3D);
    m_canvas3D->update();
    QApplication::processEvents();

    // Swap buttons
    ui->setShootMode->setHidden(true);
    ui->shootButton->setHidden(false);
    ui->exitShootMode->setHidden(false);

    // change camera settings
    m_canvas3D->orientCue();

    // change settings
    settings.useCue = true;
}
void MainWindow::shoot()
{
    m_canvas3D->shoot(settings.vel,settings.angle);
    //ui->shootButton->setHidden(true);
}
void MainWindow::exitShootMode()
{
    // Swap buttons
    ui->setShootMode->setHidden(false);
    ui->shootButton->setHidden(true);
    ui->exitShootMode->setHidden(true);

    // change camera settings
    m_canvas3D->resetCamera();

    // change settings
    settings.useCue = false;
}

void MainWindow::resetPoolScene(){
    m_canvas3D->resetPoolScene();
}
