# ofxOnnxRuntime
[ONNX Runtime](https://github.com/microsoft/onnxruntime) tiny wrapper for openFrameworks
Modified by [Ryo Hajika](https://github.com/ryohajika)

!['test'](screenshot.png)

## Installation
Go to [ONNX runtime release page](https://github.com/microsoft/onnxruntime/releases/tag/v1.19.2) to download precompiled library file and download under `libs` folder.
(for macOS I bundled 1.19.2 universal lib so should be working on both Intel and Mx Macs)

- macOS
    - Generate a project using ProjectGenerator.
- Windows (untested)
    - There are two ways to install ONNX Runtime on your project.
    1. Install using NuGet
        - I recommend this way in general.
        - Generate a project using ProjectGenerator.
        - Open `sln` file.
        - Right click your project on `Solution Explorer` pane, and then select `Manage NuGet Packages...`.
        - From `Browse` tab, search `Microsoft.ML.OnnxRuntime` (CPU) or `Microsoft.ML.OnnxRuntime.Gpu` (GPU) and install it.
    2. DLL direct download
        - You can download prebuilt DLLs from [here](https://github.com/microsoft/onnxruntime/releases).
        - Unzip downloaded `onnxruntime-win-x64-(gpu-)1.10.0.zip` and locate files on `libs\onnxruntime\lib\vs\x64\` .
        - Generate a project using ProjectGenerator, then all libs are linked correctly and all dlls are copied to `bin`.

## Tested environment
- oF 0.12.0 + Mac mini M2 + macOS Sonoma

## ToDo
- Test for Mactel
- Test for Windows
- Implement the use of MPS on macOS

## Reference Implementation
- I heavily referred [Lite.AI.ToolKit](https://github.com/DefTruth/lite.ai.toolkit) implementation.
