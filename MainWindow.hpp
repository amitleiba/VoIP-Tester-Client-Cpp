#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTextBrowser>
#include <QTime>
#include <QTimer>

#include <string>
#include <array>

#include "ui_MainWindow.h"
//#include "IncomingCallDialog.hpp"
//#include "InCallDialog.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

private slots:
    void connectButtonClicked();
    void disconnectButtonClicked();
    void runAutoTestButtonClicked();
    void manualTestRegisterButtonClicked();
    void manualTestCallButtonClicked();
    void manualTestHangupButtonClicked();
    void manualTestAnswerButtonClicked();
    void manualTestDeclineButtonClicked();
    void refreshHIstoryHeadersButtonClicked();
    void historyListItemClicked(QListWidgetItem*);
    void printLog(QTextBrowser *textBrowser, const QString& data);
    void openLogPopupWindow(const QString& data);
    void startTimer(const int softphonnIndex);
    void stopTimer(const int softphonnIndex);
    void timerUpdate();

signals:
    void connectButtonClickedSignal(const std::string &, const std::string&);
    void disconnectButtonClickedSignal();
    void runAutoTestSignal(const std::string& pbxIp, int amount);
    void manualTestRegisterSignal(int index, int id, const std::string& pbxIp);
    void manualTestUnregisterSignal(int index);
    void manualTestCallSignal(int index, const std::string& dest);
    void manualTestHangupSignal(int index);
    void manualTestAnswerSignal(int index);
    void manualTestDeclineSignal(int index);
    void printLogSignal(QTextBrowser *textBrowser, const QString& data);
    void historyListItemClickedSignal(const std::string& item);
    void openLogPopupWindowSignal(const QString& data);
    void refreshHIstoryHeadersSignal();
    void startTimerSignal(const int softphonnIndex);
    void stopTimerSignal(const int softphonnIndex);

private:
    std::array<QTimer *, 3> _timers;
    std::array<QTime, 3> _times;
    std::array<QLabel *, 3> _timerLabels;
};
#endif // MAINWINDOW_HPP
