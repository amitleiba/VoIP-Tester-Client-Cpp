#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <iostream>

#include "LogPopupWindow.hpp"
#include "CheckableComboBox.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->connect_frame_connection_button, SIGNAL(clicked()), this, SLOT(connectButtonClicked()));
    connect(ui->auto_tests_button, SIGNAL(clicked()), this, SLOT(runAutoTestButtonClicked()));
    connect(ui->softphone_registration_button_1, SIGNAL(clicked()), this, SLOT(manualTestRegisterButtonClicked()));
    connect(ui->softphone_registration_button_2, SIGNAL(clicked()), this, SLOT(manualTestRegisterButtonClicked()));
    connect(ui->softphone_registration_button_3, SIGNAL(clicked()), this, SLOT(manualTestRegisterButtonClicked()));
    connect(ui->softphone_call_button_1, SIGNAL(clicked()), this, SLOT(manualTestCallButtonClicked()));
    connect(ui->softphone_call_button_2, SIGNAL(clicked()), this, SLOT(manualTestCallButtonClicked()));
    connect(ui->softphone_call_button_3, SIGNAL(clicked()), this, SLOT(manualTestCallButtonClicked()));
    connect(ui->softphone_hangup_button_1, SIGNAL(clicked()), this, SLOT(manualTestHangupButtonClicked()));
    connect(ui->softphone_hangup_button_2, SIGNAL(clicked()), this, SLOT(manualTestHangupButtonClicked()));
    connect(ui->softphone_hangup_button_3, SIGNAL(clicked()), this, SLOT(manualTestHangupButtonClicked()));
    connect(ui->softphone_answer_button_1, SIGNAL(clicked()), this, SLOT(manualTestAnswerButtonClicked()));
    connect(ui->softphone_answer_button_2, SIGNAL(clicked()), this, SLOT(manualTestAnswerButtonClicked()));
    connect(ui->softphone_answer_button_3, SIGNAL(clicked()), this, SLOT(manualTestAnswerButtonClicked()));
    connect(ui->softphone_decline_button_1, SIGNAL(clicked()), this, SLOT(manualTestDeclineButtonClicked()));
    connect(ui->softphone_decline_button_2, SIGNAL(clicked()), this, SLOT(manualTestDeclineButtonClicked()));
    connect(ui->softphone_decline_button_3, SIGNAL(clicked()), this, SLOT(manualTestDeclineButtonClicked()));
    connect(ui->Log_History_Refresh_button, SIGNAL(clicked()), this, SLOT(refreshHIstoryHeadersButtonClicked()));
    connect(ui->Log_History_List_Widget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(historyListItemClicked(QListWidgetItem*)));
    connect(this, &MainWindow::printLogSignal, this, &MainWindow::printLog);
    connect(this, &MainWindow::openLogPopupWindowSignal, this, &MainWindow::openLogPopupWindow);
    connect(this, &MainWindow::startTimerSignal, this, &MainWindow::startTimer);
    connect(this, &MainWindow::stopTimerSignal, this, &MainWindow::stopTimer);

