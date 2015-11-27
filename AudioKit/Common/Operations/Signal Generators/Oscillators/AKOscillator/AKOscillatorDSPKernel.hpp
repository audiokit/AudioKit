//
//  AKOscillatorDSPKernel.hpp
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

#ifndef AKOscillatorDSPKernel_hpp
#define AKOscillatorDSPKernel_hpp

#import "AKDSPKernel.hpp"
#import "AKParameterRamper.hpp"

extern "C" {
#include "soundpipe.h"
}

enum {
    frequencyAddress = 0,
    amplitudeAddress = 1
};

class AKOscillatorDSPKernel : public AKDSPKernel {
public:
    // MARK: Member Functions

    AKOscillatorDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp_osc_create(&osc);
        sp_osc_init(sp, osc, ftbl, 0);
        osc->freq = 440;
        osc->amp = 1;
    }

    size_t getTableSize() {
        return ftbl->size;
    }
    
    void setupTable() {
        sp_ftbl_create(sp, &ftbl, 4096);
        NSLog(@"setting up table");
    }
    
    void setTableValue(uint32_t index, float value) {
        ftbl->tbl[index] = value;
    }
    
    void reset() {
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case frequencyAddress:
                frequencyRamper.set(clamp(value, (float)0, (float)20000));
                break;

            case amplitudeAddress:
                amplitudeRamper.set(clamp(value, (float)0, (float)10000));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case frequencyAddress:
                return frequencyRamper.goal();

            case amplitudeAddress:
                return amplitudeRamper.goal();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case frequencyAddress:
                frequencyRamper.startRamp(clamp(value, (float)0, (float)20000), duration);
                break;

            case amplitudeAddress:
                amplitudeRamper.startRamp(clamp(value, (float)0, (float)10000), duration);
                break;

        }
    }

    void setBuffers(AudioBufferList* inBufferList, AudioBufferList* outBufferList) {
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // For each sample.
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            double frequency = double(frequencyRamper.getStep());
            double amplitude = double(amplitudeRamper.getStep());

            int frameOffset = int(frameIndex + bufferOffset);

            osc->freq = (float)frequency;
            osc->amp = (float)amplitude;
            
            float temp = 0;
            for (int channel = 0; channel < channels; ++channel) {
                float* out = (float*)outBufferListPtr->mBuffers[channel].mData + frameOffset;
                if (channel == 0) {
                    sp_osc_compute(sp, osc, nil, &temp);
                }
                *out = temp;
            }
        }
    }

    // MARK: Member Variables

private:

    int channels = 2;
    float sampleRate = 44100.0;

    AudioBufferList* outBufferListPtr = nullptr;

    sp_data *sp;
    sp_osc *osc;
    sp_ftbl *ftbl;

public:
    AKParameterRamper frequencyRamper = 440;
    AKParameterRamper amplitudeRamper = 1;
};

#endif /* AKOscillatorDSPKernel_hpp */
