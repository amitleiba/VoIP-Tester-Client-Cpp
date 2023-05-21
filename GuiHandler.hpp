#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <QApplication>
#include <QObject>
#include <QSharedPointer>

#include <nlohmann/json.hpp>

#include "MainWindow.hpp"
#include "Message.hpp"
#include "VTCPOpcode.hpp"
#include "VTCPClient.hpp"
#include "ResultHandler.hpp"
#include "ManualTestHandler.hpp"
#include "ManualTestOpcode.hpp"

using json = nlohmann::json;

class GuiHandler : public QObject
{
public:
    GuiHandler(QSharedPointer<MainWindow> mainWindow):
        _connected(std::make_shared<std::atomic<bool>>(false)),
        _mainWindow(mainWindow),
        _client(_connected, std::bind(&GuiHandler::onMessageReceived, this, std::placeholders::_1)),
        _resultHandler(_mainWindow, std::bind(&VTCPClient::send, &_client, std::placeholders::_1)),
        _manualTestHandler(_mainWindow)
    {
        initSignals();
    }


    ~GuiHandler() = default;

    void runGui()
    {
        _mainWindow->show();
//        _app.exec();
    }

private:
    void initSignals()
    {
        std::cout << "init Buttons" << std::endl;
        QObject::connect(&(*_mainWindow), &MainWindow::connectButtonClickedSignal, this, &GuiHandler::onConnectButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::disconnectButtonClickedSignal, this, &GuiHandler::onDisconnectButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::runAutoTestSignal, this, &GuiHandler::onAutoTestButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::refreshHIstoryHeadersSignal, this, &GuiHandler::onRefreshHeadersButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::historyListItemClickedSignal, this, &GuiHandler::onItemClickedInHistoryList);
        QObject::connect(&(*_mainWindow), &MainWindow::manualTestRegisterSignal, this, &GuiHandler::onManualTestRegisterButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::manualTestUnregisterSignal, this, &GuiHandler::onManualTestUnregisterButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::manualTestCallSignal, this, &GuiHandler::onManualTestCallButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::manualTestHangupSignal, this, &GuiHandler::onManualTestHangupButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::manualTestAnswerSignal, this, &GuiHandler::onManualTestAnswerButtonClicked);
        QObject::connect(&(*_mainWindow), &MainWindow::manualTestDeclineSignal, this, &GuiHandler::onManualTestDeclineButtonClicked);


    }

signals:
    void printAutoTestSignal(const std::string& data);

public slots:
    void onConnectButtonClicked(const std::string& domain, const std::string& port)
    {
        if(*_connected)
        {
            std::cout << "onDisconnectButtonClicked " << std::endl;

            _client.disconnect();
            *_connected = false;
            _mainWindow->ui->connect_frame_ip_edit_text->setEnabled(true);
            _mainWindow->ui->connect_frame_port_edit_text->setEnabled(true);
            _mainWindow->ui->tabWidget->setEnabled(false);
            _mainWindow->ui->connect_frame_connection_button->setText("Connect");
        }
        else
        {
            std::cout << "onConnectButtonClickedSignal " << domain << ", " << port << std::endl;

            try
            {
                _client.connect(domain, port);
                _mainWindow->ui->connect_frame_ip_edit_text->setEnabled(false);
                _mainWindow->ui->connect_frame_port_edit_text->setEnabled(false);
                _mainWindow->ui->tabWidget->setEnabled(true);
                _mainWindow->ui->connect_frame_connection_button->setText("Disconnect");
            }
            catch (std::exception e)
            {
                *_connected = false;
                std::cout << e.what() << std::endl;
            }
        }
    }

    void onDisconnectButtonClicked()
    {
        if(!*_connected)
            return;

        std::cout << "onDisconnectButtonClicked " << std::endl;

        _client.disconnect();
        *_connected = false;
        _mainWindow->ui->connect_frame_ip_edit_text->setEnabled(true);
        _mainWindow->ui->connect_frame_port_edit_text->setEnabled(true);
    }

