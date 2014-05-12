#ifndef PLAYERTHREAD_H
#define PLAYERTHREAD_H

#include "QtAV/AVPlayer.h"
#include <QtAV/AudioFormat.h>
#include <QtAV/AudioOutputOpenAL.h>

#include <QThread>
#include <QDebug>

namespace yasem
{

class PlayerThread : public QThread
{
public:
    PlayerThread(QtAV::AVPlayer *player):
        QThread(0)
      , mpPlayer(player)
    {}
protected:
    virtual void run() {
        //mpPlayer->play();
        qDebug() << "Thread" << this->objectName() << "started";
        //QtAV::AudioOutputOpenAL* audioOutput = static_cast<QtAV::AudioOutputOpenAL*>(QtAV::AudioOutputFactory::create(QtAV::AudioOutputId_OpenAL));
        //AudioOutputOpenAL* audioOutput = static_cast<AudioOutputOpenAL*>(AudioOutputFactory::create(AudioOutputId_OpenAL));

        //qDebug() << "====================== SAMPLE RATE:" << audioOutput->sampleRate();
        //mpPlayer->setAudioOutput(audioOutput);
        mpPlayer->play();
        exec();
    }
    QtAV::AVPlayer *mpPlayer;
};

}

#endif // PLAYERTHREAD_H
