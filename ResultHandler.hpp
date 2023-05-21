#pragma once

#include <functional>
#include <iostream>
#include <unordered_map>
#include <string>
#include <regex>

#include <QSharedPointer>

#include <nlohmann/json.hpp>

#include "VTCPOpcode.hpp"
#include "Message.hpp"
#include "MainWindow.hpp"

using json = nlohmann::json;

class ResultHandler
{
public:
    ResultHandler(QSharedPointer<MainWindow> mainWindow,
                  std::function<void(const Message &)> send):
        _mainWindow(std::move(mainWindow)),
        _send(std::move(send))
    {
        _handlers.emplace(VTCPOpcode::VTCP_CONNECT_RES, std::bind(&ResultHandler::onVtcpConnectResualt, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_DISCONNECT_RES, std::bind(&ResultHandler::onVtcpDisconnectResualt, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_AUTO_TEST_RES, std::bind(&ResultHandler::onVtcpAutoTestResualt, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_HISTORY_HEADER_RES, std::bind(&ResultHandler::onVtcpHistoryHeaderResualt, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_HISTORY_LOG_RES, std::bind(&ResultHandler::onVtcpHistoryLogResualtResualt, this, std::placeholders::_1));
    }

    ~ResultHandler() = default;

    void handle(const Message& result)
    {
        try
        {
            auto op = result.readInteger();
            auto opcode = static_cast<VTCPOpcode>(op);
            _handlers.at(opcode)(result);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

private:
    void onVtcpConnectResualt(const Message& result)
    {
        std::cout << "onVtcpConnectResualt"  << std::endl;

        Message message;
        message.push(static_cast<int>(VTCPOpcode::VTCP_HISTORY_HEADER_REQ));
        _send(message);
    }

    void onVtcpDisconnectResualt(const Message& result)
    {
        std::cout << "onVtcpDisconnectResualt"  << std::endl;
    }

    void onVtcpAutoTestResualt(const Message& result)
    {
        std::cout << "onVtcpAutoTestResualt"  << std::endl;

        auto data = result.readString();
        auto formattedData = formatJson(data);

        emit _mainWindow->printLogSignal(_mainWindow->ui->auto_tests_log_text_browser, QString::fromStdString(formattedData));
    }

    void onVtcpHistoryHeaderResualt(const Message& result)
    {
        auto data = result.readString();
        _mainWindow->ui->Log_History_List_Widget->clear();
        json json_obj = json::parse(data);
        for (auto& header : json_obj["history-headers"]) {
            std::string jsonStr = header.dump();
            _mainWindow->ui->Log_History_List_Widget->addItem(QString::fromStdString(formatJson(jsonStr)));
        }
    }

    void onVtcpHistoryLogResualtResualt(const Message& result)
    {
        auto data = result.readString();
        auto formattedData = formatJson(data);
        emit _mainWindow->openLogPopupWindowSignal(QString::fromStdString(formattedData));
    }

    std::string formatJson(const std::string& data)
    {
        std::string formattedStr = data;
        formattedStr = std::regex_replace(formattedStr, std::regex("\\s*\\{\\s*"), "{");  // remove space before and after opening curly braces
        formattedStr = std::regex_replace(formattedStr, std::regex(","), ",\n");  // add newline after commas
        formattedStr = std::regex_replace(formattedStr, std::regex("\\{"), "{\n");  // add newline after opening curly braces
        formattedStr = std::regex_replace(formattedStr, std::regex("\\}"), "\n}");  // add newline before closing curly braces
        formattedStr = std::regex_replace(formattedStr, std::regex("\\[\\{"), "[\n{");  // add newline after opening square braces and before opening curly braces
        formattedStr = std::regex_replace(formattedStr, std::regex("\\}\\s*\\]"), "}\n]");  // add newline before closing curly braces and after closing square braces
        formattedStr = std::regex_replace(formattedStr, std::regex("\"type\""), "   \"type\"");  // add spaces before "type"
        formattedStr = std::regex_replace(formattedStr, std::regex("\"description-time\""), "   \"description time\"");  // add spaces before "description-time"
        formattedStr = std::regex_replace(formattedStr, std::regex("\"description\""), "   \"description\"");  // add spaces before "description"
        formattedStr = std::regex_replace(formattedStr, std::regex("data : "), "data:\n");  // add newline after "data :"
        return formattedStr;
    }

    QSharedPointer<MainWindow> _mainWindow;

    std::unordered_map<VTCPOpcode, std::function<void(const Message &)>> _handlers;
    std::function<void(const Message &)> _send;
};
