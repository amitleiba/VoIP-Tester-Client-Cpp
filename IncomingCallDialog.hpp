#ifndef INCOMINGCALLDIALOG_H
#define INCOMINGCALLDIALOG_H

#include <QDialog>

namespace Ui {
class IncomingCallDialog;
}

class IncomingCallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IncomingCallDialog(QWidget *parent, const int index, const QString& uri);
    ~IncomingCallDialog();

public slots:
    void answerButtonClicked();
    void rejectButtonClicked();

signals:
    void answerButtonClickedSignal(const int);
    void rejectButtonClickedSignal(const int);

private:
    Ui::IncomingCallDialog *ui;
    const int _softphoneIndex;
};

#endif // INCOMINGCALLDIALOG_H
