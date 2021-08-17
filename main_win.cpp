#include "main_win.h"
#include "ui_main_win.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QMap>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDebug>

const QString g_strConvertorProgramName = "Transcode.CPP.exe";
const QString g_strCompressProgramName = "7z.exe";
const QString g_strTempDirName = "tmp";
const QString g_strDstFileExt = ".cim";

main_win::main_win(QString strFilePath, QString strDstDir, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::main_win)
    , m_strFilePath(strFilePath)
    , m_strDstDir(strDstDir)
{
    ui->setupUi(this);
    m_bNoUi = !strFilePath.isEmpty();

    Init(strFilePath);
}

main_win::~main_win()
{
    delete ui;
}

void main_win::Compress2Cim()
{
    QFileInfo fi(m_strFilePath);
    QString strFileName = fi.fileName();
    QString strDstDir = (m_strDstDir.isEmpty() ? fi.dir().path() : m_strDstDir);
    QString strDstFilePath = strDstDir + "/" + strFileName + ".zip";

    QString strAppDir = QCoreApplication::applicationDirPath();
    QString strApp7z = strAppDir + "/" + g_strCompressProgramName;
    m_process.setProgram(strApp7z);

    QString strWoringDir = strAppDir + "/" + g_strTempDirName;
    m_process.setWorkingDirectory(strWoringDir);

    QStringList lstArgs = {"a", strDstFilePath, "-r"};
    m_process.setArguments(lstArgs);

    m_process.start(QIODevice::ReadOnly);
}

void main_win::PrintLog(const QString &strLog)
{
    if (m_bNoUi)
    {
        qDebug() << strLog;
    }
    else
    {
        ui->edtLog->appendPlainText(strLog);
    }
}

void main_win::on_btnBrowser_clicked()
{
    QStringList lstDir = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    QString strDir = lstDir.empty() ?"." : lstDir.first();
    QString strFilePath = QFileDialog::getOpenFileName(this, "open",  strDir);

    ui->edtFilePath->setText(strFilePath);
    m_strFilePath = strFilePath;
}

bool main_win::ConvertSkp2Cim(const QString &strSkpFilePath)
{
    QString strAppDir = QCoreApplication::applicationDirPath();
    m_process.setWorkingDirectory(strAppDir);

    QString strExePath = strAppDir + "/" + g_strConvertorProgramName;
    m_process.setProgram(strExePath);

    QDir dirApp(strAppDir);
    if (dirApp.exists(g_strTempDirName))
    {
        QDir dirTmp(strAppDir + "/" + g_strTempDirName);
        dirTmp.removeRecursively();
    }
    dirApp.mkdir(g_strTempDirName);

    QString strDstDir = strAppDir + "/" + g_strTempDirName;
    QStringList lstArgs = {strSkpFilePath, "--destination="+strDstDir};
    m_process.setArguments(lstArgs);

    m_process.start(QIODevice::ReadOnly);

    return true;
}

void main_win::Init(const QString& strFilePath)
{
    connect(&m_process, &QProcess::errorOccurred, this, [&](QProcess::ProcessError error){
        static QMap<QProcess::ProcessError, QString> mapErrors = {
            {QProcess::FailedToStart, "The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program."},
            {QProcess::Crashed, "The process crashed some time after starting successfully."},
            {QProcess::Timedout, "The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again."},
            {QProcess::WriteError, "An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel."},
            {QProcess::ReadError, "An error occurred when attempting to read from the process. For example, the process may not be running."},
            {QProcess::UnknownError, "An unknown error occurred. This is the default return value of error()."}
        };

        PrintLog(mapErrors[error]);
    });

    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          [=](int exitCode, QProcess::ExitStatus exitStatus){
        Q_UNUSED(exitCode);

        switch (exitStatus) {
        case QProcess::NormalExit:
        {
            ui->edtLog->appendPlainText("The process exited normally.");
            if (m_process.program().endsWith(g_strConvertorProgramName))
            {
                Compress2Cim();
            }
            else if (m_process.program().endsWith(g_strCompressProgramName))
            {
                QFileInfo fi(m_strFilePath);
				QString strDstDir = (m_strDstDir.isEmpty() ? fi.dir().path() : m_strDstDir);
                QString strFileName = fi.fileName();
                QString strTmpDstFilePath = strDstDir + "/" + strFileName + ".zip";
                QString strDstFilePath = strDstDir + "/" + strFileName + g_strDstFileExt;
                if (QFile::exists(strTmpDstFilePath))
                {
                    if (QFile::exists(strDstFilePath))
                    {
                        QFile::remove(strDstFilePath);
                    }

                    QFile::rename(strTmpDstFilePath, strDstFilePath);

					if (m_bNoUi)
					{
						qApp->exit(0);
					}
                    else
                    {
						QDesktopServices::openUrl(QUrl("file:///" + fi.dir().path()));
                    }
                }

            }
        }
            break;
        case QProcess::CrashExit:
            PrintLog("The process crashed.");
            break;
        default:
            break;
        }
    });

    connect(&m_process, &QProcess::readyReadStandardOutput, this, [&](){
        PrintLog(m_process.readAllStandardOutput());
    });

    connect(&m_process, &QProcess::readyReadStandardError, this, [&](){
        PrintLog(m_process.readAllStandardError());
    });

    connect(&m_process, &QProcess::started, this, [&](){
        PrintLog("the process has started.");
    });

    connect(&m_process, &QProcess::stateChanged, this, [&](QProcess::ProcessState newState){
        static QMap<QProcess::ProcessState, QString> mapState = {
            {QProcess::NotRunning, "The process is not running."},
            {QProcess::Starting, "The process is starting, but the program has not yet been invoked."},
            {QProcess::Running, "The process is running and is ready for reading and writing."}
        };

        PrintLog(mapState[newState]);
    });

    if (!m_bNoUi) ui->edtFilePath->setText(strFilePath);
}

void main_win::on_btnConvertor_clicked()
{
    QString strFilePath = ui->edtFilePath->text();
    if (strFilePath.isEmpty())
    {
        QMessageBox::warning(this, "warning", "please input the source file path.");
        return;
    }

    ConvertSkp2Cim(strFilePath);
}
