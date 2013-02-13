/**
 * @file samui.h
 * Interface for SAM GUI
 * @author Michelle Daniels
 * @copyright UCSD 2012
 */

#ifndef SAMUI_H
#define SAMUI_H

#include <QMainWindow>

#include "sam.h"

namespace Ui {
class SamUI;
}

/**
 * @class SamUI
 * SamUI is the main window of a GUI for SAM
 */
class SamUI : public QMainWindow
{
    Q_OBJECT
    
public:
    /**
     * SamUI constructor.
     * @param params SAM parameters
     * @param parent parent Qt widget
     */
    explicit SamUI(const SamParams& params, QWidget *parent = 0);

    /**
     * SamUI destructor.
     */
    ~SamUI();

public slots:
    /**
     * Perform necessary cleanup before app closes.
     */
    void doBeforeQuit();
    
private slots:
    /**
     * Respond to start button click.
     */
    void on_startSamButton_clicked();

    /**
     * Respond to about action triggered.
     */
    void on_actionAbout_triggered();

    /**
     * Respond to change in volume slider value.
     * @param value new slider value
     */
    void on_volumeSlider_valueChanged(int value);

    /**
     * Respond to mute checkbox state change.
     * @param arg1 current checkbox state
     */
    void on_muteCheckBox_stateChanged(int arg1);

private:
    Ui::SamUI *ui;                  ///< UI
    StreamingAudioManager* m_sam;   ///< SAM instance
};

#endif // SAMUI_H
