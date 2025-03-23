#include "audio_stream.hpp"


#include <queue>
#include <mutex>

struct AudioData {
    std::vector<float> samples;
};

class ThreadSafeQueue {
public:
    void push(const std::vector<float>& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push({data});
    }

    bool pop(AudioData& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        data = m_queue.front();
        m_queue.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

private:
    std::queue<AudioData> m_queue;
    mutable std::mutex m_mutex;
};

namespace astro
{

static int _pa_record_callback(
        const void* input,
        void* output,
        unsigned long frames_count,  // NOLINT
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void* user_data) {
    //
    auto as = static_cast<AudioStream*>(user_data);
    const auto* pcm_data = static_cast<const float*>(input);
    std::vector<float> v(pcm_data, pcm_data + frames_count);
    return as->OnRecord(v);
    /*
    if (g_exiting) {
        LOG(INFO) << "Exiting loop.";
        return paComplete;
    } else {
        return paContinue;
    }
    */
}


AudioStream::AudioStream(std::shared_ptr<AudioQueue> audio_queue) :
        m_audio_queue(audio_queue)
{
    PaError err = Pa_Initialize();
    m_params.device = Pa_GetDefaultInputDevice();
    //if (params.device == paNoDevice) {
    //    LOG(FATAL) << "Error: No default input device.";
    //}
    m_params.channelCount = 1;
    m_params.sampleFormat = paFloat32;
    m_params.suggestedLatency = Pa_GetDeviceInfo(m_params.device)->defaultLowInputLatency;
    m_params.hostApiSpecificStreamInfo = NULL;

    Pa_OpenStream(&m_stream, &m_params, NULL,
        16000, m_frameSize, paClipOff,
        _pa_record_callback, (void*)this);
}

AudioStream::~AudioStream() {
    Pa_CloseStream(m_stream);
    Pa_Terminate();
}

int AudioStream::OnRecord(const std::vector<float>& samples) {
    m_audio_queue->push({samples, 16000});
    return paContinue;
}

void AudioStream::start() {
    Pa_StartStream(m_stream);

    while (Pa_IsStreamActive(m_stream)) {
        Pa_Sleep(m_interval);
    }
}

}
