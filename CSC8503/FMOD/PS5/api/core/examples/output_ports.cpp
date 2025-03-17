/*==============================================================================
Output Ports Example
Copyright (c), Firelight Technologies Pty, Ltd 2018-2024.

This example demonstrates how to specify a particular ChannelGroup as the 
background music endpoint. This means GameDVR will not record audio sent to this
ChannelGroup and it can be replaced by user music.
==============================================================================*/
#include "fmod.hpp"
#include "common.h"

#include <sceerror.h>
#include <user_service.h>

int FMOD_Main()
{
    Common_Init(nullptr);

    /*
        Get User ID of the user that started the application
    */
    SceUserServiceUserId userId;
    if (sceUserServiceGetInitialUser(&userId) != SCE_OK)
    {
        ERRCHECK(FMOD_ERR_INITIALIZATION);
    }

    /*
        Create a System object and initialize
    */
    FMOD::System *system;
    FMOD_RESULT result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->init(32, FMOD_INIT_NORMAL, nullptr);
    ERRCHECK(result);

    /*
        Create some sounds to play
    */
    FMOD::Sound *sound1;
    result = system->createSound(Common_MediaPath("drumloop.wav"), FMOD_LOOP_NORMAL, nullptr, &sound1);
    ERRCHECK(result);

    FMOD::Sound *sound2;
    result = system->createSound(Common_MediaPath("wave.mp3"), FMOD_LOOP_NORMAL, nullptr, &sound2);
    ERRCHECK(result);

    FMOD::Sound *sound3;
    result = system->createSound(Common_MediaPath("jaguar.wav"), FMOD_DEFAULT, nullptr, &sound3);
    ERRCHECK(result);

    FMOD::Sound *sound4;
    result = system->createSound(Common_MediaPath("stereo.ogg"), FMOD_DEFAULT, nullptr, &sound4);
    ERRCHECK(result);

    /*
        Create a channel group to represent the background music port (backgroundPort)
    */
    FMOD::ChannelGroup *backgroundPort;
    result = system->createChannelGroup("Background Music Port", &backgroundPort);
    ERRCHECK(result);

    result = system->attachChannelGroupToPort(FMOD_PORT_TYPE_MUSIC, FMOD_PORT_INDEX_NONE, backgroundPort);
    ERRCHECK(result);

    /*
        Create a channel group to represent the controller speaker
    */
    FMOD::ChannelGroup *controllerSpeakerPort;
    result = system->createChannelGroup("Controller Speaker Port", &controllerSpeakerPort);
    ERRCHECK(result);

    result = system->attachChannelGroupToPort(FMOD_PORT_TYPE_CONTROLLER, userId, controllerSpeakerPort);
    ERRCHECK(result);

    /*
        Create a channel group to represent the controller vibration
    */
    FMOD::ChannelGroup* controllerVibrationPort;
    result = system->createChannelGroup("Controller Vibration Port", &controllerVibrationPort);
    ERRCHECK(result);

    /* To send to PSVR2 controller set portIndex to userId | FMOD_PORT_INDEX_FLAG_VR_CONTROLLER */
    result = system->attachChannelGroupToPort(FMOD_PORT_TYPE_VIBRATION, userId, controllerVibrationPort);
    ERRCHECK(result);

    /*
        Play a sound normally and on the background port
    */
    FMOD::Channel *channel1;
    result = system->playSound(sound1, nullptr, false, &channel1);
    ERRCHECK(result);
    
    FMOD::Channel *channel2;
    result = system->playSound(sound2, backgroundPort, false, &channel2);
    ERRCHECK(result);

    /*
        Main loop
    */
    do
    {
        Common_Update();

        if (Common_BtnPress(BTN_ACTION1))
        {
            bool paused = false;
            result = channel1->getPaused(&paused);
            ERRCHECK(result);
            result = channel1->setPaused(!paused);
            ERRCHECK(result);
        }

        if (Common_BtnPress(BTN_ACTION2))
        {
            bool paused = false;
            result = channel2->getPaused(&paused);
            ERRCHECK(result);
            result = channel2->setPaused(!paused);
            ERRCHECK(result);
        }

        if (Common_BtnPress(BTN_ACTION3))
        {
            FMOD::Channel *channel3 = nullptr;
            result = system->playSound(sound3, controllerSpeakerPort, false, &channel3);
            ERRCHECK(result);
        }

        if (Common_BtnPress(BTN_ACTION4))
        {
            FMOD::Channel *channel4 = nullptr;
            result = system->playSound(sound4, controllerVibrationPort, false, &channel4);
            ERRCHECK(result);
        }

        result = system->update();
        ERRCHECK(result);

        {
            Common_Draw("==================================================");
            Common_Draw("Output Ports Example.");
            Common_Draw("Copyright (c) Firelight Technologies 2018-2024.");
            Common_Draw("==================================================");
            Common_Draw("");
            Common_Draw("Press %s to toggle normal audio playback", Common_BtnStr(BTN_ACTION1));
            Common_Draw("Press %s to toggle background music playback", Common_BtnStr(BTN_ACTION2));
            Common_Draw("Press %s to play a sound on the controller", Common_BtnStr(BTN_ACTION3));
            Common_Draw("Press %s to vibrate the controller", Common_BtnStr(BTN_ACTION4));
            Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
        }

        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    
    /*
        Shut down
    */
    result = sound1->release();
    ERRCHECK(result);
    result = sound2->release();
    ERRCHECK(result);
    result = sound3->release();
    ERRCHECK(result);
    result = sound4->release();
    ERRCHECK(result);
    result = system->detachChannelGroupFromPort(backgroundPort);
    ERRCHECK(result);
    result = backgroundPort->release();
    ERRCHECK(result);
    result = system->detachChannelGroupFromPort(controllerSpeakerPort);
    ERRCHECK(result);
    result = controllerSpeakerPort->release();
    ERRCHECK(result);
    result = system->detachChannelGroupFromPort(controllerVibrationPort);
    ERRCHECK(result);
    result = controllerVibrationPort->release();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}
