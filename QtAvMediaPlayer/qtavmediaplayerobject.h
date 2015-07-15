#ifndef QTAVMEDIAPLAYEROBJECT_H
#define QTAVMEDIAPLAYEROBJECT_H

#include "macros.h"
#include "mediaplayer.h"

#include "QtAV/AVPlayer.h"

#include <QRect>
#include <QPainter>
#include <QOpenGLWidget>

class QSettings;

namespace yasem {

namespace SDK {
class GUI;
class ConfigTreeGroup;
}

class QtAVMediaPlayerObject: public SDK::MediaPlayer
{
    Q_OBJECT
public:
    QtAVMediaPlayerObject(SDK::Plugin* plugin);
    virtual ~QtAVMediaPlayerObject();

    void parent(QWidget *parent);
    QWidget *parent();
    void widget(QWidget *videoWidget);
    QWidget *widget() const;
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
    SDK::MediaPlayingState state();
    bool state(SDK::MediaPlayingState state);
    void setAspectRatio(SDK::AspectRatio ratio);
    SDK::AspectRatio getAspectRatio();
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
    QtAV::VideoRenderer* videoWidget;
    QtAV::AVPlayer* m_media_player;
    SDK::AspectRatio m_aspect_ratio;
    SDK::Config* m_yasem_settings;
    SDK::ConfigTreeGroup* m_qtav_settings;

    static void customMessageHandler(QtMsgType, const QMessageLogContext &, const QString &);
    void initSettings();
    QOpenGLWidget* openGlWidget() const;


public slots:
    QList<AudioLangInfo> getAudioLanguages();
    void setAudioLanguage(int index);
protected slots:
    void onMediaStatusChanged(QtAV::MediaStatus status);

    // AbstractPluginObject interface
public:
    SDK::PluginObjectResult init();
    SDK::PluginObjectResult deinit();

    QPoint getWidgetPos() const;

    void resize();
};

}

#endif // QTAVMEDIAPLAYEROBJECT_H
