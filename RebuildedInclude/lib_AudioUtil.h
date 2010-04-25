/*system/Include/Application/AppClass/Class/AudioUtil/AudioUtil.cpp*/
extern "C" HRESULT SetAudioSpeakerMode(ENUM_SPEAKER_OUT mode);

extern "C" HRESULT SetAudioAGCMode(ENUM_AUDIO_AGC_MODE mode);

extern "C" HRESULT SetAudioDolbyScaleHILO(long int scalehi, long int scalelo);

extern "C" HRESULT SetAudioDolbyCompressMode(long int mode);

extern "C" HRESULT SetAudioDolbyDownmixMode(long int mode);

extern "C" HRESULT SetAudioSPDIFMode(ENUM_SPDIF_MODE mode);

extern "C" HRESULT SetAudioHdmiOutputMode(ENUM_AUDIO_DIGITAL_OUTPUT_MODE mode);

extern "C" HRESULT SetAudioMpgSpdifMode(bool raw);

extern "C" HRESULT SetAudioSpdifOutputMode(ENUM_AUDIO_DIGITAL_OUTPUT_MODE mode);

extern "C" HRESULT SetAudioVolume(int volume);

extern "C" HRESULT SetAudioMute(bool bMute);

extern "C" HRESULT SetAudioPPEqualizer(ENUM_EQUALIZER_ENA mode, AUDIO_RPC_EQUALIZER_MODE info);

extern "C" HRESULT SetAudioBassManagement(ENUM_BASS_MANAGEMENT_ENA mode, AUDIO_RPC_BASS_MANAGEMENT_MODE info);

extern "C" HRESULT SetAudioMicEchoReverb(AUDIO_AO_ECHO_REVERB_CONFIG mode, AUDIO_AO_ECHO_DELAY_MODE echo_delay, AUDIO_VOLUME_LEVEL echo_fdbk_gain, AUDIO_VOLUME_LEVEL echo_volume, ENUM_REVERB_MODE reverb_mode, AUDIO_VOLUME_LEVEL reverb_volume, AUDIO_VOLUME_LEVEL bypass_volume);

extern "C" HRESULT SetAudioMic1Volume(int volume);

extern "C" HRESULT SetAudioMic2Volume(int volume);

extern "C" HRESULT SetAudioDecVolume(int volume);

extern "C" HRESULT SetAudioAuxVolume(int volume);

extern "C" HRESULT SetAudioMicConfig(AUDIO_AO_MIC_CONFIG mode);

extern "C" HRESULT SetAudioAuxConfig(AUDIO_AO_AUX_CONFIG mode);

extern "C" HRESULT SetAudioAuxEqualizer(ENUM_EQUALIZER_ENA mode, AUDIO_RPC_EQUALIZER_MODE info);

extern "C" HRESULT SetAudioMicEcho(int strength);

extern "C" HRESULT SetAudioMicReverb(bool bEnable, ENUM_REVERB_MODE mode);

extern "C" HRESULT CheckAudioFormatParserCapability(ENUM_MEDIA_TYPE type);

extern "C" HRESULT SetAudioForceChannelCtrl(int ctrlBits);

extern "C" HRESULT GetAudioDecoderCapability(long long unsigned int * capability);


