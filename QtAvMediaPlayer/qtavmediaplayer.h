#ifndef LIBAVMEDIAPLAYER_H
#define LIBAVMEDIAPLAYER_H

#include "macros.h"
#include "mediaplayerplugin.h"
#include "plugin.h"
#include "QtAVWidgets/WidgetRenderer.h"
#include "QtAV/AVPlayer.h"

#include "qtavmediaplayer_global.h"

#include <QRect>

namespace yasem {

class GuiPlugin;

class QTAVMEDIAPLAYERSHARED_EXPORT QtAvMediaPlayer: public QObject, public virtual Plugin, public MediaPlayerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.QtAvMediaPlayer/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::Plugin yasem::MediaPlayerPlugin)

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "LibAV media player for YASEM")
public:
    QtAvMediaPlayer();

protected:
    QtAV::WidgetRenderer* videoWidget;
    QtAV::AVPlayer* mediaPlayer;
    ASPECT_RATIO mAspectRatio;

    static void customMessageHandler(QtMsgType, const QMessageLogContext &, const QString &);


    // Plugin interface
public:
    PLUGIN_ERROR_CODES initialize();
    PLUGIN_ERROR_CODES deinitialize();

    // MediaPlayerPlugin interface
public:
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
    void aspectRatio(ASPECT_RATIO ratio);
    ASPECT_RATIO aspectRatio();
    void move(int x, int y);
    void raise();

    int audioPID();
    int bufferLoad();
    void position(qint64 pos);
    qint64 position();
    qint64 duration();

    // MediaPlayerPlugin interface

    void audioPID(int pid);
    int loop();
    void loop(int loop);
    bool mute();
    void mute(bool value);

    int volume();
    void volume(int vol);

    GuiPlugin* gui;


    // Plugin interface
public:
    void register_dependencies();
    void register_roles();
protected slots:
    void onMediaStatusChanged(QtAV::MediaStatus status);
};

}

#endif // LIBAVMEDIAPLAYER_H
