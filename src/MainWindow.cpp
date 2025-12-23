#include <QApplication>
#include "MainWindow.h"
#include "ConfigManager.h"
#include "DoBar.h"
#include "AboutDialog.h"
#include "HelpDialog.h"
#include "HttpAuthDialog.h"
#include <QTabWidget>
#include <QTabBar>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QWebEnginePage>
#include <QDebug>
#include <QStatusBar>
#include <QLabel>
#include <QShortcut>
#include <QMouseEvent>
#include <QDateTime>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QWebEngineHistory>
#include <QFileDialog>
#include <QFileInfo>
#include <QAuthenticator>
#include <QIcon>

MainWindow::MainWindow(QWebEngineProfile *profile, const QUrl &initialUrl, QWidget *parent)
    : QMainWindow(parent), m_profile(profile)
{
    m_loadProgress = 100;
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setWindowIcon(QIcon(":/assets/assets/icon.png"));

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->tabBar()->hide();
    setCentralWidget(m_tabWidget);

    m_standaloneTabBar = new QTabBar(this);
    m_standaloneTabBar->setTabsClosable(true);
    m_standaloneTabBar->setMovable(true);
    m_standaloneTabBar->setExpanding(false);
    m_standaloneTabBar->setVisible(ConfigManager::instance().tabBarVisibleByDefault());

    m_doBarFrame = new QFrame(this);
    m_doBarFrame->setObjectName("doBarFrame");
    m_doBarFrame->hide();
    
    m_doBar = new DoBar(m_doBarFrame);
    QVBoxLayout *frameLayout = new QVBoxLayout(m_doBarFrame);
    frameLayout->setContentsMargins(10, 10, 10, 10);
    frameLayout->addWidget(m_doBar);
    m_doBarFrame->setLayout(frameLayout);

    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);
    m_statusBar->setVisible(ConfigManager::instance().statusBarVisibleByDefault());
    m_statusUrl = new QLabel(this);
    m_statusSecure = new QLabel(this);
    m_statusTabs = new QLabel(this);
    m_statusClock = new QLabel(this);

    m_statusBar->addWidget(m_statusSecure);
    m_statusBar->addWidget(m_statusUrl, 1);
    m_statusBar->addPermanentWidget(m_statusTabs);
    m_statusBar->addPermanentWidget(m_statusClock);

    if (initialUrl.isValid()) {
        createNewTab(initialUrl);
    } else {
        createNewTab(QUrl(ConfigManager::instance().startPage()));
    }

    // --- Shortcuts ---
    QJsonObject shortcuts = ConfigManager::instance().shortcuts();
    auto addShortcut = [&](const QString &key, auto slot) {
        if (shortcuts.contains(key)) {
            QShortcut *sc = new QShortcut(QKeySequence(shortcuts[key].toString()), this);
            connect(sc, &QShortcut::activated, this, slot);
        }
    };

    addShortcut("toggle_do_bar", &MainWindow::toggleDoBar);
    addShortcut("toggle_status_bar", &MainWindow::toggleStatusBar);
    addShortcut("toggle_tab_bar", &MainWindow::toggleTabBar);
    addShortcut("quit", [this](){ QApplication::instance()->quit(); });
    addShortcut("new_tab", [this](){ handleCommand("!!!tn"); });
    addShortcut("about", &MainWindow::showAboutDialog);

    QShortcut *esc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(esc, &QShortcut::activated, this, [this](){
        if (m_doBarFrame->isVisible()) {
            m_doBarFrame->hide();
        }
    });

    QShortcut *f5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(f5, &QShortcut::activated, this, [this](){ if(currentWebView()) currentWebView()->reload(); });
    QShortcut *ctrlR = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this);
    connect(ctrlR, &QShortcut::activated, this, [this](){ if(currentWebView()) currentWebView()->reload(); });

    connect(m_doBar, &DoBar::commandEntered, this, &MainWindow::handleCommand);
    
    connect(m_tabWidget, &QTabWidget::currentChanged, m_standaloneTabBar, &QTabBar::setCurrentIndex);
    connect(m_standaloneTabBar, &QTabBar::currentChanged, m_tabWidget, &QTabWidget::setCurrentIndex);
    connect(m_standaloneTabBar, &QTabBar::tabCloseRequested, m_tabWidget, &QTabWidget::tabCloseRequested);

    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index){
        if (m_tabWidget->count() > 1) {
            m_tabWidget->widget(index)->deleteLater();
            m_tabWidget->removeTab(index);
            m_standaloneTabBar->removeTab(index);
            updateStatusBar();
            updateTabBarVisibility(m_tabWidget->count());
        } else {
            QApplication::instance()->quit();
        }
    });
    
    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    m_clockTimer->start(1000);
    updateClock();

    connect(m_profile, &QWebEngineProfile::downloadRequested, this, &MainWindow::handleDownloadRequested);
}

