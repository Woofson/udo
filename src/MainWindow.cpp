#include <QApplication> // For QApplication::instance()
#include "MainWindow.h"
#include "ConfigManager.h"
#include "DoBar.h"
#include "AboutDialog.h"
#include "HelpDialog.h"
#include "HttpAuthDialog.h"
#include <QTabWidget>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QTabBar>
#include <QAction>
#include <QMenu>
#include <QWebEnginePage>
#include <QDebug>
#include <QStatusBar>
#include <QLabel>
#include <QShortcut>
#include <QMouseEvent>
#include <QDateTime> // For clock
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QWebEngineHistory>
#include <QFileDialog> // For Save As dialog
#include <QFileInfo>   // For file info
#include <QAuthenticator>

MainWindow::MainWindow(QWebEngineProfile *profile, const QUrl &initialUrl, QWidget *parent)
    : QMainWindow(parent), m_profile(profile)
{
    m_loadProgress = 100;
    setWindowFlags(Qt::FramelessWindowHint);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->tabBar()->hide();
    setCentralWidget(m_tabWidget);

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


    QJsonObject shortcuts = ConfigManager::instance().shortcuts();
    for (auto it = shortcuts.begin(); it != shortcuts.end(); ++it) {
        QString commandName = it.key();
        QString keySequence = it.value().toString();
        if (commandName == "toggle_do_bar" || commandName == "toggle_status_bar" || commandName == "about") {
            continue;
        }
        QShortcut *shortcut = new QShortcut(QKeySequence(keySequence), this);
        connect(shortcut, &QShortcut::activated, this, [this, commandName](){
            handleShortcut(commandName);
        });
    }

    QAction *doBarAction = new QAction(this);
    doBarAction->setShortcut(QKeySequence(ConfigManager::instance().shortcuts().value("toggle_do_bar").toString()));
    addAction(doBarAction);
    connect(doBarAction, &QAction::triggered, this, &MainWindow::toggleDoBar);

    QAction *statusBarAction = new QAction(this);
    statusBarAction->setShortcut(QKeySequence(ConfigManager::instance().shortcuts().value("toggle_status_bar").toString()));
    addAction(statusBarAction);
    connect(statusBarAction, &QAction::triggered, this, [this](){
        m_statusBar->setVisible(!m_statusBar->isVisible());
    });
    
    QAction *aboutAction = new QAction(this);
    aboutAction->setShortcut(QKeySequence(ConfigManager::instance().shortcuts().value("about").toString()));
    addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);

    // Add standard refresh shortcuts
    QAction *refreshAction = new QAction(this);
    refreshAction->setShortcuts({QKeySequence(Qt::Key_F5), QKeySequence(Qt::CTRL | Qt::Key_R)});
    addAction(refreshAction);
    connect(refreshAction, &QAction::triggered, this, [this](){
        if (auto webView = currentWebView()) {
            webView->reload();
        }
    });


    connect(m_doBar, &DoBar::commandEntered, this, &MainWindow::handleCommand);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::handleTabChanged);
    connect(m_tabWidget, SIGNAL(countChanged(int)), this, SLOT(updateTabBarVisibility(int)));

    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index){
        if (m_tabWidget->count() > 1) {
            m_tabWidget->widget(index)->deleteLater();
            m_tabWidget->removeTab(index);
            updateStatusBar();
        } else {
            close();
        }
    });
    
    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    m_clockTimer->start(1000);
    updateClock();

    handleTabChanged(0);
    updateStatusBar();

    connect(m_profile, &QWebEngineProfile::downloadRequested, this, &MainWindow::handleDownloadRequested);
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow destructor called. Application shutting down.";
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void MainWindow::toggleDoBar()
{
    if (m_doBarFrame->isVisible()) {
        m_doBarFrame->hide();
    } else {
        int barWidth = width() * 0.7;
        if (barWidth > 800) barWidth = 800;
        m_doBarFrame->setGeometry((width() - barWidth) / 2, 20, barWidth, m_doBarFrame->sizeHint().height());
        m_doBarFrame->show();
        m_doBarFrame->raise();
        m_doBar->focusLineEdit();
    }
}

