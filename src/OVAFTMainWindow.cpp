#include "OVAFTMainWindow.h"

#include <QAction>
#include <QApplication>
#include <QColorDialog>
#include <QFileDialog>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>
#include "OVAFTFaceTracker.h"
#include "OVAFTGLWidget.h"
#include "OVAFTLive2DManager.h"
#include "OVAFTUtility.h"
#include "OVAFTView.h"

OVAFTMainWindow::OVAFTMainWindow() {
    // Check Requirement(s)
    CheckRequirements();

    // Minimum Window Size
    setMinimumWidth(720);
    setMinimumHeight(480);

    InitializeLayout();
    InitializeMenu();
}

void OVAFTMainWindow::aboutQtEvent() {
    QApplication::aboutQt();
}

void OVAFTMainWindow::angleXEvent() {
    OVAFTFaceTracker::GetInstance()->setAngleXEnabled(angleXAction->isChecked());
}

void OVAFTMainWindow::angleYEvent() {
    OVAFTFaceTracker::GetInstance()->setAngleYEnabled(angleYAction->isChecked());
}

void OVAFTMainWindow::angleZEvent() {
    OVAFTFaceTracker::GetInstance()->setAngleZEnabled(angleZAction->isChecked());
}

void OVAFTMainWindow::backgroundColorEvent() {
    QColorDialog colorDialog(this);
    colorDialog.setWindowTitle(QString("Background Color"));
    colorDialog.setCurrentColor(OVAFTGLWidget::GetInstance()->GetBackgroundColor());
    if (colorDialog.exec() != QColorDialog::Accepted) return;
    OVAFTGLWidget::GetInstance()->SetBackgroundColor(colorDialog.selectedColor());
}

void OVAFTMainWindow::bodyAngleXEvent() {
    OVAFTFaceTracker::GetInstance()->setBodyAngleXEnabled(bodyAngleXAction->isChecked());
}

void OVAFTMainWindow::bodyAngleYEvent() {
    OVAFTFaceTracker::GetInstance()->setBodyAngleYEnabled(bodyAngleYAction->isChecked());
}

void OVAFTMainWindow::bodyAngleZEvent() {
    OVAFTFaceTracker::GetInstance()->setBodyAngleZEnabled(bodyAngleZAction->isChecked());
}

void OVAFTMainWindow::bottomEvent() {
    OVAFTGLWidget::GetInstance()->GetView()->ToBottom();
}

void OVAFTMainWindow::changeCameraEvent() {
    OVAFTFaceTracker::GetInstance()->ChangeCamera();
}

void OVAFTMainWindow::CheckRequirements() {
    // Landmark
    std::filesystem::path landmarkPath(OVAFTUtility::dirname(OVAFTUtility::getExecutablePath()));
    landmarkPath.append("resources");
    landmarkPath.append("landmark");
    landmarkPath.append("shape_predictor_68_face_landmarks.dat");
    std::ifstream checkLandmarkFile(landmarkPath.string());
    if (!checkLandmarkFile.is_open()) {
        QMessageBox landmarkMessage;
        landmarkMessage.setWindowTitle("Error");
        landmarkMessage.setText("Cannot find Landmark file!");
        landmarkMessage.setIcon(QMessageBox::Critical);
        landmarkMessage.addButton(QMessageBox::Ok);
        landmarkMessage.setDefaultButton(QMessageBox::Ok);
        landmarkMessage.exec();
        QApplication::quit();
    }
}

void OVAFTMainWindow::cheekEvent() {
    OVAFTFaceTracker::GetInstance()->setCheekEnabled(cheekAction->isChecked());
}

void OVAFTMainWindow::eyeLOpenEvent() {
    OVAFTFaceTracker::GetInstance()->setEyeLOpenEnabled(eyeLOpenAction->isChecked());
}

void OVAFTMainWindow::eyeLSmileEvent() {
    OVAFTFaceTracker::GetInstance()->setEyeLSmileEnabled(eyeLSmileAction->isChecked());
}


void OVAFTMainWindow::eyeROpenEvent() {
    OVAFTFaceTracker::GetInstance()->setEyeROpenEnabled(eyeROpenAction->isChecked());
}

void OVAFTMainWindow::eyeRSmileEvent() {
    OVAFTFaceTracker::GetInstance()->setEyeRSmileEnabled(eyeRSmileAction->isChecked());
}


