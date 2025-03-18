1. mkdir astro-build
2. cd astro-build
3. cmake <path-to-astro-cmakelist.txt>
4. make -j <n>

5. Download and extract following models to working directory (astro-build):

wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/silero_vad.onnx

wget https://github.com/k2-fsa/sherpa-onnx/releases/download/kws-models/sherpa-onnx-kws-zipformer-wenetspeech-3.3M-2024-01-01-mobile.tar.bz2
tar xvf sherpa-onnx-kws-zipformer-wenetspeech-3.3M-2024-01-01-mobile.tar.bz2

wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-paraformer-zh-small-2024-03-09.tar.bz2
tar xvf sherpa-onnx-paraformer-zh-small-2024-03-09.tar.bz2

6. run ./astro-bin
7. say smth into mic

