#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>
#include <array>
#include <atomic>
#include <condition_variable>
#include <string>
#include <sstream>

#include <QSharedPointer>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QString>

#include <pjsua2.hpp>

#include "Softphone.hpp"
#include "SoftphoneArguments.hpp"
#include "ManualTestOpcode.hpp"
#include "Message.hpp"
#include "VTCPManualStatus.hpp"
#include "PjManager.hpp"
#include "MainWindow.hpp"
#include "InCallDialog.hpp"
#include "IncomingCallDialog.hpp"

class ManualTestHandler {
public:
    ManualTestHandler(QSharedPointer<MainWindow> mainWindow):
        _pjManager(8090, 5),
        _mainWindow(std::move(mainWindow))
    {
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_REGISTER_REQ, std::bind(&ManualTestHandler::manualTestRegister, this, std::placeholders::_1));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_UNREGISTER_REQ, std::bind(&ManualTestHandler::manualTestUnregister, this, std::placeholders::_1));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_CALL_REQ, std::bind(&ManualTestHandler::manualTestCall, this, std::placeholders::_1));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_HANGUP_REQ, std::bind(&ManualTestHandler::manualTestHangup, this, std::placeholders::_1));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_ANSWER_REQ, std::bind(&ManualTestHandler::manualTestAnswer, this, std::placeholders::_1));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_DECLINE_REQ, std::bind(&ManualTestHandler::manualTestDecline, this, std::placeholders::_1));
    }

    ~ManualTestHandler() = default;

    void handleManualTest(const Message& request)
    {
        try
        {
            auto op = request.readInteger();
            auto opcode = static_cast<ManualTestOpcode>(op);
            _manualTestHandlers.at(opcode)(request);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

private:
    void manualTestRegister(const Message& request)
    {
        auto index = request.readInteger();
        auto id = request.readInteger();
        auto domain = request.readString();

        QTextBrowser *logTextBrowser;

        if(!(index >= 0 && index <= 2))
        {
             _mainWindow->ui->softphone_log_text_browser_1->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_2->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_3->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
        }
        else if(index == 2)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
        }

        if(_manualTestSoftphones[index])
        {
            std::string errorMessage = "The softphone is already registered\n";
            logTextBrowser->insertPlainText(QString::fromStdString(errorMessage));
        }
        if(id > 0 && id < 9999)
        {
            std::string errorMessage = "Invalid ID, the ID cant be between 0 to 9999\n";
            logTextBrowser->insertPlainText(QString::fromStdString(errorMessage));
            return;
        }
        else
        {
            logTextBrowser->insertPlainText(QString::fromStdString("Trying to register...\n"));
            _manualTestSoftphones.at(index) = std::make_shared<Softphone>(
                SoftphoneArguments(DEFAULT_SECRET, domain, DEFAULT_TIMEOUT, id),
                std::bind(&ManualTestHandler::onIncomingCall, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3),
                std::bind(&ManualTestHandler::onCallState, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3),
                std::bind(&ManualTestHandler::onRegState, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
        }
    }

    void manualTestUnregister(const Message& request)
    {
        auto index = request.readInteger();

        QTextBrowser *logTextBrowser;

        if(!(index >= 0 && index <= 2))
        {
             _mainWindow->ui->softphone_log_text_browser_1->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_2->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_3->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
        }
        else {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
        }

        if(!_manualTestSoftphones[index])
        {
            logTextBrowser->insertPlainText(QString::fromStdString("Error: Can't unregister, please register first\n"));
        }
        else
        {
            logTextBrowser->insertPlainText(QString::fromStdString("Trying to unregister...\n"));
            _manualTestSoftphones[index]->unregister();
        }


    }

    void manualTestCall(const Message& request)
    {
        auto index = request.readInteger();
        auto destUri = request.readString();

        QTextBrowser *logTextBrowser;

        if(!(index >= 0 && index <= 2))
        {
             _mainWindow->ui->softphone_log_text_browser_1->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_2->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_3->
                     insertPlainText(QString::fromStdString("Manual test index error"));
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
        }
        else {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
        }

        if(!_manualTestSoftphones[index])
        {
            logTextBrowser->insertPlainText(QString::fromStdString("Error: Can't call, please register first\n"));
        }
        else
        {
            bool found = false;
            for(int i = 0; i < _manualTestSoftphones.max_size(); i++)
            {
                if(i != index && _manualTestSoftphones[i])
                {
                    if(_manualTestSoftphones[i]->getUri() == destUri)
                    {
                        found = true;
                    }
                }
            }

            if(!found)
            {
                logTextBrowser->insertPlainText(QString::fromStdString("Error: The uri not found in the client manual tests array\n"));
            }

            else
            {
                _manualTestSoftphones[index]->call(destUri);
            }
        }
    }

    void manualTestHangup(const Message& request)
    {
        auto index = request.readInteger();

        QTextBrowser *logTextBrowser;

        if(!(index >= 0 && index <= 2))
        {
             _mainWindow->ui->softphone_log_text_browser_1->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_2->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_3->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
        }
        else {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
        }

        if(!_manualTestSoftphones[index])
        {
            logTextBrowser->insertPlainText(QString::fromStdString("Error: Can't hangup, please register first\n"));
        }
        else
        {
            _manualTestSoftphones[index]->hangup();
        }
    }

    void manualTestAnswer(const Message& request)
    {
        auto index = request.readInteger();

        QTextBrowser *logTextBrowser;

        if(!(index >= 0 && index <= 2))
        {
             _mainWindow->ui->softphone_log_text_browser_1->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_2->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_3->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
        }
        else {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
        }

        if(!_manualTestSoftphones[index])
        {
            logTextBrowser->insertPlainText(QString::fromStdString("Error: Can't call, please register first\n"));
        }
        else
        {
            _manualTestSoftphones[index]->setAnsweredIncomingCall(true);
            _cv.notify_one();
        }
    }

    void manualTestDecline(const Message& request)
    {
        auto index = request.readInteger();

        QTextBrowser *logTextBrowser;

        if(!(index >= 0 && index <= 2))
        {
             _mainWindow->ui->softphone_log_text_browser_1->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_2->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_3->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
        }
        else {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
        }

        if(!_manualTestSoftphones[index])
        {
            logTextBrowser->insertPlainText(QString::fromStdString("Error: Can't call, please register first\n"));
        }
        else
        {
            _manualTestSoftphones[index]->setDeclinedIncomingCall(true);
            _cv.notify_one();
        }
    }

    void onCallState(const pj::OnCallStateParam &prm, const pj::CallInfo &ci, const int softphoneID)
    {
        std::shared_ptr<Softphone> softphone;
        int index = -1;
        for(int i=0; i < _manualTestSoftphones.size(); i++)
        {
            if(_manualTestSoftphones[i])
            {
                if(_manualTestSoftphones[i]->getId() == softphoneID)
                {
                    index = i;
                    softphone = _manualTestSoftphones[i];
                    break;
                }
            }
        }

        QTextBrowser *logTextBrowser;
        QLabel *statusLabel;
        QTextEdit *callUriTextEdit;
        QPushButton *declineButton,  *answerButton, *hangupButton, *callButton, *registrationButton;

        if(!(index >= 0 && index <= 2))
        {
             _mainWindow->ui->softphone_log_text_browser_1->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_2->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_3->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
            statusLabel = _mainWindow->ui->softphone_current_status_label_1;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_1;
            callButton = _mainWindow->ui->softphone_call_button_1;
            declineButton = _mainWindow->ui->softphone_decline_button_1;
            answerButton = _mainWindow->ui->softphone_answer_button_1;
            hangupButton = _mainWindow->ui->softphone_hangup_button_1;
            registrationButton = _mainWindow->ui->softphone_registration_button_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
            statusLabel = _mainWindow->ui->softphone_current_status_label_2;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_2;
            callButton = _mainWindow->ui->softphone_call_button_2;
            declineButton = _mainWindow->ui->softphone_decline_button_2;
            answerButton = _mainWindow->ui->softphone_answer_button_2;
            hangupButton = _mainWindow->ui->softphone_hangup_button_2;
            registrationButton = _mainWindow->ui->softphone_registration_button_2;
        }
        else
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
            statusLabel = _mainWindow->ui->softphone_current_status_label_3;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_3;
            callButton = _mainWindow->ui->softphone_call_button_3;
            declineButton = _mainWindow->ui->softphone_decline_button_3;
            answerButton = _mainWindow->ui->softphone_answer_button_3;
            hangupButton = _mainWindow->ui->softphone_hangup_button_3;
            registrationButton = _mainWindow->ui->softphone_registration_button_3;
        }

        PJ_UNUSED_ARG(prm);
        std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
                  << "]" << std::endl;
        std::stringstream message;
        message << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
                << "]" << std::endl;

        logTextBrowser->insertPlainText(QString::fromStdString(message.str()));
        if (ci.state == PJSIP_INV_STATE_DISCONNECTED)
        {
            if(ci.id == softphone->getCallID())
            {
                statusLabel->setText(QString::fromStdString("Call with " + ci.remoteUri + " ended"));
                emit _mainWindow->stopTimerSignal(index);
                hangupButton->setEnabled(false);
                callUriTextEdit->setEnabled(true);
                callButton->setEnabled(true);
                registrationButton->setEnabled(true);
                if(ci.lastStatusCode == PJSIP_SC_BUSY_HERE)
                {
                    logTextBrowser->insertPlainText(QString::fromStdString("The callee " + ci.remoteUri
                                                                           + " is busy with another call\n"));
                }
            }
        }
        if(ci.state == PJSIP_INV_STATE_CALLING)
        {
            statusLabel->setText(QString::fromStdString("Calling to " + ci.remoteUri));

            callUriTextEdit->setEnabled(false);
            callButton->setEnabled(false);
            registrationButton->setEnabled(false);
        }
        if(ci.state == PJSIP_INV_STATE_CONFIRMED)
        {
            statusLabel->setText(QString::fromStdString("In call with " + ci.remoteUri));
            emit _mainWindow->startTimerSignal(index);
            hangupButton->setEnabled(true);
            declineButton->setEnabled(false);
            answerButton->setEnabled(false);
        }
    }

    void onRegState(const pj::OnRegStateParam &prm, const pj::AccountInfo &ai, const int softphoneID)
    {
        int index = -1;
        for(int i=0; i < _manualTestSoftphones.size(); i++)
        {
            if(_manualTestSoftphones[i])
            {
                if(_manualTestSoftphones[i]->getId() == softphoneID)
                {
                    index = i;
                    break;
                }
            }
        }
        QTextBrowser* logTextBrowser;
        QLabel *uriLabel, *statusLabel;
        QTextEdit *ipTextEdit, *idTextEdit, *callUriTextEdit;
        QPushButton *registrationButton, *callButton;

        if(!(index >= 0 && index <= 2))
        {
            std::cout << "Manual test index error" << std::endl;
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
            uriLabel = _mainWindow->ui->softphone_uri_label_1;
            statusLabel = _mainWindow->ui->softphone_current_status_label_1;
            ipTextEdit = _mainWindow->ui->softphone_pbx_ip_edit_text_1;
            idTextEdit = _mainWindow->ui->softphone_id_edit_text_1;
            registrationButton = _mainWindow->ui->softphone_registration_button_1;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_1;
            callButton = _mainWindow->ui->softphone_call_button_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
            uriLabel = _mainWindow->ui->softphone_uri_label_2;
            statusLabel = _mainWindow->ui->softphone_current_status_label_2;
            ipTextEdit = _mainWindow->ui->softphone_pbx_ip_edit_text_2;
            idTextEdit = _mainWindow->ui->softphone_id_edit_text_2;
            registrationButton = _mainWindow->ui->softphone_registration_button_2;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_2;
            callButton = _mainWindow->ui->softphone_call_button_2;
        }
        else
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
            uriLabel = _mainWindow->ui->softphone_uri_label_3;
            statusLabel = _mainWindow->ui->softphone_current_status_label_3;
            ipTextEdit = _mainWindow->ui->softphone_pbx_ip_edit_text_3;
            idTextEdit = _mainWindow->ui->softphone_id_edit_text_3;
            registrationButton = _mainWindow->ui->softphone_registration_button_3;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_3;
            callButton = _mainWindow->ui->softphone_call_button_3;
        }

        std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
                  << prm.code << std::endl;
        std::stringstream message;
        message << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
                << prm.code << std::endl;
        logTextBrowser->insertPlainText(QString::fromStdString(message.str()));
        if(ai.regIsActive)
        {
            uriLabel->setText(QString::fromStdString(ai.uri));
            statusLabel->setText(QString::fromStdString("Registered"));
            ipTextEdit->setEnabled(false);
            idTextEdit->setEnabled(false);
            registrationButton->setText("Unregister");
            callUriTextEdit->setEnabled(true);
            callButton->setEnabled(true);
        }
        else
        {
            uriLabel->setText(QString::fromStdString("null"));
            statusLabel->setText(QString::fromStdString("Unregistered"));
            ipTextEdit->setEnabled(true);
            idTextEdit->setEnabled(true);
            registrationButton->setText("Register");
            callUriTextEdit->setEnabled(false);
            callButton->setEnabled(false);
            _manualTestSoftphones[index] = nullptr;
        }
    }

    void onIncomingCall(const pj::CallInfo& ci, pj::Call * incomingCall, const int softphoneID)
    {
        std::shared_ptr<Softphone> softphone;
        int index = -1;
        for(int i=0; i < _manualTestSoftphones.size(); i++)
        {
            if(_manualTestSoftphones[i])
            {
                if(_manualTestSoftphones[i]->getId() == softphoneID)
                {
                    index = i;
                    softphone = _manualTestSoftphones[i];
                    break;
                }
            }
        }

        QTextBrowser *logTextBrowser;
        QTextEdit *callUriTextEdit;
        QPushButton *declineButton,  *answerButton, *callButton;

        if(!(index >= 0 && index <= 2))
        {
             _mainWindow->ui->softphone_log_text_browser_1->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_2->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
             _mainWindow->ui->softphone_log_text_browser_3->
                     insertPlainText(QString::fromStdString("Manual test index error\n"));
            return;
        }
        else if(index == 0)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_1;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_1;
            declineButton = _mainWindow->ui->softphone_decline_button_1;
            answerButton = _mainWindow->ui->softphone_answer_button_1;
            callButton = _mainWindow->ui->softphone_call_button_1;
        }
        else if(index == 1)
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_2;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_2;
            declineButton = _mainWindow->ui->softphone_decline_button_2;
            answerButton = _mainWindow->ui->softphone_answer_button_2;
            callButton = _mainWindow->ui->softphone_call_button_2;
        }
        else
        {
            logTextBrowser = _mainWindow->ui->softphone_log_text_browser_3;
            callUriTextEdit = _mainWindow->ui->softphone_dest_edit_text_3;
            declineButton = _mainWindow->ui->softphone_decline_button_3;
            answerButton = _mainWindow->ui->softphone_answer_button_3;
            callButton = _mainWindow->ui->softphone_call_button_3;
        }

        std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
                  << ci.stateText << "]" << std::endl;
        std::stringstream message;
        message << "*** Incoming Call: " <<  ci.remoteUri << " ["
                << ci.stateText << "]" << std::endl;

        logTextBrowser->insertPlainText(QString::fromStdString(message.str()));

        if(softphone->activeCall())
        {
            pj::CallOpParam opcode;
            opcode.statusCode = PJSIP_SC_BUSY_HERE;
            incomingCall->hangup(opcode);
        }
        else
        {
            callUriTextEdit->setEnabled(false);
            callButton->setEnabled(false);
            declineButton->setEnabled(true);
            answerButton->setEnabled(true);

            std::unique_lock<std::mutex> lock(_mutex);
            auto now = std::chrono::steady_clock::now();
            auto wait_until = now + std::chrono::milliseconds(90000); // wait for 1.5 minutes

            auto statusFlag = _cv.wait_until(lock, wait_until);

            if(statusFlag == std::cv_status::timeout || softphone->getDeclinedIncomingCall())
            {
                pj::CallOpParam opcode;
                opcode.statusCode = PJSIP_SC_DECLINE;
                incomingCall->hangup(opcode);
            }

            else if(softphone->getAnsweredIncomingCall())
            {
                softphone->setCall(incomingCall);

                pj::CallOpParam prm;

                prm.statusCode = PJSIP_SC_OK;
                softphone->answer(prm);
            }

            softphone->setAnsweredIncomingCall(false);
            softphone->setDeclinedIncomingCall(false);

            lock.unlock();
        }
    }

    PjManager _pjManager;
    QSharedPointer<MainWindow> _mainWindow;

    std::unordered_map<ManualTestOpcode, std::function<void(const Message &)>> _manualTestHandlers;
    std::array<std::shared_ptr<Softphone>, 3> _manualTestSoftphones;

    std::mutex _mutex;
    std::condition_variable _cv;

    static constexpr int DEFAULT_ID_MULTIPLICATION_VALUE = 10000;

    static constexpr auto DEFAULT_SECRET = "12345678";
    static constexpr auto DEFAULT_TIMEOUT = 5000;
};
