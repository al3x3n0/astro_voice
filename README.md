# Astro Voice Assistant

A C++ voice assistant that uses Sherpa-onnx for speech recognition and text-to-speech.

## Features

- Voice Activity Detection (VAD)
- Keyword Spotting (KWS)
- Speech Recognition (ASR)
  - English (Zipformer)
  - Chinese (Paraformer)
  - Multilingual (Sense Voice)
- Text-to-Speech (TTS)

## Dependencies

- C++17 or later
- CMake 3.14 or later
- Sherpa-onnx
- nlohmann/json
- cxxopts

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Configuration

The application uses a JSON configuration file to manage model paths and parameters. You can specify the configuration file using the `--config` or `-c` command-line argument:

```bash
./astro-bin --config path/to/config.json
```

If no configuration file is specified, the application will use default paths relative to the current working directory.

### Default Configuration Structure

```json
{
    "models": {
        "vad": {
            "path": "./silero_vad.onnx",
            "description": "Voice Activity Detection model"
        },
        "kws": {
            "path": "./sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01",
            "description": "Keyword Spotting model",
            "tokens_type": "bpe",
            "default_threshold": 0.5,
            "default_boost": 1.0
        },
        "asr_zh": {
            "path": "./sherpa-onnx-paraformer-zh-small-2024-03-09",
            "description": "Chinese Speech Recognition model"
        },
        "asr_en": {
            "path": "./sherpa-onnx-zipformer-gigaspeech-2023-12-12",
            "description": "English Speech Recognition model"
        },
        "asr_multi": {
            "path": "./sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17",
            "description": "Multilingual Speech Recognition model"
        },
        "tts": {
            "path": "./matcha-icefall-en_US-ljspeech",
            "description": "Text-to-Speech model",
            "vocoder": "./vocos-22khz-univ.onnx",
            "tokens": "./matcha-icefall-en_US-ljspeech/tokens.txt",
            "data_dir": "./matcha-icefall-en_US-ljspeech/espeak-ng-data",
            "num_threads": 1,
            "debug": true
        }
    },
    "keywords": {
        "default_threshold": 0.5,
        "default_boost": 1.0,
        "output_dir": "./keywords"
    },
    "build_dir": "."
}
```

### Command Line Options

- `--config, -c`: Path to configuration file
- `--help, -h`: Show help message

## Usage

1. Place your model files in the appropriate directories as specified in the configuration
2. Run the application:
   ```bash
   ./astro-bin --config path/to/config.json
   ```
3. The application will:
   - Load the specified configuration file
   - Initialize all models with the configured paths
   - Start listening for voice input
   - Process speech and generate responses

## Model Files

The following model files are required:

### VAD Model
- `silero_vad.onnx`

### KWS Model
- `sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/encoder-epoch-12-avg-2-chunk-16-left-64.int8.onnx`
- `sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/decoder-epoch-12-avg-2-chunk-16-left-64.int8.onnx`
- `sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/joiner-epoch-12-avg-2-chunk-16-left-64.int8.onnx`
- `sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01/tokens.txt`

### ASR Models
- English: `sherpa-onnx-zipformer-gigaspeech-2023-12-12/*`
- Chinese: `sherpa-onnx-paraformer-zh-small-2024-03-09/*`
- Multilingual: `sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17/*`

### TTS Model
- `matcha-icefall-en_US-ljspeech/model-steps-3.onnx`
- `vocos-22khz-univ.onnx`
- `matcha-icefall-en_US-ljspeech/tokens.txt`
- `matcha-icefall-en_US-ljspeech/espeak-ng-data/*`

## License

[Your License Here]

# English Version

1. mkdir astro-build
2. cd astro-build
3. cmake <path-to-astro-cmakelist.txt>
4. make -j <n>

5. Download and extract following models to working directory (astro-build):

# Voice Activity Detection (VAD) model
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/silero_vad.onnx

# Keyword Spotting (KWS) model
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/kws-models/sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01.tar.bz2
tar xvf sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01.tar.bz2

# Chinese Speech Recognition model
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-paraformer-zh-small-2024-03-09.tar.bz2
tar xvf sherpa-onnx-paraformer-zh-small-2024-03-09.tar.bz2

# English Speech Recognition model
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-zipformer-gigaspeech-2023-12-12.tar.bz2
tar xvf sherpa-onnx-zipformer-gigaspeech-2023-12-12.tar.bz2

# Multilingual Speech Recognition model
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17.tar.bz2
tar xvf sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17.tar.bz2

6. run ./astro-bin
7. say smth into mic

# 中文版本

1. 创建构建目录
mkdir astro-build
2. 进入构建目录
cd astro-build
3. 运行 CMake
cmake <path-to-astro-cmakelist.txt>
4. 编译
make -j <n>

5. 下载并解压以下模型到工作目录 (astro-build):

# 语音活动检测 (VAD) 模型
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/silero_vad.onnx

# 关键词检测 (KWS) 模型
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/kws-models/sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01.tar.bz2
tar xvf sherpa-onnx-kws-zipformer-gigaspeech-3.3M-2024-01-01.tar.bz2

# 中文语音识别模型
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-paraformer-zh-small-2024-03-09.tar.bz2
tar xvf sherpa-onnx-paraformer-zh-small-2024-03-09.tar.bz2

# 英文语音识别模型
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-zipformer-gigaspeech-2023-12-12.tar.bz2
tar xvf sherpa-onnx-zipformer-gigaspeech-2023-12-12.tar.bz2

# 多语言语音识别模型
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17.tar.bz2
tar xvf sherpa-onnx-sense-voice-zh-en-ja-ko-yue-2024-07-17.tar.bz2

6. 运行程序
./astro-bin
7. 对着麦克风说话

