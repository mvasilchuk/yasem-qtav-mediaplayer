#include "qtavmediaplayerobject.h"
#include "guipluginobject.h"
#include "pluginmanager.h"
#include "core.h"

#include "QtAVWidgets/WidgetRenderer.h"
#include "QtAV/AVPlayer.h"
#include "QtAV/AudioOutput.h"
#include "QtAV/QtAV_Global.h"
#include <QtAV/AudioFormat.h>
#include <QtAV/AudioOutput.h>

#include <QApplication>
#include <QList>
#include <QDebug>
#include <QMetaProperty>

using namespace yasem;
using namespace QtAV;

QtAVMediaPlayerObject::QtAVMediaPlayerObject(Plugin* plugin):
    MediaPlayerPluginObject(plugin),
    m_aspect_ratio(ASPECT_RATIO_16_9)
{
    QtAV::setFFmpegLogHandler(NULL);
    fullscreen(true);
}

QtAVMediaPlayerObject::~QtAVMediaPlayerObject()
{

}

void QtAVMediaPlayerObject::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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

void QtAVMediaPlayerObject::onMediaStatusChanged(QtAV::MediaStatus status)
{
    switch(status)
    {
        case QtAV::MediaStatus::UnknownMediaStatus: {
            emit statusChanged(MediaStatus::UnknownMediaStatus);
            break;
        }
        case QtAV::MediaStatus::NoMedia: {
            emit statusChanged(MediaStatus::NoMedia);
            break;
        }
        case QtAV::MediaStatus::LoadingMedia: {
            emit statusChanged(MediaStatus::LoadingMedia);
            break;
        }
        case QtAV::MediaStatus::StalledMedia: {
            emit statusChanged(MediaStatus::StalledMedia);
            break;
        }
        case QtAV::MediaStatus::BufferingMedia: {
            emit statusChanged(MediaStatus::BufferingMedia);
            break;
        }
        case QtAV::MediaStatus::BufferedMedia: {
            emit statusChanged(MediaStatus::BufferedMedia);
            break;
        }
        case QtAV::MediaStatus::EndOfMedia: {
            emit statusChanged(MediaStatus::EndOfMedia);
            break;
        }
        case QtAV::MediaStatus::InvalidMedia: {
            emit statusChanged(MediaStatus::InvalidMedia);
            break;
        }
        default: {
            break;
        }

    }
}

void QtAVMediaPlayerObject::parent(QWidget *parent)
{
    videoWidget->setParent(parent);
}

QWidget *QtAVMediaPlayerObject::parent()
{
    return videoWidget->parentWidget();
}

void QtAVMediaPlayerObject::widget(QWidget *videoWidget)
{
    //this->videoWidget = videoWidget;
}

QWidget *QtAVMediaPlayerObject::widget()
{
    return this->videoWidget;
}

bool QtAVMediaPlayerObject::mediaPlay(const QString &url)
{
    if(!processHooks(MediaPlayerPluginObject::BEFORE_PLAY)) return false;

    mediaPlayer->play(url);
    videoWidget->show();
    aspectRatio(m_aspect_ratio);

    return processHooks(MediaPlayerPluginObject::AFTER_PLAY);
}

bool QtAVMediaPlayerObject::mediaContinue()
{
    mediaPlayer->pause(false);
    return true;
}

bool QtAVMediaPlayerObject::mediaPause()
{
    mediaPlayer->pause(true);
    return true;
}

bool QtAVMediaPlayerObject::mediaStop()
{
    mediaPlayer->stop();
    return true;
}

bool QtAVMediaPlayerObject::mediaReset()
{
    return true;
}

void QtAVMediaPlayerObject::show()
{
    videoWidget->show();
}

void QtAVMediaPlayerObject::hide()
{
    videoWidget->hide();
}

void QtAVMediaPlayerObject::rect(const QRect &rect)
{
    videoWidget->setGeometry(rect);
}

QRect QtAVMediaPlayerObject::rect()
{
    STUB();
    return videoWidget->geometry();
}

bool QtAVMediaPlayerObject::isVisible()
{
    STUB();
    return videoWidget->isVisible();
}

MediaPlayingState QtAVMediaPlayerObject::state()
{
    return PlayingState;
}

bool QtAVMediaPlayerObject::state(MediaPlayingState state)
{
    STUB() << state;
    return true;
}

void QtAVMediaPlayerObject::aspectRatio(AspectRatio ratio)
{
    STUB() << ratio;

    m_aspect_ratio = ratio;

    QtAV::VideoRenderer* renderer = mediaPlayer->renderer();

    if(m_aspect_ratio == ASPECT_RATIO_AUTO)
    {
        renderer->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
        return;
    }

    renderer->setOutAspectRatioMode(QtAV::VideoRenderer::CustomAspectRation);
    float a_ratio = 1;
    switch(m_aspect_ratio)
    {
        case ASPECT_RATIO_1_1: {
            a_ratio = 1;
            break;
        }
        case ASPECT_RATIO_5_4: {
            a_ratio = 16.0 / 9;
            break;
        }
        case ASPECT_RATIO_4_3: {
            a_ratio = 4.0 / 3;
            break;
        }
        case ASPECT_RATIO_11_8: {
            a_ratio = 11.0 / 8;
            break;
        }
        case ASPECT_RATIO_14_10: {
            a_ratio = 14.0 / 10;
            break;
        }
        case ASPECT_RATIO_3_2: {
            a_ratio = 3.0 / 2;
            break;
        }
        case ASPECT_RATIO_14_9: {
            a_ratio = 14.0 / 9;
            break;
        }
        case ASPECT_RATIO_16_10: {
            a_ratio = 16.0 / 10;
            break;
        }

        case ASPECT_RATIO_16_9: {
            a_ratio = 16.0 / 9;
            break;
        }
        case ASPECT_RATIO_2_35_1: {
            a_ratio = 2.35 / 1;
            break;
        }
        default: {
            a_ratio = 16.0 / 9;
            break;
        }
    }
    renderer->setOutAspectRatio(a_ratio);
}

