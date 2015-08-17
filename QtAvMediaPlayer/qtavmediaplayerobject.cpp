#include "qtavmediaplayerobject.h"
#include "gui.h"
#include "pluginmanager.h"
#include "core.h"
#include "yasemsettings.h"
#include "configuration_items.h"

#include "QtAV/AVPlayer.h"
#include "QtAV/AudioOutput.h"
#include "QtAV/QtAV_Global.h"
#include <QtAV/AudioFormat.h>
#include <QtAV/AudioOutput.h>
#include <QtAV/VideoRendererTypes.h>
#include "QtAVWidgets/OpenGLWidgetRenderer.h"
#include <QtAVWidgets/WidgetRenderer.h>

#include <QApplication>
#include <QList>
#include <QDebug>
#include <QMetaProperty>

using namespace yasem;
using namespace QtAV;

QtAVMediaPlayerObject::QtAVMediaPlayerObject(SDK::Plugin* plugin):
    SDK::MediaPlayer(plugin),
    m_media_player(NULL),
    m_aspect_ratio(SDK::ASPECT_RATIO_AUTO)
{
    m_support_opengl = true;
    QtAV::setFFmpegLogHandler(NULL);
    setFullscreen(true);
    initSettings();
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

void QtAVMediaPlayerObject::initSettings()
{
    m_qtav_settings = new SDK::ConfigTreeGroup("qtav-player", "qtav", tr("QtAV"));
    m_qtav_settings->setEnabled(false);
    SDK::ConfigTreeGroup* qtav_video = new SDK::ConfigTreeGroup("video", tr("Video settings"));
    SDK::ConfigTreeGroup* qtav_audio = new SDK::ConfigTreeGroup("audio", tr("Audio settings"));

    /*QString default_video_output = QString::fromStdString(VideoRendererFactory::name(QtAV::VideoRendererId_Widget));
    ConfigItem* video_output = new ConfigItem("output", tr("Video output"), default_video_output, ConfigItem::LIST);
    for(const std::string &name: VideoRendererFactory::registeredNames())
    {
        QString s_name = QString::fromStdString(name);
        QString title = s_name;
        if(title == default_video_output)
            title.append(tr(" (default)"));
        video_output->options().insert(s_name, title);
    }
    qtav_video->addItem(video_output);*/

    QString default_decoder_name = QString::fromStdString(VideoDecoderFactory::name(QtAV::VideoDecoderId_FFmpeg));
    SDK::ListConfigItem* video_decoder_priority = new SDK::ListConfigItem("decoder", tr("Decoder"), default_decoder_name);

    // TODO: fix exception in commented code (debug mode only)
    /*
    std::vector<std::string> vec = VideoDecoderFactory::registeredNames();
    for(const std::string name:  vec)
    {
        const QString s_name = QString::fromStdString(name);
        QString title = s_name;
        if(title == default_decoder_name)
            title.append(tr(" (default)"));
        video_decoder_priority->options().insert(title, s_name);
    }*/

    qtav_video->addItem(video_decoder_priority);
    connect(video_decoder_priority, &SDK::ConfigItem::saved, [=]() {
        QString decoder = video_decoder_priority->getValue().toString();
        DEBUG() << "setting a new decoder" << decoder;
        if(m_media_player)
            m_media_player->setPriority(QVector<VideoDecoderId>() << VideoDecoderFactory::id(decoder.toStdString()));
    });

    m_qtav_settings->addItem(qtav_video);
    m_qtav_settings->addItem(qtav_audio);

    settings()->findItem(SETTINGS_GROUP_PLUGINS)->addItem(m_qtav_settings);
    settings()->load();
}

/**
 * @brief QtAVMediaPlayerObject::openGlWidget
 * Only to check if OpenGL enabled!
 * @return
 */
QOpenGLWidget *QtAVMediaPlayerObject::openGlWidget() const
{
    return dynamic_cast<QOpenGLWidget*>(videoWidget);
}

void QtAVMediaPlayerObject::onMediaStatusChanged(QtAV::MediaStatus status)
{
    switch(status)
    {
        case QtAV::MediaStatus::UnknownMediaStatus: {
            emit statusChanged(SDK::MediaStatus::UnknownMediaStatus);
            break;
        }
        case QtAV::MediaStatus::NoMedia: {
            emit statusChanged(SDK::MediaStatus::NoMedia);
            break;
        }
        case QtAV::MediaStatus::LoadingMedia: {
            emit statusChanged(SDK::MediaStatus::LoadingMedia);
            emit statusChanged(SDK::MediaStatus::VideoInfoReceived); // TODO: Should be in another place
            break;
        }
        case QtAV::MediaStatus::StalledMedia: {
            emit statusChanged(SDK::MediaStatus::StalledMedia);
            break;
        }
        case QtAV::MediaStatus::BufferingMedia: {
            emit statusChanged(SDK::MediaStatus::BufferingMedia);
            break;
        }
        case QtAV::MediaStatus::BufferedMedia: {
            emit statusChanged(SDK::MediaStatus::BufferedMedia);
            break;
        }
        case QtAV::MediaStatus::EndOfMedia: {
            //emit statusChanged(SDK::MediaStatus::EndOfMedia);
            break;
        }
        case QtAV::MediaStatus::InvalidMedia: {
            emit statusChanged(SDK::MediaStatus::InvalidMedia);
            break;
        }
        default: {
            break;
        }

    }
}

void QtAVMediaPlayerObject::parent(QWidget *parent)
{
    widget()->setParent(parent);
}

QWidget *QtAVMediaPlayerObject::parent()
{
    return widget()->parentWidget();
}

void QtAVMediaPlayerObject::widget(QWidget *videoWidget)
{
    //this->videoWidget = videoWidget;
}

QWidget *QtAVMediaPlayerObject::widget() const
{
    QWidget* widget = dynamic_cast<QWidget*>(this->videoWidget);
    Q_ASSERT(widget);
    return widget;
}

bool QtAVMediaPlayerObject::mediaPlay(const QString &url)
{
    if(!processHooks(MediaPlayer::BEFORE_PLAY)) return false;

    m_media_player->play(url);
    m_aspect_ratio = (SDK::AspectRatio)settings()->findItem("/media/video/aspect_ratio")->value().toInt();
    //videoWidget->show();
    setAspectRatio(m_aspect_ratio);

    return processHooks(MediaPlayer::AFTER_PLAY);
}

bool QtAVMediaPlayerObject::mediaContinue()
{
    m_media_player->pause(false);
    return true;
}

bool QtAVMediaPlayerObject::mediaPause()
{
    m_media_player->pause(true);
    return true;
}

bool QtAVMediaPlayerObject::mediaStop()
{
    m_media_player->stop();
    return true;
}

bool QtAVMediaPlayerObject::mediaReset()
{
    return true;
}

void QtAVMediaPlayerObject::show()
{
    widget()->show();
}

void QtAVMediaPlayerObject::hide()
{
    widget()->hide();
}

void QtAVMediaPlayerObject::rect(const QRect &rect)
{
    widget()->setGeometry(rect);
}

QRect QtAVMediaPlayerObject::rect() const
{
    return widget()->geometry();
}

bool QtAVMediaPlayerObject::isVisible() const
{
    STUB();
    return widget()->isVisible();
}

SDK::MediaPlayingState QtAVMediaPlayerObject::state()
{
    return SDK::PlayingState;
}

bool QtAVMediaPlayerObject::state(SDK::MediaPlayingState state)
{
    STUB() << state;
    return true;
}

void QtAVMediaPlayerObject::setAspectRatio(SDK::AspectRatio ratio)
{
    STUB() << ratio;

    m_aspect_ratio = ratio;

    QtAV::VideoRenderer* renderer = m_media_player->renderer();

    if(m_aspect_ratio == SDK::ASPECT_RATIO_AUTO)
    {
        renderer->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
        return;
    }

    renderer->setOutAspectRatioMode(QtAV::VideoRenderer::CustomAspectRation);
    float a_ratio = 1;
    switch(m_aspect_ratio)
    {
        case SDK::ASPECT_RATIO_1_1: {
            a_ratio = 1;
            break;
        }
        case SDK::ASPECT_RATIO_5_4: {
            a_ratio = 16.0 / 9;
            break;
        }
        case SDK::ASPECT_RATIO_4_3: {
            a_ratio = 4.0 / 3;
            break;
        }
        case SDK::ASPECT_RATIO_11_8: {
            a_ratio = 11.0 / 8;
            break;
        }
        case SDK::ASPECT_RATIO_14_10: {
            a_ratio = 14.0 / 10;
            break;
        }
        case SDK::ASPECT_RATIO_3_2: {
            a_ratio = 3.0 / 2;
            break;
        }
        case SDK::ASPECT_RATIO_14_9: {
            a_ratio = 14.0 / 9;
            break;
        }
        case SDK::ASPECT_RATIO_16_10: {
            a_ratio = 16.0 / 10;
            break;
        }

        case SDK::ASPECT_RATIO_16_9: {
            a_ratio = 16.0 / 9;
            break;
        }
        case SDK::ASPECT_RATIO_2_35_1: {
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

SDK::AspectRatio QtAVMediaPlayerObject::getAspectRatio() const
{
    return (SDK::AspectRatio)m_aspect_ratio;
}

void QtAVMediaPlayerObject::move(int x, int y)
{
    widget()->move(x, y);
}

int QtAVMediaPlayerObject::getAudioPID() const
{
    STUB();
    return m_media_player->currentAudioStream();
}

void QtAVMediaPlayerObject::setAudioPID(int pid)
{
    STUB() << pid;
    m_media_player->setAudioStream(pid);
}

int QtAVMediaPlayerObject::bufferLoad() const
{
    STUB();
    return 100;
}

qint64 QtAVMediaPlayerObject::getPosition() const
{
    qint64 pos = m_media_player->position();
    return pos;
}

qint64 QtAVMediaPlayerObject::getDuration() const
{
    STUB();
    qint64 duration = m_media_player->duration();
    return duration;
}

int QtAVMediaPlayerObject::getBrightness() const
{
    return m_media_player->brightness();
}

void yasem::QtAVMediaPlayerObject::setBrightness(int brightness)
{
    m_media_player->setBrightness(brightness);
}

int QtAVMediaPlayerObject::getContrast() const
{
    return m_media_player->contrast();
}

void yasem::QtAVMediaPlayerObject::setContrast(int contrast)
{
    m_media_player->setContrast(contrast);
}

int yasem::QtAVMediaPlayerObject::getSaturation() const
{
    return m_media_player->saturation();
}

void yasem::QtAVMediaPlayerObject::setSaturation(int saturation)
{
    m_media_player->setSaturation(saturation);
}

QList<AudioLangInfo> QtAVMediaPlayerObject::getAudioLanguages()
{
    QList<AudioLangInfo> languages;
    int count = m_media_player->audioStreamCount();
    for(int index = 0; index < count; index++)
    {
        languages.append(AudioLangInfo(index, QString::number(index), QString::number(index)));
    }
    return languages;
}

void QtAVMediaPlayerObject::setAudioLanguage(int index)
{
    m_media_player->setAudioStream(index);
}

int QtAVMediaPlayerObject::getLoop() const
{
    return m_media_player->repeat();
}

void QtAVMediaPlayerObject::setLoop(int loop)
{
    m_media_player->setRepeat(loop);
}

bool QtAVMediaPlayerObject::isMute() const
{
    return m_media_player->isMute();
}

void QtAVMediaPlayerObject::setMute(bool value)
{
    m_media_player->setMute(value);
}

void QtAVMediaPlayerObject::setPosition(qint64 pos)
{
    m_media_player->setPosition(pos);
}

int QtAVMediaPlayerObject::getVolume() const
{
    STUB();
    AudioOutput* audio = m_media_player->audio();
    if(audio != NULL)
    {
        return (int)(audio->volume() * 100);
    }
    return 100;
}

void QtAVMediaPlayerObject::setVolume(int vol)
{
    STUB() << vol;
    AudioOutput* audio = m_media_player->audio();
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
    metadata.filename = m_media_player->file();

    return metadata;
}

SDK::Config *QtAVMediaPlayerObject::settings() const
{
    return SDK::Core::instance()->yasem_settings();
}

SDK::PluginObjectResult QtAVMediaPlayerObject::init()
{
    STUB();

    /*ConfigItem* output = m_qtav_settings->findItemByPath("video/output");
    QString output_name = output->value().toString();
    if(output_name.isEmpty())
    {
        output->setValue(output->getDefaultValue());
        m_yasem_settings->save(static_cast<ConfigContainer*>(m_qtav_settings->findItemByKey("video")));
    }
    else
    {
        videoWidget = VideoRendererFactory::create(VideoRendererFactory::id(output_name.toStdString()));
    }*/

    videoWidget = new OpenGLWidgetRenderer();
    widget()->show();
    QOpenGLWidget* oglWidget = openGlWidget();
    if(SDK::Core::instance()->getVM() != SDK::Core::VM_VIRTUAL_BOX && oglWidget != NULL)
    {
        QPair<int,int> opengl_version = oglWidget->context()->format().version();
        if(!openGlWidget()->isValid() || opengl_version.first < 2) // if version is 1.1 it's probably a VirtualBox or other virtualization software
        {
            WARN() << "Can't instantinate OpenGL renderer. Falling back to WidgetRenderer";
            delete videoWidget;
            videoWidget = new WidgetRenderer();
        }
    }
    else
    {
        WARN() << "OpenGL widget not created. Falling back to WidgetRenderer";
        delete videoWidget;
        videoWidget = new WidgetRenderer();
    }


    widget()->setAttribute(Qt::WA_TransparentForMouseEvents);

#ifdef USE_REAL_TRANSPARENCY
    videoWidget->setAttribute(Qt::WA_PaintOnScreen, true);
    videoWidget->setAutoFillBackground(false);
    videoWidget->setUpdatesEnabled(false); // No need to render video on window
    videoWidget->setVisible(false);

    disconnect(videoWidget, SIGNAL(imageReady()), 0, 0);
    connect(videoWidget, SIGNAL(imageReady), this, SIGNAL(rendered));
#endif //USE_REAL_TRANSPARENCY

    m_media_player = new QtAV::AVPlayer();
    m_media_player->setAsyncLoad(true);
    m_media_player->setInterruptTimeout(10000);
    m_media_player->setRenderer(videoWidget);

    SDK::ConfigItem* decoder = m_qtav_settings->findItemByPath("video/decoder");
    QString decoder_name = decoder->value().toString();
    if(decoder_name.isEmpty())
    {
        decoder->setValue(decoder->getDefaultValue());
        settings()->save(static_cast<SDK::ConfigContainer*>(m_qtav_settings->findItemByKey("video")));
    }
    else
    {
        m_media_player->setPriority(QVector<VideoDecoderId>() << VideoDecoderFactory::id(decoder_name.toStdString()));
    }

    m_qtav_settings->setEnabled(true);

    /*
    QString current_renderer = QString::fromStdString(VideoRendererFactory::name(mediaPlayer->renderer()->id()));
    m_qtav_settings->findItemByPath("video/output")->setValue(current_renderer);
    */

    //FIXME() << "Media player signals not connected under Windows!";
    // New signal/slot connection doesn't work under windows  because signal defines two times
    connect(m_media_player, SIGNAL(paused(bool)),                    this, SIGNAL(paused(bool)));
    connect(m_media_player, SIGNAL(started()),                       this, SIGNAL(started()));
    connect(m_media_player, SIGNAL(stopped()),                       this, SIGNAL(stopped()));
    connect(m_media_player, SIGNAL(speedChanged(qreal)),             this, SIGNAL(speedChanged(qreal)));
    connect(m_media_player, SIGNAL(repeatChanged(int)),              this, SIGNAL(repeatChanged(int)));
    connect(m_media_player, SIGNAL(currentRepeatChanged(int)),       this, SIGNAL(currentRepeatChanged(int)));
    connect(m_media_player, SIGNAL(startPositionChanged(qint64)),    this, SIGNAL(startPositionChanged(qint64)));
    connect(m_media_player, SIGNAL(stopPositionChanged(qint64)),     this, SIGNAL(stopPositionChanged(qint64)));
    connect(m_media_player, SIGNAL(positionChanged(qint64)),         this, SIGNAL(positionChanged(qint64)));
    connect(m_media_player, SIGNAL(brightnessChanged(int)),          this, SIGNAL(brightnessChanged(int)));
    connect(m_media_player, SIGNAL(contrastChanged(int)),            this, SIGNAL(contrastChanged(int)));
    connect(m_media_player, SIGNAL(saturationChanged(int)),          this, SIGNAL(saturationChanged(int)));
    connect(m_media_player, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),  this, SLOT(onMediaStatusChanged(QtAV::MediaStatus)));

    return SDK::PLUGIN_OBJECT_RESULT_OK;
}

SDK::PluginObjectResult QtAVMediaPlayerObject::deinit()
{
    return SDK::PLUGIN_OBJECT_RESULT_OK;
}

QPoint QtAVMediaPlayerObject::getWidgetPos() const
{
    return widget()->pos();
}


void QtAVMediaPlayerObject::resize()
{
    MediaPlayer::resize();
}
