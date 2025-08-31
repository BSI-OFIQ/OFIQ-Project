# Fast Inference, Measure‑Driven Execution, and Logging

## Overview
Execution is now measure‑driven. Only modules required by selected measures are initialized and run. This reduces latency and memory for subset configurations while preserving default behavior for full configurations.

## Measure Dependencies
- Pose: `HeadPose`, `HeadPoseYaw`, `HeadPosePitch`, `HeadPoseRoll`, `InterEyeDistance`, `EyesVisible`
- Face Parsing: `BackgroundUniformity`, `NoHeadCoverings`
- Face Occlusion Segmentation: `FaceOcclusionPrevention`, `MouthOcclusionPrevention`, `EyesVisible`

When none of the dependent measures are selected, the corresponding module is not instantiated and its preprocessing step is skipped.

## Jetson Defaults (aarch64)
- ONNX Runtime CUDA provider by default
- cuDNN convolution algorithm search: Default (faster initialization)
- Copies use default CUDA stream
- TensorRT remains opt‑in (recommended only with prebuilt engines)

## Configurable Logging
Runtime logging is configurable via config or environment variables.

- Levels: `off`, `error`, `warn`, `info`, `debug`
- Categories (optional): `init`, `providers`, `executor` (and others as added). Empty list = allow all.

Config keys:
```json
"logging": {
  "level": "info",
  "categories": ["init", "providers"]
}
```

Environment variables (fallback):
```bash
export OFIQ_LOG_LEVEL=debug
export OFIQ_LOG_CATEGORIES=init,providers
```

## Examples
- Provider messages only: `level=info`, `categories=["providers"]`
- Full debug across categories: `level=debug` (omit categories)

## Performance Notes (Jetson Orin)
- UnifiedQualityScore‑only: ~211 ms mean per image (detector + ADNet + MagFace)
- All measures: ~617 ms mean per image

Exact numbers depend on device, warmup, and model cache.
