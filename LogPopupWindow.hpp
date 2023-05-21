#pragma once

#include <QDialog>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QLabel>

class LogPopupWindow : public QDialog {
public:
    LogPopupWindow(QWidget *parent,QString data) : QDialog(parent) {
        setWindowTitle("Log Window");
        setGeometry(100, 100, 500, 850);

        auto layout = new QVBoxLayout(this);

        text_browser = new QTextBrowser(this);
        text_browser->setGeometry(2, 0, 496, 648);
        text_browser->setPlainText(data);

        layout->addWidget(text_browser);
        setLayout(layout);
    }

private:
    QTextBrowser *text_browser;
};