MainWindow::~MainWindow()
{
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move((event->globalPosition() - m_dragPosition).toPoint());
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    layoutUI();
}

void MainWindow::layoutUI()
{
    if (m_doBarFrame->isVisible()) {
        int barWidth = width() * 0.7;
        if (barWidth > 800) barWidth = 800;
        m_doBarFrame->setGeometry((width() - barWidth) / 2, 50, barWidth, m_doBarFrame->sizeHint().height());
    }

    if (m_standaloneTabBar->isVisible()) {
        QString pos = ConfigManager::instance().tabBarPosition();
        int tbHeight = m_standaloneTabBar->sizeHint().height();
        int y = 0;
        
        if (pos == "top") {
            y = 0;
        } else {
            y = height() - tbHeight;
            if (m_statusBar->isVisible()) {
                y -= m_statusBar->height();
            }
        }
        m_standaloneTabBar->setGeometry(0, y, width(), tbHeight);
        m_standaloneTabBar->raise();
    }
}

void MainWindow::toggleDoBar()
{
    m_doBarFrame->setVisible(!m_doBarFrame->isVisible());
    if (m_doBarFrame->isVisible()) {
        layoutUI();
        m_doBarFrame->raise();
        m_doBar->focusLineEdit();
    }
}

void MainWindow::toggleStatusBar()
{
    m_statusBar->setVisible(!m_statusBar->isVisible());
    layoutUI();
}

void MainWindow::toggleTabBar()
{
    m_standaloneTabBar->setVisible(!m_standaloneTabBar->isVisible());
    layoutUI();
}

void MainWindow::createNewTab(const QUrl& url)
{
    QWebEngineView *webView = new QWebEngineView(this);
    QWebEnginePage *page = new QWebEnginePage(m_profile, webView);
    webView->setPage(page);
    
    webView->setUrl(url);

    int index = m_tabWidget->addTab(webView, "New Tab");
    m_standaloneTabBar->addTab("New Tab");
    m_tabWidget->setCurrentIndex(index);

    connect(webView, &QWebEngineView::titleChanged, this, [this, index](const QString &title){
        m_tabWidget->setTabText(index, title);
        m_standaloneTabBar->setTabText(index, title);
    });

    connect(webView, &QWebEngineView::urlChanged, this, [this](){ this->updateStatusBar(); });
    connect(page, &QWebEnginePage::loadProgress, this, &MainWindow::handleLoadProgress);
    connect(page, &QWebEnginePage::loadFinished, this, [this](bool ok){
        Q_UNUSED(ok);
        m_loadProgress = 100;
        updateStatusBar();
        m_cancelledAuthHosts.clear();
        m_authAttempts.clear();
    });
    connect(page, &QWebEnginePage::authenticationRequired, this, &MainWindow::handleAuthenticationRequired);
    
    // Modern Permission API for video calls etc.
    connect(page, &QWebEnginePage::permissionRequested, this, &MainWindow::onPermissionRequested);
    
    updateStatusBar();
    updateTabBarVisibility(m_tabWidget->count());
}