void OVAFTMainWindow::fullScreenEvent() {
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void OVAFTMainWindow::quitEvent() {
    QApplication::quit();
}

void OVAFTMainWindow::InitializeLayout() {
    auto *layout(new QVBoxLayout);
    auto *glWidget(OVAFTGLWidget::GetInstance());
    layout->addWidget(glWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);
}

void OVAFTMainWindow::InitializeMenu() {
    auto *fileMenu(new QMenu("&File", this));
    auto *editMenu(new QMenu("&Edit", this));
    auto *viewMenu(new QMenu("&View", this));
    auto *helpMenu(new QMenu("&Help", this));
    auto *openModelAction(new QAction("&Open Model", this));
    openModelAction->setShortcut(QKeySequence::Open);
    auto *quitAction(new QAction("&Quit", this));
    quitAction->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(openModelAction);
    fileMenu->insertSeparator(new QAction);
    fileMenu->addAction(quitAction);
    auto *changeCameraAction(new QAction("&Change Camera", this));
    auto *trackingMenu(new QMenu("&Tracking", this));
    angleXAction = new QAction("AngleX", this);
    angleXAction->setCheckable(true);
    angleXAction->setChecked(true);
    angleYAction = new QAction("AngleY", this);
    angleYAction->setCheckable(true);
    angleYAction->setChecked(true);
    angleZAction = new QAction("AngleZ", this);
    angleZAction->setCheckable(true);
    angleZAction->setChecked(true);
    bodyAngleXAction = new QAction("BodyAngleX", this);
    bodyAngleXAction->setCheckable(true);
    bodyAngleXAction->setChecked(true);
    bodyAngleYAction = new QAction("BodyAngleY", this);
    bodyAngleYAction->setCheckable(true);
    bodyAngleYAction->setChecked(true);
    bodyAngleZAction = new QAction("BodyAngleZ", this);
    bodyAngleZAction->setCheckable(true);
    bodyAngleZAction->setChecked(true);
    cheekAction = new QAction("Cheek", this);
    cheekAction->setCheckable(true);
    cheekAction->setChecked(true);
    eyeLOpenAction = new QAction("EyeLOpen", this);
    eyeLOpenAction->setCheckable(true);
    eyeLOpenAction->setChecked(true);
    eyeLSmileAction = new QAction("EyeLSmile", this);
    eyeLSmileAction->setCheckable(true);
    eyeLSmileAction->setChecked(true);
    eyeROpenAction = new QAction("EyeROpen", this);
    eyeROpenAction->setCheckable(true);
    eyeROpenAction->setChecked(true);
    eyeRSmileAction = new QAction("EyeRSmile", this);
    eyeRSmileAction->setCheckable(true);
    eyeRSmileAction->setChecked(true);
    mouthFormAction = new QAction("MouthForm", this);
    mouthFormAction->setCheckable(true);
    mouthFormAction->setChecked(true);
    mouthOpenYAction = new QAction("MouthOpenY", this);
    mouthOpenYAction->setCheckable(true);
    mouthOpenYAction->setChecked(true);
    trackingMenu->addAction(angleXAction);
    trackingMenu->addAction(angleYAction);
    trackingMenu->addAction(angleZAction);
    trackingMenu->addAction(bodyAngleXAction);
    trackingMenu->addAction(bodyAngleYAction);
    trackingMenu->addAction(bodyAngleZAction);
    trackingMenu->addAction(cheekAction);
    trackingMenu->addAction(eyeLOpenAction);
    trackingMenu->addAction(eyeLSmileAction);
    trackingMenu->addAction(eyeROpenAction);
    trackingMenu->addAction(eyeRSmileAction);
    trackingMenu->addAction(mouthFormAction);
    trackingMenu->addAction(mouthOpenYAction);
    auto *backgroundColorAction(new QAction("&Background Color", this));
    auto *resetAngleAction(new QAction("&Reset Angle", this));
    editMenu->addAction(changeCameraAction);
    editMenu->addMenu(trackingMenu);
    editMenu->addAction(backgroundColorAction);
    editMenu->addAction(resetAngleAction);
    auto *zoomInAction(new QAction("ZoomIn", this));
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    auto *zoomOutAction(new QAction("ZoomOut", this));
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    auto *resetScaleAction(new QAction("Reset Scale", this));
    auto *leftAction(new QAction("Left", this));
    leftAction->setShortcut(Qt::Key_Left);
    auto *rightAction(new QAction("Right", this));
    rightAction->setShortcut(Qt::Key_Right);
    auto *topAction(new QAction("Top", this));
    topAction->setShortcut(Qt::Key_Up);
    auto *bottomAction(new QAction("Bottom", this));
    bottomAction->setShortcut(Qt::Key_Down);
    auto *resetPositionAction(new QAction("Reset Position", this));
    auto *fullScreenAction(new QAction("Full Screen", this));
    fullScreenAction->setShortcut(QKeySequence::FullScreen);
    auto *maximizeAction(new QAction("Maximize", this));
    auto *minimizeAction(new QAction("Minimize", this));
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);
    viewMenu->addAction(resetScaleAction);
    viewMenu->insertSeparator(new QAction);
    viewMenu->addAction(leftAction);
    viewMenu->addAction(rightAction);
    viewMenu->addAction(topAction);
    viewMenu->addAction(bottomAction);
    viewMenu->addAction(resetPositionAction);
    viewMenu->insertSeparator(new QAction);
    viewMenu->addAction(fullScreenAction);
    viewMenu->addAction(maximizeAction);
    viewMenu->addAction(minimizeAction);
    auto *aboutQtAction(new QAction("&About Qt", this));
    helpMenu->addAction(aboutQtAction);
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
    connect(openModelAction, SIGNAL(triggered()), this, SLOT(openModelEvent()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quitEvent()));
    connect(changeCameraAction, SIGNAL(triggered()), this, SLOT(changeCameraEvent()));
    connect(angleXAction, SIGNAL(triggered()), this, SLOT(angleXEvent()));
    connect(angleYAction, SIGNAL(triggered()), this, SLOT(angleYEvent()));
    connect(angleZAction, SIGNAL(triggered()), this, SLOT(angleZEvent()));
    connect(bodyAngleXAction, SIGNAL(triggered()), this, SLOT(bodyAngleXEvent()));
    connect(bodyAngleYAction, SIGNAL(triggered()), this, SLOT(bodyAngleYEvent()));
    connect(bodyAngleZAction, SIGNAL(triggered()), this, SLOT(bodyAngleZEvent()));
    connect(cheekAction, SIGNAL(triggered()), this, SLOT(cheekEvent()));
    connect(eyeLOpenAction, SIGNAL(triggered()), this, SLOT(eyeLOpenEvent()));
    connect(eyeLSmileAction, SIGNAL(triggered()), this, SLOT(eyeLSmileEvent()));
    connect(eyeROpenAction, SIGNAL(triggered()), this, SLOT(eyeROpenEvent()));
    connect(eyeRSmileAction, SIGNAL(triggered()), this, SLOT(eyeRSmileEvent()));
    connect(mouthFormAction, SIGNAL(triggered()), this, SLOT(mouthFormEvent()));
    connect(mouthOpenYAction, SIGNAL(triggered()), this, SLOT(mouthOpenYEvent()));
    connect(backgroundColorAction, SIGNAL(triggered()), this, SLOT(backgroundColorEvent()));
    connect(resetAngleAction, SIGNAL(triggered()), this, SLOT(resetAngleEvent()));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomInEvent()));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOutEvent()));
    connect(resetScaleAction, SIGNAL(triggered()), this, SLOT(resetScaleEvent()));
    connect(leftAction, SIGNAL(triggered()), this, SLOT(leftEvent()));
    connect(rightAction, SIGNAL(triggered()), this, SLOT(rightEvent()));
    connect(topAction, SIGNAL(triggered()), this, SLOT(topEvent()));
    connect(bottomAction, SIGNAL(triggered()), this, SLOT(bottomEvent()));
    connect(resetPositionAction, SIGNAL(triggered()), this, SLOT(resetPositionEvent()));
    connect(fullScreenAction, SIGNAL(triggered()), this, SLOT(fullScreenEvent()));
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(maximizeEvent()));
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(minimizeEvent()));
    connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQtEvent()));
}

