#ifndef QTAVMEDIAPLAYEROBJECT_H
#define QTAVMEDIAPLAYEROBJECT_H

#include "macros.h"
#include "mediaplayerpluginobject.h"

#include "QtAV/AVPlayer.h"
#include "QtAVWidgets/OpenGLWidgetRenderer.h"

#include <QRect>
#include <QPainter>

class QSettings;

namespace yasem {

class GuiPluginObject;
class ConfigTreeGroup;

class QtAVMediaPlayerObject: public MediaPlayerPluginObject
{
    Q_OBJECT
public:
    QtAVMediaPlayerObject(Plugin* plugin);
    virtual ~QtAVMediaPlayerObject();

    void parent(QWidget *parent);
    QWidget *parent();
    void widget(QWidget *videoWidget);
    QWidget *widget();
    bool mediaPlay(const QString &url);
    bool mediaContinue();
    bool mediaPause();
    bool mediaStop();
    bool mediaReset();
    void show();
    void hide();
    void rect(const QRect &rect);
    QRect rect() const;
    bool isVisible() const;
    MediaPlayingState state();
    bool state(MediaPlayingState state);
    void setAspectRatio(AspectRatio ratio);
    AspectRatio getAspectRatio();
    void move(int x, int y);

    int getAudioPID() const;
    void setAudioPID(int pid);

    int bufferLoad() const;
    void setPosition(qint64 pos);
    qint64 getPosition() const;
    qint64 getDuration() const;

    int getBrightness() const;
    int getContrast() const;

    void setBrightness(int brightness);
    void setContrast(int contrast);

    void setSaturation(int saturation);
    int getSaturation() const;


    int getLoop() const;
    void setLoop(int loop);
    bool isMute() const;
    void setMute(bool value);

    int getVolume() const;
    void setVolume(int vol);

    MediaMetadata getMediaMetadata();

protected:
    QtAV::OpenGLWidgetRenderer* videoWidget;
    QtAV::AVPlayer* mediaPlayer;
    AspectRatio m_aspect_ratio;
    GuiPluginObject* gui;
    QPixmap m_last_frame;
    QString m_config_file;
    YasemSettings* m_yasem_settings;
    QSettings* m_settings;
    ConfigTreeGroup* m_qtav_settings;

    static void customMessageHandler(QtMsgType, const QMessageLogContext &, const QString &);
    void initSettings();

public slots:
    QList<AudioLangInfo> getAudioLanguages();
    void setAudioLanguage(int index);
protected slots:
    void onMediaStatusChanged(QtAV::MediaStatus status);

    // AbstractPluginObject interface
public:
    PluginObjectResult init();
    PluginObjectResult deinit();

    QPixmap& render();
    QPoint getWidgetPos() const;

    void resize();
};

}

#endif // QTAVMEDIAPLAYEROBJECT_H
