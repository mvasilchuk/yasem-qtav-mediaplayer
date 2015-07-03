#ifndef LIBAVMEDIAPLAYER_H
#define LIBAVMEDIAPLAYER_H

#include "plugin.h"

#include "qtavmediaplayer_global.h"

namespace yasem {

class QTAVMEDIAPLAYERSHARED_EXPORT QtAvMediaPlayerPlugin: public SDK::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.QtAvMediaPlayer/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::SDK::Plugin)

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "QtAV media player")
    Q_CLASSINFO("version", MODULE_VERSION)
    Q_CLASSINFO("revision", GIT_VERSION)
public:
    explicit QtAvMediaPlayerPlugin(QObject* parent = NULL);
    virtual ~QtAvMediaPlayerPlugin();

    // Plugin interface
public:
    void register_dependencies();
    void register_roles();
};

}

#endif // LIBAVMEDIAPLAYER_H