void OVAFTMainWindow::leftEvent() {
    OVAFTGLWidget::GetInstance()->GetView()->ToLeft();
}

void OVAFTMainWindow::maximizeEvent() {
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void OVAFTMainWindow::minimizeEvent() {
    showMinimized();
}

void OVAFTMainWindow::mouthFormEvent() {
    OVAFTFaceTracker::GetInstance()->setMouthFormEnabled(mouthFormAction->isChecked());
}

void OVAFTMainWindow::mouthOpenYEvent() {
    OVAFTFaceTracker::GetInstance()->setMouthOpenYEnabled(mouthOpenYAction->isChecked());
}

void OVAFTMainWindow::openModelEvent() {
    QString moc3Path = QFileDialog::getOpenFileName(this, "Open Model", nullptr, ".moc3 file(*.moc3)");
    OVAFTLive2DManager::GetInstance()->LoadScene(moc3Path.toStdString());
}

void OVAFTMainWindow::resetAngleEvent() {
    OVAFTFaceTracker::GetInstance()->ResetAngle();
}

void OVAFTMainWindow::resetPositionEvent() {
    OVAFTGLWidget::GetInstance()->GetView()->ResetPosition();
}

void OVAFTMainWindow::resetScaleEvent() {
    OVAFTGLWidget::GetInstance()->GetView()->ResetScale();
}

void OVAFTMainWindow::rightEvent() {
    OVAFTGLWidget::GetInstance()->GetView()->ToRight();
}

void OVAFTMainWindow::topEvent() {
    OVAFTGLWidget::GetInstance()->GetView()->ToTop();
}

void OVAFTMainWindow::zoomInEvent() {
    OVAFTGLWidget::GetInstance()->GetView()->ZoomIn();
}

void OVAFTMainWindow::zoomOutEvent() {
    OVAFTGLWidget::GetInstance()->GetView()->ZoomOut();
}
