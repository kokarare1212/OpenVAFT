#pragma once

#include <QAction>
#include <QShowEvent>
#include <QMainWindow>

class OVAFTMainWindow : public QMainWindow {
Q_OBJECT
public:
    OVAFTMainWindow();

public Q_SLOTS:

    void aboutQtEvent();

    void angleXEvent();

    void angleYEvent();

    void angleZEvent();

    void bodyAngleXEvent();

    void bodyAngleYEvent();

    void bodyAngleZEvent();

    void bottomEvent();

    void cheekEvent();

    void backgroundColorEvent();

    void changeCameraEvent();

    void eyeLOpenEvent();

    void eyeLSmileEvent();

    void eyeROpenEvent();

    void eyeRSmileEvent();

    void fullScreenEvent();

    void leftEvent();

    void maximizeEvent();

    void mouthFormEvent();

    void mouthOpenYEvent();

    void minimizeEvent();

    void openModelEvent();

    void quitEvent();

    void resetAngleEvent();

    void resetPositionEvent();

    void resetScaleEvent();

    void rightEvent();

    void topEvent();

    void zoomInEvent();

    void zoomOutEvent();

private:
    void CheckRequirements();

    void InitializeLayout();

    void InitializeMenu();

    QAction *angleXAction;
    QAction *angleYAction;
    QAction *angleZAction;
    QAction *bodyAngleXAction;
    QAction *bodyAngleYAction;
    QAction *bodyAngleZAction;
    QAction *cheekAction;
    QAction *eyeLOpenAction;
    QAction *eyeLSmileAction;
    QAction *eyeROpenAction;
    QAction *eyeRSmileAction;
    bool firstEvent = true;
    QAction *mouthFormAction;
    QAction *mouthOpenYAction;
};
