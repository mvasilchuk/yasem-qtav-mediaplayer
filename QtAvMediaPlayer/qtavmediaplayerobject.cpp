#include "qtavmediaplayerobject.h"
#include "guipluginobject.h"
#include "pluginmanager.h"
#include "core.h"
#include "yasemsettings.h"

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

QtAVMediaPlayerObject::QtAVMediaPlayerObject(Plugin* plugin):
    MediaPlayerPluginObject(plugin),
    m_aspect_ratio(ASPECT_RATIO_AUTO),
    m_config_file("qtav-player"),
    m_yasem_settings(Core::instance()->yasem_settings()),
    m_settings(Core::instance()->settings(m_config_file))
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
    m_qtav_settings = new ConfigTreeGroup(m_config_file, "qtav", tr("QtAV"));
    ConfigTreeGroup* qtav_video = new ConfigTreeGroup("video", tr("Video settings"));
    ConfigTreeGroup* qtav_audio = new ConfigTreeGroup("audio", tr("Audio settings"));

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
    ListConfigItem* video_decoder_priority = new ListConfigItem("decoder", tr("Decoder"), default_decoder_name);
    for(const std::string &name: VideoDecoderFactory::registeredNames())
    {
        const QString s_name = QString::fromStdString(name);
        QString title = s_name;
        if(title == default_decoder_name)
            title.append(tr(" (default)"));
        video_decoder_priority->options().insert(title, s_name);
    }
    qtav_video->addItem(video_decoder_priority);
    connect(video_decoder_priority, &ConfigItem::saved, [=]() {
        QString decoder = video_decoder_priority->getValue().toString();
        DEBUG() << "setting a new decoder" << decoder;
        mediaPlayer->setPriority(QVector<VideoDecoderId>() << VideoDecoderFactory::id(decoder.toStdString()));
    });

    m_qtav_settings->addItem(qtav_video);
    m_qtav_settings->addItem(qtav_audio);

    m_yasem_settings->findItem(YasemSettings::SETTINGS_GROUP_PLUGINS)->addItem(m_qtav_settings);
    m_yasem_settings->load();
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
    if(!processHooks(MediaPlayerPluginObject::BEFORE_PLAY)) return false;

    mediaPlayer->play(url);
    m_aspect_ratio = (AspectRatio)m_yasem_settings->findItem("/media/video/aspect_ratio")->value().toInt();
    //videoWidget->show();
    setAspectRatio(m_aspect_ratio);

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

MediaPlayingState QtAVMediaPlayerObject::state()
{
    return PlayingState;
}

bool QtAVMediaPlayerObject::state(MediaPlayingState state)
{
    STUB() << state;
    return true;
}

void QtAVMediaPlayerObject::setAspectRatio(AspectRatio ratio)
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

AspectRatio QtAVMediaPlayerObject::getAspectRatio()
{
    return (AspectRatio)m_aspect_ratio;
}

void QtAVMediaPlayerObject::move(int x, int y)
{
    widget()->move(x, y);
}

int QtAVMediaPlayerObject::getAudioPID() const
{
    STUB();
    return mediaPlayer->currentAudioStream();
}

void QtAVMediaPlayerObject::setAudioPID(int pid)
{
    STUB() << pid;
    mediaPlayer->setAudioStream(pid);
}

int QtAVMediaPlayerObject::bufferLoad() const
{
    STUB();
    return 100;
}

qint64 QtAVMediaPlayerObject::getPosition() const
{
    qint64 pos = mediaPlayer->position();
    return pos;
}

qint64 QtAVMediaPlayerObject::getDuration() const
{
    STUB();
    qint64 duration = mediaPlayer->duration();
    return duration;
}

int QtAVMediaPlayerObject::getBrightness() const
{
    return mediaPlayer->brightness();
}

void yasem::QtAVMediaPlayerObject::setBrightness(int brightness)
{
    mediaPlayer->setBrightness(brightness);
}

int QtAVMediaPlayerObject::getContrast() const
{
    return mediaPlayer->contrast();
}

void yasem::QtAVMediaPlayerObject::setContrast(int contrast)
{
    mediaPlayer->setContrast(contrast);
}

int yasem::QtAVMediaPlayerObject::getSaturation() const
{
    return mediaPlayer->saturation();
}

