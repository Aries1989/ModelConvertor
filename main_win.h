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
    main_win(QString strFilePath = "", QString strDstDir = "", QWidget * parent = nullptr);
    ~main_win();

    bool ConvertSkp2Cim(const QString& strSkpFilePath);

private:
    void Init(const QString& strFilePath);
    void Compress2Cim();
    void PrintLog(const QString& strLog);

private slots:
    void on_btnBrowser_clicked();

    void on_btnConvertor_clicked();

private:
    Ui::main_win *ui;
    QProcess m_process;

    bool m_bNoUi;
    QString m_strFilePath;
    QString m_strDstDir;
};
#endif // MAIN_WIN_H
