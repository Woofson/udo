#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QContextMenuEvent>
#include <QPoint>
#include <QMouseEvent>
#include <QTimer>
#include <QFrame>
#include <QUrl>
#include <QAuthenticator>
#include <QWebEngineDownloadRequest>
#include <QSet>
#include <QMap>
#include <QString>

// Forward declarations
class QWebEngineProfile;
class QTabWidget;
class QWebEngineView;
class DoBar;
class QStatusBar;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWebEngineProfile *profile, const QUrl &initialUrl = QUrl(), QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateTabBarVisibility(int count);
    void toggleDoBar();
    void handleCommand(const QString &command);
    void updateStatusBar(int count = 0);
    void handleTabChanged(int index);
    void handleLoadProgress(int progress);
    void handleShortcut(const QString &commandName);
    void showAboutDialog();
    void showHelpDialog();
    void updateClock();
    void handleDownloadRequested(QWebEngineDownloadRequest *download);
    void handleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator);

private:
    void createNewTab(const QUrl& url);
    QWebEngineView* currentWebView() const;
    QUrl prepareUrl(const QString &input);

    // Member Variables
    QWebEngineProfile *m_profile;
    QTabWidget *m_tabWidget;
    DoBar *m_doBar;
    QFrame *m_doBarFrame;
    QStatusBar *m_statusBar;
    QLabel *m_statusUrl;
    QLabel *m_statusSecure;
    QLabel *m_statusTabs;
    QLabel *m_statusClock;
    QTimer *m_clockTimer;
    int m_loadProgress;
    QPoint m_dragPosition;
    QMap<QString, int> m_authAttempts;
    QSet<QString> m_cancelledAuthHosts;
};

#endif // MAINWINDOW_H