//    connect(this, &MainWindow::openIncomingCallDialogSignal, this, &MainWindow::openIncomingCallDialog);
//    connect(this, &MainWindow::openInCallDialogSignal, this, &MainWindow::openInCallDialog);

    _timerLabels[0] = ui->softphone_timer_label_1;
    _timerLabels[1] = ui->softphone_timer_label_2;
    _timerLabels[2] = ui->softphone_timer_label_3;

    for(int i = 0; i < _timers.size();  i++)
    {
        _timers[i] = new QTimer(this);
        connect(_timers[i], SIGNAL(timeout()), this, SLOT(timerUpdate()));
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectButtonClicked()
{
    std::cout << "onConnectButtonClicked" << std::endl;

    emit connectButtonClickedSignal(ui->connect_frame_ip_edit_text->toPlainText().toStdString(),
                                    ui->connect_frame_port_edit_text->toPlainText().toStdString());
}

void MainWindow::disconnectButtonClicked()
{
    std::cout << "disconnectButtonClicked" << std::endl;

    emit disconnectButtonClickedSignal();
}

void MainWindow::runAutoTestButtonClicked()
{
    std::cout << "runAutoTestButtonClicked" << std::endl;

    emit runAutoTestSignal(ui->auto_tests_pbx_ip_edit_text->toPlainText().toStdString(),
                           ui->auto_tests_amout_edit_text->toPlainText().toInt());
}

void MainWindow::manualTestRegisterButtonClicked()
{
    std::cout << "manualTestRegisterButtonClicked" << std::endl;

    QPushButton* sender = (QPushButton*)QObject::sender();
    if(sender->text() == QString::fromStdString("Register"))
    {
        if (sender == ui->softphone_registration_button_1){
            emit manualTestRegisterSignal(0, ui->softphone_id_edit_text_1->toPlainText().toInt(),
                                             ui->softphone_pbx_ip_edit_text_1->toPlainText().toStdString());
        } else if (sender == ui->softphone_registration_button_2) {
            emit manualTestRegisterSignal(1, ui->softphone_id_edit_text_2->toPlainText().toInt(),
                                             ui->softphone_pbx_ip_edit_text_2->toPlainText().toStdString());
        } else if (sender == ui->softphone_registration_button_3) {
            emit manualTestRegisterSignal(2, ui->softphone_id_edit_text_3->toPlainText().toInt(),
                                             ui->softphone_pbx_ip_edit_text_3->toPlainText().toStdString());
        }
    }
    else
    {
        if (sender == ui->softphone_registration_button_1) {
            emit manualTestUnregisterSignal(0);
        } else if (sender == ui->softphone_registration_button_2) {
            emit manualTestUnregisterSignal(1);
        } else if (sender == ui->softphone_registration_button_3) {
            emit manualTestUnregisterSignal(2);
        }
    }

}

//void MainWindow::manualTestUnregisterButtonClicked()
//{
//    std::cout << "manualTestUnregisterButtonClicked" << std::endl;

//    QObject* sender = QObject::sender();
//    if (sender == ui->softphone_unregister_button_1) {
//        emit manualTestUnregisterSignal(0);
//    } else if (sender == ui->softphone_unregister_button_2) {
//        emit manualTestUnregisterSignal(1);
//    } else if (sender == ui->softphone_unregister_button_3) {
//        emit manualTestUnregisterSignal(2);
//    }
//}

void MainWindow::manualTestCallButtonClicked()
{
    std::cout << "manualTestCallButtonClicked" << std::endl;

    QObject* sender = QObject::sender();
    if (sender == ui->softphone_call_button_1) {
        emit manualTestCallSignal(0, ui->softphone_dest_edit_text_1->toPlainText().toStdString());
    } else if (sender == ui->softphone_call_button_2) {
        emit manualTestCallSignal(1, ui->softphone_dest_edit_text_2->toPlainText().toStdString());
    } else if (sender == ui->softphone_call_button_3) {
        emit manualTestCallSignal(2, ui->softphone_dest_edit_text_3->toPlainText().toStdString());
    }
}

void MainWindow::manualTestHangupButtonClicked()
{
    std::cout << "manualTestHangupButtonClicked" << std::endl;

    QObject* sender = QObject::sender();
    if (sender == ui->softphone_hangup_button_1) {
        emit manualTestHangupSignal(0);
    } else if (sender == ui->softphone_hangup_button_2) {
        emit manualTestHangupSignal(1);
    } else if (sender == ui->softphone_hangup_button_3) {
        emit manualTestHangupSignal(2);
    }
}

void MainWindow::manualTestAnswerButtonClicked()
{
    std::cout << "manualTestAnswerButtonClicked" << std::endl;

    QObject* sender = QObject::sender();
    if (sender == ui->softphone_answer_button_1) {
        emit manualTestAnswerSignal(0);
    } else if (sender == ui->softphone_answer_button_2) {
        emit manualTestAnswerSignal(1);
    } else if (sender == ui->softphone_answer_button_3) {
        emit manualTestAnswerSignal(2);
    }
}

void MainWindow::manualTestDeclineButtonClicked()
{
    std::cout << "manualTestDeclineButtonClicked" << std::endl;

    QObject* sender = QObject::sender();
    if (sender == ui->softphone_decline_button_1) {
        emit manualTestDeclineSignal(0);
    } else if (sender == ui->softphone_decline_button_2) {
        emit manualTestDeclineSignal(1);
    } else if (sender == ui->softphone_decline_button_3) {
        emit manualTestDeclineSignal(2);
    }
}

void MainWindow::refreshHIstoryHeadersButtonClicked()
{
    std::cout << "refreshHIstoryHeadersButtonClicked" << std::endl;

    emit refreshHIstoryHeadersSignal();
}

void MainWindow::historyListItemClicked(QListWidgetItem* item)
{
    std::cout << "historyListItemClicked" << std::endl;

    emit historyListItemClickedSignal(item->text().toStdString());
}

void MainWindow::printLog(QTextBrowser *textBrowser, const QString& data)
{
    textBrowser->setText(data);
}

void MainWindow::openLogPopupWindow(const QString &data)
{
    LogPopupWindow *popupWindow = new LogPopupWindow(this, data);
    popupWindow->show();
}

void MainWindow::startTimer(const int softphoneIndex)
{
    _timerLabels[softphoneIndex]->setEnabled(true);
    _times[softphoneIndex].setHMS(0,0,0);
    _timers[softphoneIndex]->start(1000);
}

void MainWindow::stopTimer(const int softphoneIndex)
{
    _timers[softphoneIndex]->stop();
    _timerLabels[softphoneIndex]->setEnabled(false);
}

void MainWindow::timerUpdate()
{
    QObject* senderObj = QObject::sender();

    int softphoneIndex = -1;
    for (int i = 0; i < _timers.size(); ++i)
    {
        if (senderObj == _timers[i])
        {
            softphoneIndex = i;
            break;
        }
    }
    if(softphoneIndex != -1)
    {
        _times[softphoneIndex] = _times[softphoneIndex].addSecs(1);
        QFont font = _timerLabels[softphoneIndex]->font();
        font.setPointSize(48);
        _timerLabels[softphoneIndex]->setText(_times[softphoneIndex].toString("mm:ss"));
        _timerLabels[softphoneIndex]->setFont(font);
    }
}

//void MainWindow::openIncomingCallDialog(const int index, const QString &uri)
//{
//    IncomingCallDialog *incomingCallDialog = new IncomingCallDialog(this, index, uri);
//    connect(incomingCallDialog, &IncomingCallDialog::answerButtonClickedSignal,
//            this, &MainWindow::manualTestAnswerButtonClicked);
//    connect(incomingCallDialog, &IncomingCallDialog::rejectButtonClickedSignal,
//            this, &MainWindow::manualTestDeclineButtonClicked);
//    incomingCallDialog->show();
//}

//void MainWindow::openInCallDialog(const int index)
//{
//    InCallDialog *inCallDialog = new InCallDialog(this, index);
//    connect(inCallDialog, &InCallDialog::hangupButtonClickedSignal,
//            this, &MainWindow::manualTestHangupButtonClicked);
//    inCallDialog->show();
//}

