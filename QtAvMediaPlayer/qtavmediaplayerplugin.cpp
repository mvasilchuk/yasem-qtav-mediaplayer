#include "qtavmediaplayerplugin.h"
#include "qtavmediaplayerobject.h"

using namespace yasem;

QtAvMediaPlayerPlugin::QtAvMediaPlayerPlugin(QObject* parent):
    Plugin(parent)
{
    setMultithreading(false);
}

QtAvMediaPlayerPlugin::~QtAvMediaPlayerPlugin()
{

}

void yasem::QtAvMediaPlayerPlugin::register_dependencies()
{
}

void yasem::QtAvMediaPlayerPlugin::register_roles()
{
    register_role(ROLE_MEDIA, new QtAVMediaPlayerObject(this));
}
