/****************************************************************************
 * Main developer, C# developing:                                           *
 * Copyright (C) 2014-2016 by Sergey Zheigurov                              *
 * Russia, Novy Urengoy                                                     *
 * zheigurov@gmail.com                                                      *
 *                                                                          *
 * C# to Qt portation, Linux developing                                     *
 * Copyright (C) 2015-2016 by Eduard Kalinowski                             *
 * Germany, Lower Saxony, Hanover                                           *
 * eduard_kalinowski@yahoo.de                                               *
 *                                                                          *
 * C# project CNC-controller-for-mk1                                        *
 * https://github.com/selenur/CNC-controller-for-mk1                        *
 *                                                                          *
 * The Qt project                                                           *
 * https://github.com/eduard-x/cnc-qt                                       *
 *                                                                          *
 * CNC-Qt is free software; may be distributed and/or modified under the    *
 * terms of the GNU General Public License version 3 as published by the    *
 * Free Software Foundation and appearing in the file LICENSE_GPLv3         *
 * included in the packaging of this file.                                  *
 *                                                                          *
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU Lesser General Public         *
 * License along with CNC-Qt. If not, see  http://www.gnu.org/licenses      *
 ****************************************************************************/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QCloseEvent>
#include <QKeyEvent>

#include <QProgressBar>
#include <QScrollArea>
#include <QImage>
#include <QTimeLine>

#include <deque>
#include <utility>

#include "../version.h"
#include "vec.h"

#if USE_OPENGL == true
#include "GLWidget.h"
#endif

#include "Reader.h"
#include "mk1Controller.h"
#include "Geometry.h"
#include "Translator.h"
#include "ui_MainWindow.h"

#if USE_OPENGL == true
class GLWidget;
#endif

class mk1Controller;
class cTranslator;
class Reader;


class MessageTimerBox: public QMessageBox
{
    private:
        int timeout;
        bool autoClose;
        int currentTime;
        QString defaultText;

    public:
        void setTimeout(int t);
        void setDefaultText(const QString &t);
        void setAutoClose(bool a);
        void showEvent ( QShowEvent * event );

    private:
        void timerEvent(QTimerEvent *event);
        //     static int exec(const QString &title, const QString &text, int ticon, int timer);
};


class MessageBox: public cTranslator
{
    public:
        static int exec(void* p, const QString &title, const QString &text, int ticon);
};



enum AxesFix { FixX = 0, FixY, FixZ };

//
enum KeyPad { NoManuaControl = -1, NumPad = 0, CursorPad, UserDefined };

//
// task state
//
enum EStatusTask { Waiting = 0, Start, Working, Paused, Stop };

//
// class for current state of controller
//
class Task
{
    public:
        static EStatusTask Status;
        //
        // begin of task (line of g-code)
        static int posCodeStart;
        //
        // end of task (line of g-code)
        static int posCodeEnd;
        //
        // current line
        static int posCodeNow;
};


struct uKeys {
    QString name;
    Qt::Key code;
};


class MainWindow : public QMainWindow, public Ui::MainWindow,  public Reader
{
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

        float GetDeltaZ(float _x, float _y);

        void moveToPoint(bool surfaceScan = false);

    public:
        mk1Controller *cnc;

        QVector<QVector<dPoint> > surfaceMatrix; // scanned points of surface
        int scanPosX;
        int scanPosY;

        QString programStyleSheet;

        //
        // for the correction possibility
        bool Correction;
        float deltaX;
        float deltaY;
        float deltaZ;
        float koeffSizeX;
        float koeffSizeY;
        int  fixedAxes;
        bool deltaFeed;

        bool enableOpenGL;

        // user defined control keys
        QVector<uKeys> userKeys;

        //
        int veloManual;

        int currentKeyPad;
#if USE_OPENGL == true
        GLWidget *scene3d; // OpenGL widget
#endif
        // 3d Settings
        bool ShowGrate;
        float grateXmin;
        float grateXmax;
        float grateYmin;
        float grateYmax;

        int PosX, PosY, PosZ;
        int PosAngleX, PosAngleY, PosAngleZ;
        int PosZoom;

        bool ShowInstrument;
        bool ShowGrid;
        bool ShowSurface;
        bool ShowAxes;

        int GridXstart;
        int GridXend;
        int GridYstart;
        int GridYend;
        int GrigStep;
        // end of 3d


    public slots:
        // connect to controller
        void onCncMessage(int n_msg);
        void onCncNewData();
        void onCncHotplug();


    private slots:
        void onExit();
        void onManualControlDialog();

        void onOpenFile();
        void onSaveFile();

        void onChangeFix(bool b);

        void onDeviceInfo();

        void onSettings();
        void onLogClear();

        void onSpindel();
        void onEmergyStop();
        void onScanSurface();
        void onEditGCode(int row, int col);
        void onCellSelect(int row, int col);

        void onStartTask();
        void onPauseTask();
        void onStopTask();
        void onMainTaskTimer();

        void onStatus(); // not connected
        void onRunToPoint();
        void onSendCommand();

        void on3dSettings();

        void onGeneratorCode(); // not connected

        bool readLangDir();

        void onAbout();
        void onAboutQt();

        void setLang(QAction* mnu);

    public slots:
        void onButtonXtoZero();
        void onButtonYtoZero();
        void onButtonZtoZero();

        void getZRotation(int z);
        void getYRotation(int y);
        void getXRotation(int x);
        void getScale(int s);

    private:
        void Feed();
        bool runCommand();
        void fillListWidget(QStringList listCode);
        QString getLocaleString();
        bool getLangTable();
        void setLangGUI();
        void displayRotation();
        void addConnections();
        void translateGUI();
        void addStatusWidgets();

        void refreshElementsForms();

        void AddLog(QString _text = "");
        void SendSignal();
        void readGUISettings();
        void writeGUISettings();
        void readSettings(bool global);
        void writeSettings(bool global);
        QString selectedLang;

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        QTimer  mainTaskTimer;
        QLabel *statusLabel1;
        QProgressBar *statusProgress;
        QLabel *statusLabel2;

        QTimer  renderTimer;
        QVector<QAction*> actLangSelect;
        QMenu *langMenu;
        QActionGroup* langGroup;
        QFont sysFont;
        short fontSize;
        //         QString programStyleSheet;
        QStringList langFiles;
        QString langDir;
        QString helpDir;
        QString currentLang;

        int veloSubmission;
        int veloMoving;

        int xAngle, yAngle, zAngle;
        int scale;
        QString currentAppDir;
};

#endif // MAINWINDOW_H
