#ifndef INCALLDIALOG_HPP
#define INCALLDIALOG_HPP

#include <QDialog>
#include <QTime>
#include <QTimer>

namespace Ui {
class InCallDialog;
}

class InCallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InCallDialog(QWidget *parent, const int index);
    ~InCallDialog();

public slots:
    void timerUpdate();
    void hangupButtonClicked();

signals:
    void hangupButtonClickedSignal(const int);

private:
    Ui::InCallDialog *ui;
    const int _softphoneIndex;
    QTimer *timer;
    QTime time;
};

#endif // INCALLDIALOG_HPP
