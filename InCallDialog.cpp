#include "InCallDialog.hpp"
#include "ui_InCallDialog.h"

InCallDialog::InCallDialog(QWidget *parent, const int index) :
    QDialog(parent),
    ui(new Ui::InCallDialog),
    _softphoneIndex(index)
{
    ui->setupUi(this);

    connect(ui->hangupButton, SIGNAL(clicked()), this, SLOT(hangupButtonClicked()));

    time.setHMS(0,0,0);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(1000);
}

InCallDialog::~InCallDialog()
{
    delete ui;
}

void InCallDialog::timerUpdate()
{
    time = time.addSecs(1);
    QFont font = ui->countdownLabel->font();
    font.setPointSize(72);
    ui->countdownLabel->setText(time.toString("mm:ss"));
    ui->countdownLabel->setFont(font);
}

void InCallDialog::hangupButtonClicked()
{
    timer->stop();
    close();
    emit hangupButtonClickedSignal(_softphoneIndex);
}
