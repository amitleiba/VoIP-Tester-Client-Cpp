#include "IncomingCallDialog.hpp"
#include "ui_incomingcalldialog.h"

IncomingCallDialog::IncomingCallDialog(QWidget *parent, const int index, const QString& uri) :
    QDialog(parent),
    ui(new Ui::IncomingCallDialog),
    _softphoneIndex(index)
{
    ui->setupUi(this);
    ui->uriLabel->setText(uri);

    connect(ui->answerButton, SIGNAL(clicked()), this, SLOT(answerButtonClicked()));
    connect(ui->rejectButton, SIGNAL(clicked()), this, SLOT(rejectButtonClicked()));
}

IncomingCallDialog::~IncomingCallDialog()
{
    delete ui;
}

void IncomingCallDialog::answerButtonClicked()
{
    close();
    emit answerButtonClickedSignal(_softphoneIndex);
}

void IncomingCallDialog::rejectButtonClicked()
{
    close();
    emit answerButtonClickedSignal(_softphoneIndex);
}