void MainWindow::createNewTab(const QUrl& url)
{
    QWebEngineView *webView = new QWebEngineView();
    QWebEnginePage *page = new QWebEnginePage(m_profile, webView);
    webView->setPage(page);
    webView->setUrl(url);
    int index = m_tabWidget->addTab(webView, "New Tab");
    m_tabWidget->setCurrentIndex(index);

    connect(webView, &QWebEngineView::titleChanged, this, [this, index](const QString &title){
        m_tabWidget->setTabText(index, title);
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
    updateStatusBar();
}

void MainWindow::updateTabBarVisibility(int count)
{
    m_tabWidget->tabBar()->setVisible(count > 1);
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
    QUrl url = QUrl::fromUserInput(input);
    if (ConfigManager::instance().prioritizeHttps() && url.scheme() == "http") {
        url.setScheme("https");
    }
    return url;
}

void MainWindow::handleCommand(const QString &command)
{
    m_doBarFrame->hide();
    qDebug() << "Handling command:" << command;

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
                m_tabWidget->widget(m_tabWidget->currentIndex())->deleteLater();
                m_tabWidget->removeTab(m_tabWidget->currentIndex());
                updateStatusBar();
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
            qDebug() << "Current tab's history cleared.";
        } else if (cmd == "clear_cookies") {
            m_profile->cookieStore()->deleteAllCookies();
            qDebug() << "All cookies cleared.";
        } else if (cmd == "clear_cache") {
            m_profile->clearHttpCache();
            qDebug() << "HTTP cache cleared.";
        }
        else {
            qDebug() << "Unknown internal command:" << cmd;
        }

    } else if (command.startsWith("!!")) {
        QString bookmark = command.mid(2).trimmed();
        QString url = ConfigManager::instance().doubleBangs().value(bookmark).toString();
        if (!url.isEmpty()) {
            currentWebView()->setUrl(prepareUrl(url));
        } else {
            qDebug() << "Unknown bookmark:" << bookmark;
        }

    } else if (command.startsWith("!")) {
        QStringList parts = command.split(" ");
        QString bang = parts.first().mid(1);
        QString query = parts.mid(1).join(" ");
        QString urlTemplate = ConfigManager::instance().bangs().value(bang).toString();

        if (!urlTemplate.isEmpty()) {
            urlTemplate.replace("%s", query);
            currentWebView()->setUrl(prepareUrl(urlTemplate));
        } else {
            qDebug() << "Unknown bang:" << bang;
        }

    } else {
        currentWebView()->setUrl(prepareUrl(command));
    }
}

void MainWindow::handleTabChanged(int index)
{
    m_loadProgress = 100;
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
    if (commandName == "quit") {
        handleCommand("!!!q");
    } else if (commandName == "new_tab") {
        handleCommand("!!!tn");
    } else if (commandName == "about") {
        showAboutDialog();
    }
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

void MainWindow::handleDownloadRequested(QWebEngineDownloadRequest *download)
{
    QString defaultPath = ConfigManager::instance().downloadsPath();
    QString suggestedFileName = download->suggestedFileName();

    QString selectedFile = QFileDialog::getSaveFileName(this, tr("Save Download"),
                                                    QDir(defaultPath).filePath(suggestedFileName));

    if (selectedFile.isEmpty()) {
        download->cancel();
        qDebug() << "Download cancelled by user.";
    } else {
        download->setDownloadDirectory(QFileInfo(selectedFile).path());
        download->setDownloadFileName(QFileInfo(selectedFile).fileName());
        download->accept();
        qDebug() << "Download accepted to:" << selectedFile;
    }
}

void MainWindow::handleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator)
{
    const QString host = requestUrl.host();
    if (m_cancelledAuthHosts.contains(host)) {
        qDebug() << "Authentication for" << host << "already cancelled. Denying.";
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
        qDebug() << "Authentication cancelled by user for" << host;
        m_cancelledAuthHosts.insert(host);
        m_authAttempts.remove(host);
    }
}