AspectRatio QtAVMediaPlayerObject::aspectRatio()
{
    return (AspectRatio)m_aspect_ratio;
}

void QtAVMediaPlayerObject::move(int x, int y)
{
    videoWidget->move(x, y);
}

void QtAVMediaPlayerObject::raise()
{
    videoWidget->raise();
    //resize();
}

int QtAVMediaPlayerObject::audioPID()
{
    STUB();
    return mediaPlayer->currentAudioStream();
}

int QtAVMediaPlayerObject::bufferLoad()
{
    STUB();
    return 100;
}

qint64 QtAVMediaPlayerObject::position()
{
    qint64 pos = mediaPlayer->position();
    return pos;
}

qint64 QtAVMediaPlayerObject::duration()
{
    STUB();
    qint64 duration = mediaPlayer->duration();
    return duration;
}

int QtAVMediaPlayerObject::getBrightness()
{
    return mediaPlayer->brightness();
}

int QtAVMediaPlayerObject::getContrast()
{
    return mediaPlayer->contrast();
}


void QtAVMediaPlayerObject::audioPID(int pid)
{
    STUB() << pid;
    mediaPlayer->setAudioStream(pid);
}

QList<AudioLangInfo> QtAVMediaPlayerObject::getAudioLanguages()
{
    QList<AudioLangInfo> languages;
    int count = mediaPlayer->audioStreamCount();
    for(int index = 0; index < count; index++)
    {
        languages.append(AudioLangInfo(index, QString::number(index), QString::number(index)));
    }
    return languages;
}

void QtAVMediaPlayerObject::setAudioLanguage(int index)
{
    mediaPlayer->setAudioStream(index);
}

int QtAVMediaPlayerObject::loop()
{
    return mediaPlayer->repeat();
}

void QtAVMediaPlayerObject::loop(int loop)
{
    mediaPlayer->setRepeat(loop);
}

bool QtAVMediaPlayerObject::mute()
{
    return mediaPlayer->isMute();
}

void QtAVMediaPlayerObject::mute(bool value)
{
    mediaPlayer->setMute(value);
}

void QtAVMediaPlayerObject::position(qint64 pos)
{
    mediaPlayer->setPosition(pos);
}

int QtAVMediaPlayerObject::volume()
{
    STUB();
    AudioOutput* audio = mediaPlayer->audio();
    if(audio != NULL)
    {
        return (int)(audio->volume() * 100);
    }
    return 100;
}

void QtAVMediaPlayerObject::volume(int vol)
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

MediaMetadata QtAVMediaPlayerObject::getMediaMetadata()
{
    MediaMetadata metadata;
    metadata.filename = mediaPlayer->file();

    return metadata;
}







PluginObjectResult yasem::QtAVMediaPlayerObject::init()
{
    STUB();
    gui = dynamic_cast<GuiPluginObject*>(PluginManager::instance()->getByRole(ROLE_GUI));
    videoWidget = new QtAV::WidgetRenderer();
    mediaPlayer = new QtAV::AVPlayer();
    mediaPlayer->setAsyncLoad(true);
    mediaPlayer->setInterruptTimeout(10000);
    mediaPlayer->setRenderer(videoWidget);

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
    connect(mediaPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),  this, SLOT(onMediaStatusChanged(QtAV::MediaStatus)));
#else
    connect(mediaPlayer, &AVPlayer::paused,               this, &QtAVMediaPlayerObject::paused);
    connect(mediaPlayer, &AVPlayer::started,              this, &QtAVMediaPlayerObject::started);
    connect(mediaPlayer, &AVPlayer::stopped,              this, &QtAVMediaPlayerObject::stopped);
    connect(mediaPlayer, &AVPlayer::speedChanged,         this, &QtAVMediaPlayerObject::speedChanged);
    connect(mediaPlayer, &AVPlayer::repeatChanged,        this, &QtAVMediaPlayerObject::repeatChanged);
    connect(mediaPlayer, &AVPlayer::currentRepeatChanged, this, &QtAVMediaPlayerObject::currentRepeatChanged);
    connect(mediaPlayer, &AVPlayer::startPositionChanged, this, &QtAVMediaPlayerObject::startPositionChanged);
    connect(mediaPlayer, &AVPlayer::stopPositionChanged,  this, &QtAVMediaPlayerObject::stopPositionChanged);
    connect(mediaPlayer, &AVPlayer::positionChanged,      this, &QtAVMediaPlayerObject::positionChanged);
    connect(mediaPlayer, &AVPlayer::brightnessChanged,    this, &QtAVMediaPlayerObject::brightnessChanged);
    connect(mediaPlayer, &AVPlayer::contrastChanged,      this, &QtAVMediaPlayerObject::contrastChanged);
    connect(mediaPlayer, &AVPlayer::saturationChanged,    this, &QtAVMediaPlayerObject::saturationChanged);
    connect(mediaPlayer, &AVPlayer::mediaStatusChanged,   this, &QtAVMediaPlayerObject::onMediaStatusChanged);
#endif //Q_OS_WIN32

    return PLUGIN_OBJECT_RESULT_OK;
}

PluginObjectResult yasem::QtAVMediaPlayerObject::deinit()
{
    return PLUGIN_OBJECT_RESULT_OK;
}
