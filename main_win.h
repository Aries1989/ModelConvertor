#ifndef MAIN_WIN_H
#define MAIN_WIN_H

#include <QWidget>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class main_win; }
QT_END_NAMESPACE

class main_win : public QWidget
{
    Q_OBJECT

public:
    main_win(QWidget *parent = nullptr);
    ~main_win();

private:
    void Compress2Cim();

private slots:
    void on_btnBrowser_clicked();

    void on_btnConvertor_clicked();

private:
    Ui::main_win *ui;
    QProcess m_process;
};
#endif // MAIN_WIN_H
