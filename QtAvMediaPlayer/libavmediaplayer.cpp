#include "libavmediaplayer.h"
#include "guiplugin.h"
#include "pluginmanager.h"
#include "playerthread.h"
#include "core.h"

#include "QtAV/WidgetRenderer.h"
#include "QtAV/AVPlayer.h"
#include "QtAV/AudioOutput.h"
#include "QtAV/QtAV_Global.h"
#include <QtAV/AudioFormat.h>
#include <QtAV/AudioOutputOpenAL.h>

#include <QApplication>
#include <QList>
#include <QDebug>

using namespace yasem;
using namespace QtAV;

LibAvMediaPlayer::LibAvMediaPlayer()
{
    QtAV::setFFmpegLogHandler(NULL);
    fullscreen(true);
    //qInstallMessageHandler(customMessageHandler);
}

void LibAvMediaPlayer::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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

PLUGIN_ERROR_CODES LibAvMediaPlayer::initialize()
{
    STUB();
    gui = dynamic_cast<GuiPlugin*>(PluginManager::instance()->getByRole("gui"));
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

    connect(mediaPlayer, &QtAV::AVPlayer::paused,               &this->mediaSignalSender, &MediaSignalSender::paused);
    connect(mediaPlayer, &QtAV::AVPlayer::started,              &this->mediaSignalSender, &MediaSignalSender::started);
    connect(mediaPlayer, &QtAV::AVPlayer::stopped,              &this->mediaSignalSender, &MediaSignalSender::stopped);
    connect(mediaPlayer, &QtAV::AVPlayer::speedChanged,         &this->mediaSignalSender, &MediaSignalSender::speedChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::repeatChanged,        &this->mediaSignalSender, &MediaSignalSender::repeatChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::currentRepeatChanged, &this->mediaSignalSender, &MediaSignalSender::currentRepeatChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::startPositionChanged, &this->mediaSignalSender, &MediaSignalSender::startPositionChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::stopPositionChanged,  &this->mediaSignalSender, &MediaSignalSender::stopPositionChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::positionChanged,      &this->mediaSignalSender, &MediaSignalSender::positionChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::brightnessChanged,    &this->mediaSignalSender, &MediaSignalSender::brightnessChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::contrastChanged,      &this->mediaSignalSender, &MediaSignalSender::contrastChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::saturationChanged,    &this->mediaSignalSender, &MediaSignalSender::saturationChanged);


    return PLUGIN_ERROR_NO_ERROR;
}

PLUGIN_ERROR_CODES LibAvMediaPlayer::deinitialize()
{
    STUB();
    return PLUGIN_ERROR_NO_ERROR;
}

void LibAvMediaPlayer::parent(QWidget *parent)
{
    videoWidget->setParent(parent);
}

QWidget *LibAvMediaPlayer::parent()
{
    return videoWidget->parentWidget();
}

void LibAvMediaPlayer::widget(QWidget *videoWidget)
{
    //this->videoWidget = videoWidget;
}

QWidget *LibAvMediaPlayer::widget()
{
    return this->videoWidget;
}

bool LibAvMediaPlayer::mediaPlay(const QString &url)
{
    if(!processHooks(MediaPlayerPlugin::BEFORE_PLAY)) return false;

    if(playerThread->isRunning())
    {
        qDebug() << playerThread->objectName() << "is running. Terminating...";
        playerThread->terminate();
        playerThread->wait(100);
        qDebug() << "Video player thread terminated!";
    }

    //mediaPlayer->enableAudio(false);
    mediaPlayer->setFile(url);


    playerThread->start();
    videoWidget->show();

    return processHooks(MediaPlayerPlugin::BEFORE_PLAY);
}

bool LibAvMediaPlayer::mediaContinue()
{
    mediaPlayer->pause(false);
    return true;
}

bool LibAvMediaPlayer::mediaPause()
{
    mediaPlayer->pause(true);
    return true;
}

bool LibAvMediaPlayer::mediaStop()
{
    mediaPlayer->stop();
    return true;
}

bool LibAvMediaPlayer::mediaReset()
{
    return true;
}

void LibAvMediaPlayer::show()
{
    videoWidget->show();
}

void LibAvMediaPlayer::hide()
{
    videoWidget->hide();
}

void LibAvMediaPlayer::rect(const QRect &rect)
{
    STUB() << rect;
    videoWidget->setGeometry(rect);
}

QRect LibAvMediaPlayer::rect()
{
    STUB();
    return videoWidget->geometry();
}

bool LibAvMediaPlayer::isVisible()
{
    STUB();
    return videoWidget->isVisible();
}

MediaPlayingState LibAvMediaPlayer::state()
{
    return PlayingState;
}

bool LibAvMediaPlayer::state(MediaPlayingState state)
{
    STUB();
    return true;
}

void LibAvMediaPlayer::aspectRatio(ASPECT_RATIO mode)
{

}

ASPECT_RATIO LibAvMediaPlayer::aspectRatio()
{
    return ASPECT_RATIO_AUTO;
}

void LibAvMediaPlayer::move(int x, int y)
{
    videoWidget->move(x, y);
}

void LibAvMediaPlayer::raise()
{
    videoWidget->raise();
}

int LibAvMediaPlayer::audioPID()
{
    STUB();
    return 0;
}

int LibAvMediaPlayer::bufferLoad()
{
    STUB();
    return 100;
}

long LibAvMediaPlayer::mediaLength()
{
    STUB();
    return 0;
}

qint64 LibAvMediaPlayer::position()
{
    STUB();
    return mediaPlayer->position();
}

qint64 LibAvMediaPlayer::duration()
{
    return mediaPlayer->duration();
}


void LibAvMediaPlayer::audioPID(int pid)
{
    STUB();
}

int LibAvMediaPlayer::loop()
{
    STUB();
    return 0;
}

void LibAvMediaPlayer::loop(int loop)
{
    STUB();
}

bool LibAvMediaPlayer::mute()
{
    STUB();
    return mediaPlayer->isMute();
}

void LibAvMediaPlayer::mute(bool value)
{
    STUB();
    mediaPlayer->setMute(value);
}

void LibAvMediaPlayer::position(qint64 pos)
{
    STUB();
    mediaPlayer->setPosition(pos);
}

int LibAvMediaPlayer::volume()
{
    STUB();
    //Q_ASSERT(mediaPlayer->audio());
    //return (int)(mediaPlayer->audio()->volume() * 100);
    return 100;
}

void LibAvMediaPlayer::volume(int vol)
{
    STUB();
    //Q_ASSERT(mediaPlayer->audio());
    //mediaPlayer->audio()->setVolume(100 / vol);
}