    void onAutoTestButtonClicked(const std::string& ip, int amount)
    {
        if(!*_connected)
            return;

        std::cout << "onAutoTestButtonClicked " << std::endl;

        Message message;
        message.push(static_cast<int>(VTCPOpcode::VTCP_AUTO_TEST_REQ));
        message.push(ip);
        message.push(amount);

        _client.send(message);
    }

    void onManualTestRegisterButtonClicked(int softphoneIndex, int softphoneId, const std::string& pbxIP)
    {
        std::cout << "onManualTestRegisterButtonClicked " << softphoneIndex << ", " <<
                     softphoneId << ", " << pbxIP << std::endl;
        if(!(*_connected)) {
            return;
        }

        Message message;

        message.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_REGISTER_REQ));
        message.push(softphoneIndex);
        message.push(softphoneId);
        message.push(pbxIP);
        _manualTestHandler.handleManualTest(message);
    }

    void onManualTestUnregisterButtonClicked(int softphoneIndex)
    {
        std::cout << "onManualTestUnregisterButtonClicked " << softphoneIndex << std::endl;
        if(!(*_connected)) {
            return;
        }

        Message message;

        message.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_UNREGISTER_REQ));
        message.push(softphoneIndex);
        _manualTestHandler.handleManualTest(message);
    }

    void onManualTestCallButtonClicked(int softphoneIndex, const std::string& destUri)
    {
        std::cout << "onManualTestCallButtonClicked " << softphoneIndex << ", " << destUri << std::endl;
        if(!(*_connected)) {
            return;
        }

        Message message;

        message.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_CALL_REQ));
        message.push(softphoneIndex);
        message.push(destUri);
        _manualTestHandler.handleManualTest(message);
    }

    void onManualTestHangupButtonClicked(int softphoneIndex)

    {
        std::cout << "onManualTestHangupButtonClicked " << softphoneIndex << std::endl;
        if(!(*_connected)) {
            return;
        }

        Message message;

        message.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_HANGUP_REQ));
        message.push(softphoneIndex);
        _manualTestHandler.handleManualTest(message);
    }

    void onManualTestAnswerButtonClicked(int softphoneIndex)
    {
        std::cout << "onManualTestAnswerButtonClicked " << softphoneIndex << std::endl;
        if(!(*_connected)) {
            return;
        }

        Message message;

        message.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_ANSWER_REQ));
        message.push(softphoneIndex);
        _manualTestHandler.handleManualTest(message);
    }

    void onManualTestDeclineButtonClicked(int softphoneIndex)
    {
        std::cout << "onManualTestAnswerButtonClicked " << softphoneIndex << std::endl;
        if(!(*_connected)) {
            return;
        }

        Message message;

        message.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_DECLINE_REQ));
        message.push(softphoneIndex);
        _manualTestHandler.handleManualTest(message);
    }

    void onRefreshHeadersButtonClicked() {
        Message message;
        message.push(static_cast<int>(VTCPOpcode::VTCP_HISTORY_HEADER_REQ));
        _client.send(message);
    }

    void onItemClickedInHistoryList(const std::string& data) {
        if(!(*(_connected)))
        {
            return;
        }

        Message message;
        message.push(static_cast<int>(VTCPOpcode::VTCP_HISTORY_LOG_REQ));
        json json_list = json::parse(data);
        message.push(json_list["_id"].get<std::string>());
        _client.send(message);
    }


private:
    void onMessageReceived(const Message& request)
    {
        _resultHandler.handle(request);
    }

    std::shared_ptr<std::atomic<bool>> _connected;
    QSharedPointer<MainWindow> _mainWindow;
    VTCPClient _client;
    ResultHandler _resultHandler;
    ManualTestHandler _manualTestHandler;

};