void MainWindow::onPermissionRequested(QWebEnginePermission permission)
{
    // Auto-grant permissions for video calls (Messenger, Discord, etc.)
    qDebug() << "Auto-granting permission for:" << permission.permissionType();
    permission.grant();
}

void MainWindow::updateTabBarVisibility(int count)
{
    Q_UNUSED(count);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if (QWebEngineView *webView = currentWebView()) {
        QMenu menu(this);
        menu.addAction(webView->page()->action(QWebEnginePage::Back));
        menu.addAction(webView->page()->action(QWebEnginePage::Forward));
        menu.addAction(webView->page()->action(QWebEnginePage::Reload));
        menu.addSeparator();
        menu.addAction(webView->page()->action(QWebEnginePage::Stop));
        menu.exec(event->globalPos());
    }
}

QWebEngineView* MainWindow::currentWebView() const
{
    return qobject_cast<QWebEngineView*>(m_tabWidget->currentWidget());
}

QUrl MainWindow::prepareUrl(const QString &input)
{
    if (input.startsWith("/") || input.startsWith("./") || input.startsWith("../")) {
        return QUrl::fromLocalFile(QFileInfo(input).absoluteFilePath());
    }
    
    QUrl url = QUrl::fromUserInput(input);
    if (ConfigManager::instance().prioritizeHttps() && url.scheme() == "http") {
        url.setScheme("https");
    }
    return url;
}

void MainWindow::handleCommand(const QString &command)
{
    m_doBarFrame->hide();
    if (command.startsWith("!!!")) {
        QString cmdKey = command.mid(3).trimmed();
        QString cmd = ConfigManager::instance().tripleBangs().value(cmdKey).toString();

        if (cmd == "quit") {
            QApplication::instance()->quit();
        } else if (cmd == "tab_new") {
            createNewTab(QUrl(ConfigManager::instance().startPage()));
            updateStatusBar();
        } else if (cmd == "tab_close") {
            if (m_tabWidget->count() > 0) {
                m_tabWidget->tabCloseRequested(m_tabWidget->currentIndex());
            }
        } else if (cmd == "tab_next") {
            if (m_tabWidget->count() > 0) {
                int nextIndex = (m_tabWidget->currentIndex() + 1) % m_tabWidget->count();
                m_tabWidget->setCurrentIndex(nextIndex);
            }
        } else if (cmd == "tab_prev") {
            if (m_tabWidget->count() > 0) {
                int prevIndex = (m_tabWidget->currentIndex() - 1 + m_tabWidget->count()) % m_tabWidget->count();
                m_tabWidget->setCurrentIndex(prevIndex);
            }
        } else if (cmd == "about") {
            showAboutDialog();
        } else if (cmd == "help") {
            showHelpDialog();
        } else if (cmd == "maximize") {
            showMaximized();
        } else if (cmd == "minimize") {
            showMinimized();
        } else if (cmd == "normalize") {
            showNormal();
        } else if (cmd == "clear_history") {
            if(currentWebView()) {
                currentWebView()->history()->clear();
            }
        } else if (cmd == "clear_cookies") {
            m_profile->cookieStore()->deleteAllCookies();
        } else if (cmd == "clear_cache") {
            m_profile->clearHttpCache();
        }
    } else if (command.startsWith("!!")) {
        QString bookmark = command.mid(2).trimmed();
        QString url = ConfigManager::instance().doubleBangs().value(bookmark).toString();
        if (!url.isEmpty()) {
            currentWebView()->setUrl(prepareUrl(url));
        }
    } else if (command.startsWith("!")) {
        QStringList parts = command.split(" ");
        QString bang = parts.first().mid(1);
        QString query = parts.mid(1).join(" ");
        QString urlTemplate = ConfigManager::instance().bangs().value(bang).toString();
        if (!urlTemplate.isEmpty()) {
            urlTemplate.replace("%s", query);
            currentWebView()->setUrl(prepareUrl(urlTemplate));
        }
    } else {
        currentWebView()->setUrl(prepareUrl(command));
    }
}