void yasem::QtAVMediaPlayerObject::setSaturation(int saturation)
{
    mediaPlayer->setSaturation(saturation);
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

int QtAVMediaPlayerObject::getLoop() const
{
    return mediaPlayer->repeat();
}

void QtAVMediaPlayerObject::setLoop(int loop)
{
    mediaPlayer->setRepeat(loop);
}

bool QtAVMediaPlayerObject::isMute() const
{
    return mediaPlayer->isMute();
}

void QtAVMediaPlayerObject::setMute(bool value)
{
    mediaPlayer->setMute(value);
}

void QtAVMediaPlayerObject::setPosition(qint64 pos)
{
    mediaPlayer->setPosition(pos);
}

int QtAVMediaPlayerObject::getVolume() const
{
    STUB();
    AudioOutput* audio = mediaPlayer->audio();
    if(audio != NULL)
    {
        return (int)(audio->volume() * 100);
    }
    return 100;
}

void QtAVMediaPlayerObject::setVolume(int vol)
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

PluginObjectResult QtAVMediaPlayerObject::init()
{
    STUB();
    gui = dynamic_cast<GuiPluginObject*>(PluginManager::instance()->getByRole(ROLE_GUI));

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
    QPair<int,int> opengl_version = openGlWidget()->context()->format().version();
    if(!openGlWidget()->isValid() || opengl_version.first < 2) // if version is 1.1 it's probably a VirtualBox or other virtualization software
    {
        WARN() << "Can't instantinate OpenGL renderer. Falling back to WidgetRenderer";
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

    mediaPlayer = new QtAV::AVPlayer();
    mediaPlayer->setAsyncLoad(true);
    mediaPlayer->setInterruptTimeout(10000);
    mediaPlayer->setRenderer(videoWidget);

    ConfigItem* decoder = m_qtav_settings->findItemByPath("video/decoder");
    QString decoder_name = decoder->value().toString();
    if(decoder_name.isEmpty())
    {
        decoder->setValue(decoder->getDefaultValue());
        m_yasem_settings->save(static_cast<ConfigContainer*>(m_qtav_settings->findItemByKey("video")));
    }
    else
    {
        mediaPlayer->setPriority(QVector<VideoDecoderId>() << VideoDecoderFactory::id(decoder_name.toStdString()));
    }

    /*
    QString current_renderer = QString::fromStdString(VideoRendererFactory::name(mediaPlayer->renderer()->id()));
    m_qtav_settings->findItemByPath("video/output")->setValue(current_renderer);
    */

    //FIXME() << "Media player signals not connected under Windows!";
    // New signal/slot connection doesn't work under windows  because signal defines two times
    connect(mediaPlayer, SIGNAL(paused(bool)),                    this, SIGNAL(paused(bool)));
    connect(mediaPlayer, SIGNAL(started()),                       this, SIGNAL(started()));
    connect(mediaPlayer, SIGNAL(stopped()),                       this, SIGNAL(stopped()));
    connect(mediaPlayer, SIGNAL(speedChanged(qreal)),             this, SIGNAL(speedChanged(qreal)));
    connect(mediaPlayer, SIGNAL(repeatChanged(int)),              this, SIGNAL(repeatChanged(int)));
    connect(mediaPlayer, SIGNAL(currentRepeatChanged(int)),       this, SIGNAL(currentRepeatChanged(int)));
    connect(mediaPlayer, SIGNAL(startPositionChanged(qint64)),    this, SIGNAL(startPositionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(stopPositionChanged(qint64)),     this, SIGNAL(stopPositionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)),         this, SIGNAL(positionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(brightnessChanged(int)),          this, SIGNAL(brightnessChanged(int)));
    connect(mediaPlayer, SIGNAL(contrastChanged(int)),            this, SIGNAL(contrastChanged(int)));
    connect(mediaPlayer, SIGNAL(saturationChanged(int)),          this, SIGNAL(saturationChanged(int)));
    connect(mediaPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),  this, SLOT(onMediaStatusChanged(QtAV::MediaStatus)));

    return PLUGIN_OBJECT_RESULT_OK;
}

PluginObjectResult QtAVMediaPlayerObject::deinit()
{
    return PLUGIN_OBJECT_RESULT_OK;
}

QPixmap& QtAVMediaPlayerObject::render()
{
    widget()->render(&m_last_frame);
    return m_last_frame;
}


QPoint QtAVMediaPlayerObject::getWidgetPos() const
{
    return widget()->pos();
}


void QtAVMediaPlayerObject::resize()
{
    MediaPlayerPluginObject::resize();
    m_last_frame = QPixmap(widget()->size());
}
