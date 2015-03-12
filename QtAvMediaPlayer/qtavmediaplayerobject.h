#ifndef QTAVMEDIAPLAYEROBJECT_H
#define QTAVMEDIAPLAYEROBJECT_H

#include "macros.h"
#include "mediaplayerpluginobject.h"

#include "QtAVWidgets/WidgetRenderer.h"
#include "QtAV/AVPlayer.h"

#include <QRect>

namespace yasem {

class GuiPluginObject;

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
    QRect rect();
    bool isVisible();
    MediaPlayingState state();
    bool state(MediaPlayingState state);
    void aspectRatio(AspectRatio ratio);
    AspectRatio aspectRatio();
    void move(int x, int y);
    void raise();

    int audioPID();
    int bufferLoad();
    void position(qint64 pos);
    qint64 position();
    qint64 duration();

    int getBrightness();
    int getContrast();

    // MediaPlayerPlugin interface

    void audioPID(int pid);
    int loop();
    void loop(int loop);
    bool mute();
    void mute(bool value);

    int volume();
    void volume(int vol);

    MediaMetadata getMediaMetadata();

protected:
    QtAV::WidgetRenderer* videoWidget;
    QtAV::AVPlayer* mediaPlayer;
    AspectRatio m_aspect_ratio;
    GuiPluginObject* gui;

    static void customMessageHandler(QtMsgType, const QMessageLogContext &, const QString &);

public slots:
    QList<AudioLangInfo> getAudioLanguages();
    void setAudioLanguage(int index);
protected slots:
    void onMediaStatusChanged(QtAV::MediaStatus status);

    // AbstractPluginObject interface
public:
    PluginObjectResult init();
    PluginObjectResult deinit();
};

}

#endif // QTAVMEDIAPLAYEROBJECT_H
