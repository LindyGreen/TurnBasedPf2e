#!/bin/bash

# Force NVIDIA GPU for Unreal Engine
export DRI_PRIME=1
export __NV_PRIME_RENDER_OFFLOAD=1
export __GLX_VENDOR_LIBRARY_NAME=nvidia
export VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/nvidia_icd.x86_64.json

# Disable Vulkan validation layers and enable eGPU-friendly settings
export VK_INSTANCE_LAYERS=""
export VK_LAYER_PATH=""
export VK_LOADER_DEBUG=""
# Reduce GPU memory pressure for eGPU stability
export UE_LOG_LEVEL=Warning

# Launch Unreal Editor
cd /home/rvh/RiderProjects/TurnBasedPf2e
/home/rvh/RiderProjects/UnrealEngine/Engine/Binaries/Linux/UnrealEditor GASLearning.uproject