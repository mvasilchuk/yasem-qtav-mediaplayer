#include "qtavmediaplayer.h"
#include "guiplugin.h"
#include "pluginmanager.h"
#include "playerthread.h"
#include "core.h"

#include "QtAV/WidgetRenderer.h"
#include "QtAV/AVPlayer.h"
#include "QtAV/AudioOutput.h"
#include "QtAV/QtAV_Global.h"
#include <QtAV/AudioFormat.h>
#include <QtAV/AudioOutput.h>

#include <QApplication>
#include <QList>
#include <QDebug>

using namespace yasem;
using namespace QtAV;

QtAvMediaPlayer::QtAvMediaPlayer()
{
    QtAV::setFFmpegLogHandler(NULL);
    fullscreen(true);
    //qInstallMessageHandler(customMessageHandler);
}

void QtAvMediaPlayer::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;

    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    //QFile outFile("debuglog.txt");
    //outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    //QTextStream ts(&outFile);
    //ts << txt << endl;
}

PLUGIN_ERROR_CODES QtAvMediaPlayer::initialize()
{
    STUB();
    gui = dynamic_cast<GuiPlugin*>(PluginManager::instance()->getByRole(ROLE_GUI));
    videoWidget = new QtAV::WidgetRenderer();
    mediaPlayer = new QtAV::AVPlayer();
    playerThread = new PlayerThread(mediaPlayer);
    playerThread->setObjectName("QtAV Player thread");
    mediaPlayer->moveToThread(playerThread);

    AudioOutputFactory factory;
    QStringList names;
    std::vector<std::string> vNames = factory.registeredNames();
    std::vector<std::string>::iterator iter;
    for(iter = vNames.begin(); iter != vNames.end(); iter++)
    {
        names.append(QString::fromStdString(*iter));
    }

    mediaPlayer->setRenderer(videoWidget);

    /**
    void mediaStatusChanged(QtAV::MediaStatus status); //explictly use QtAV::MediaStatus
    void error(const QtAV::AVError& e); //explictly use QtAV::AVError in connection for Qt4 syntax
    void paused(bool p);
    void started();
    void stopped();
    void speedChanged(qreal speed);
    void repeatChanged(int r);
    void currentRepeatChanged(int r);
    void startPositionChanged(qint64 position);
    void stopPositionChanged(qint64 position);
    void positionChanged(qint64 position);
    void brightnessChanged(int val);
    void contrastChanged(int val);
    void saturationChanged(int val);
      */



#ifdef Q_OS_WIN
    //FIXME() << "Media player signals not connected under Windows!";
    // New signal/slot connection doesn't work under windows  because signal defines two times
    connect(mediaPlayer, SIGNAL(paused(bool)),                    &this->mediaSignalSender, SIGNAL(paused(bool)));
    connect(mediaPlayer, SIGNAL(started()),                       &this->mediaSignalSender, SIGNAL(started()));
    connect(mediaPlayer, SIGNAL(stopped()),                       &this->mediaSignalSender, SIGNAL(stopped()));
    connect(mediaPlayer, SIGNAL(speedChanged(qreal)),             &this->mediaSignalSender, SIGNAL(speedChanged(qreal)));
    connect(mediaPlayer, SIGNAL(repeatChanged(int)),              &this->mediaSignalSender, SIGNAL(repeatChanged(int)));
    connect(mediaPlayer, SIGNAL(currentRepeatChanged(int)),       &this->mediaSignalSender, SIGNAL(currentRepeatChanged(int)));
    connect(mediaPlayer, SIGNAL(startPositionChanged(qint64)),    &this->mediaSignalSender, SIGNAL(startPositionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(stopPositionChanged(qint64)),     &this->mediaSignalSender, SIGNAL(stopPositionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)),         &this->mediaSignalSender, SIGNAL(positionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(brightnessChanged(int)),          &this->mediaSignalSender, SIGNAL(brightnessChanged(int)));
    connect(mediaPlayer, SIGNAL(contrastChanged(int)),            &this->mediaSignalSender, SIGNAL(contrastChanged(int)));
    connect(mediaPlayer, SIGNAL(saturationChanged(int)),          &this->mediaSignalSender, SIGNAL(saturationChanged(int)));
#else
    connect(mediaPlayer, &AVPlayer::paused,               &this->mediaSignalSender, &MediaSignalSender::paused);
    connect(mediaPlayer, &AVPlayer::started,              &this->mediaSignalSender, &MediaSignalSender::started);
    connect(mediaPlayer, &AVPlayer::stopped,              &this->mediaSignalSender, &MediaSignalSender::stopped);
    connect(mediaPlayer, &AVPlayer::speedChanged,         &this->mediaSignalSender, &MediaSignalSender::speedChanged);
    connect(mediaPlayer, &AVPlayer::repeatChanged,        &this->mediaSignalSender, &MediaSignalSender::repeatChanged);
    connect(mediaPlayer, &AVPlayer::currentRepeatChanged, &this->mediaSignalSender, &MediaSignalSender::currentRepeatChanged);
    connect(mediaPlayer, &AVPlayer::startPositionChanged, &this->mediaSignalSender, &MediaSignalSender::startPositionChanged);
    connect(mediaPlayer, &AVPlayer::stopPositionChanged,  &this->mediaSignalSender, &MediaSignalSender::stopPositionChanged);
    connect(mediaPlayer, &AVPlayer::positionChanged,      &this->mediaSignalSender, &MediaSignalSender::positionChanged);
    connect(mediaPlayer, &AVPlayer::brightnessChanged,    &this->mediaSignalSender, &MediaSignalSender::brightnessChanged);
    connect(mediaPlayer, &AVPlayer::contrastChanged,      &this->mediaSignalSender, &MediaSignalSender::contrastChanged);
    connect(mediaPlayer, &AVPlayer::saturationChanged,    &this->mediaSignalSender, &MediaSignalSender::saturationChanged);
#endif //Q_OS_WIN32

    return PLUGIN_ERROR_NO_ERROR;
}

PLUGIN_ERROR_CODES QtAvMediaPlayer::deinitialize()
{
    STUB();
    return PLUGIN_ERROR_NO_ERROR;
}

void QtAvMediaPlayer::parent(QWidget *parent)
{
    videoWidget->setParent(parent);
}

QWidget *QtAvMediaPlayer::parent()
{
    return videoWidget->parentWidget();
}

void QtAvMediaPlayer::widget(QWidget *videoWidget)
{
    //this->videoWidget = videoWidget;
}

QWidget *QtAvMediaPlayer::widget()
{
    return this->videoWidget;
}

bool QtAvMediaPlayer::mediaPlay(const QString &url)
{
    if(!processHooks(MediaPlayerPlugin::BEFORE_PLAY)) return false;

    if(playerThread->isRunning())
    {
        qDebug() << playerThread->objectName() << "is running. Terminating...";
        playerThread->terminate();
        playerThread->wait(100);
        qDebug() << "Video player thread terminated!";
    }

    mediaPlayer->setFile(url);
    playerThread->start();
    videoWidget->show();

    return processHooks(MediaPlayerPlugin::BEFORE_PLAY);
}

bool QtAvMediaPlayer::mediaContinue()
{
    mediaPlayer->pause(false);
    return true;
}

bool QtAvMediaPlayer::mediaPause()
{
    mediaPlayer->pause(true);
    return true;
}

bool QtAvMediaPlayer::mediaStop()
{
    mediaPlayer->stop();
    return true;
}

bool QtAvMediaPlayer::mediaReset()
{
    return true;
}

void QtAvMediaPlayer::show()
{
    videoWidget->show();
}

void QtAvMediaPlayer::hide()
{
    videoWidget->hide();
}

void QtAvMediaPlayer::rect(const QRect &rect)
{
    videoWidget->setGeometry(rect);
}

QRect QtAvMediaPlayer::rect()
{
    STUB();
    return videoWidget->geometry();
}

bool QtAvMediaPlayer::isVisible()
{
    STUB();
    return videoWidget->isVisible();
}

MediaPlayingState QtAvMediaPlayer::state()
{
    return PlayingState;
}

bool QtAvMediaPlayer::state(MediaPlayingState state)
{
    STUB() << state;
    return true;
}

void QtAvMediaPlayer::aspectRatio(ASPECT_RATIO mode)
{
    STUB() << mode;
}

ASPECT_RATIO QtAvMediaPlayer::aspectRatio()
{
    return ASPECT_RATIO_AUTO;
}

void QtAvMediaPlayer::move(int x, int y)
{
    videoWidget->move(x, y);
}

void QtAvMediaPlayer::raise()
{
    videoWidget->raise();
}

int QtAvMediaPlayer::audioPID()
{
    STUB();
    return 0;
}

int QtAvMediaPlayer::bufferLoad()
{
    STUB();
    return 100;
}

qint64 QtAvMediaPlayer::position()
{
    qint64 pos = mediaPlayer->position();
    return pos;
}

qint64 QtAvMediaPlayer::duration()
{
    STUB();
    qint64 duration = mediaPlayer->duration();
    return duration;
}


void QtAvMediaPlayer::audioPID(int pid)
{
    STUB() << pid;
}

int QtAvMediaPlayer::loop()
{
    return mediaPlayer->repeat();
}

void QtAvMediaPlayer::loop(int loop)
{
    mediaPlayer->setRepeat(loop);
}

bool QtAvMediaPlayer::mute()
{
    return mediaPlayer->isMute();
}

void QtAvMediaPlayer::mute(bool value)
{
    mediaPlayer->setMute(value);
}

void QtAvMediaPlayer::position(qint64 pos)
{
    mediaPlayer->setPosition(pos);
}

int QtAvMediaPlayer::volume()
{
    STUB();
    AudioOutput* audio = mediaPlayer->audio();
    if(audio != NULL)
    {
        return (int)(audio->volume() * 100);
    }
    return 100;
}

void QtAvMediaPlayer::volume(int vol)
{
    STUB() << vol;
    AudioOutput* audio = mediaPlayer->audio();
    if(audio != NULL)
    {
        if(vol > 0)
            audio->setVolume(100 / vol);
        else
            audio->setVolume(0);
    }
}


void yasem::QtAvMediaPlayer::register_dependencies()
{
}

void yasem::QtAvMediaPlayer::register_roles()
{
    register_role(ROLE_MEDIA);
}