void MainWindow::handleTabChanged(int index)
{
    m_loadProgress = 100;
    m_standaloneTabBar->setCurrentIndex(index);
    updateStatusBar();
}

void MainWindow::updateStatusBar(int count)
{
    Q_UNUSED(count);
    QWebEngineView *webView = currentWebView();
    if (webView) {
        if (m_loadProgress < 100) {
             m_statusUrl->setText(QString("Loading (%1%)...").arg(m_loadProgress));
        } else {
            m_statusUrl->setText(webView->url().toString().left(80));
        }
        bool isSecure = webView->url().scheme() == "https";
        m_statusSecure->setText(isSecure ? "🔒" : "🔓");
    } else {
        m_statusUrl->clear();
        m_statusSecure->clear();
    }
    m_statusTabs->setText(QString("Tab: %1/%2").arg(m_tabWidget->currentIndex() + 1).arg(m_tabWidget->count()));
}

void MainWindow::handleLoadProgress(int progress)
{
    m_loadProgress = progress;
    updateStatusBar();
}

void MainWindow::handleShortcut(const QString &commandName)
{
}

void MainWindow::showAboutDialog()
{
    AboutDialog about(this);
    about.exec();
}

void MainWindow::showHelpDialog()
{
    HelpDialog help(this);
    help.exec();
}

void MainWindow::updateClock()
{
    QString format = ConfigManager::instance().clockFormat();
    m_statusClock->setText(QDateTime::currentDateTime().toString(format));
}

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>

void MainWindow::handleDownloadRequested(QWebEngineDownloadRequest *download)
{
    QString defaultPath = ConfigManager::instance().downloadsPath();
    QString suggestedFileName = download->suggestedFileName();
    QString selectedFile = QFileDialog::getSaveFileName(this, tr("Save Download"),
                                                    QDir(defaultPath).filePath(suggestedFileName));
    if (selectedFile.isEmpty()) {
        download->cancel();
    } else {
        download->setDownloadDirectory(QFileInfo(selectedFile).path());
        download->setDownloadFileName(QFileInfo(selectedFile).fileName());
        download->accept();

        // Connect to signals to track progress and completion
        connect(download, &QWebEngineDownloadRequest::stateChanged, this, [this, download](QWebEngineDownloadRequest::DownloadState state){
            if (state == QWebEngineDownloadRequest::DownloadCompleted) {
                sendNotification("Download Complete", QString("Saved to: %1").arg(download->downloadFileName()));
            } else if (state == QWebEngineDownloadRequest::DownloadInterrupted) {
                sendNotification("Download Failed", QString("Failed to download: %1").arg(download->downloadFileName()));
            }
        });
    }
}

void MainWindow::sendNotification(const QString &title, const QString &body)
{
    QDBusInterface notificationInterface("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications");
    if (notificationInterface.isValid()) {
        QList<QVariant> args;
        args << "udo" // app_name
             << (unsigned int)0 // replaces_id
             << "" // app_icon (can be path to icon)
             << title // summary
             << body // body
             << QStringList() // actions
             << QVariantMap() // hints
             << -1; // expire_timeout (-1 = default)
             
        notificationInterface.callWithArgumentList(QDBus::AutoDetect, "Notify", args);
    }
}

void MainWindow::handleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator)
{
    const QString host = requestUrl.host();
    if (m_cancelledAuthHosts.contains(host)) {
        return;
    }
    QString errorMessage;
    if (m_authAttempts.contains(host)) {
        errorMessage = "Invalid credentials. Please try again.";
    }
    HttpAuthDialog authDialog(this, host, errorMessage);
    if (authDialog.exec() == QDialog::Accepted) {
        authenticator->setUser(authDialog.username());
        authenticator->setPassword(authDialog.password());
        m_authAttempts[host]++;
    } else {
        m_cancelledAuthHosts.insert(host);
        m_authAttempts.remove(host);
    }
}