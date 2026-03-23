# Emergency Vehicle Siren Detection (TinyML Siren Detector)
## Overview
This project aims to detect emergency vehicle sirens (e.g., ambulance, police, fire truck) in real-time using a TinyML model deployed on an embedded device.

The system captures audio signals, classifies them as “siren” or “non-siren”, and triggers a signal to alert the driver or support autonomous driving systems.

## Objectives
- Real-time detection of siren sounds  
- Fully on-device processing (Edge AI / TinyML)  
- Low latency and low power consumption  
- Creation of a custom audio dataset

## System Architecture
1. Audio input is captured using the onboard microphone (Arduino Nano 33 BLE Sense)  
2. The signal is preprocessed, and features are extracted  
3. A trained TinyML model performs classification  
4. Output:
   - Siren detected → trigger signal  
   - No siren → no action
  
## Hardware
- Arduino Nano 33 BLE Sense  
- Built-in microphone  
- USB connection for power and data

## Software and Tools
- Edge Impulse (data collection, training, deployment)  
- Arduino IDE  

## Dataset
A custom dataset was created due to the lack of suitable existing datasets.
The dataset contains three types of audio recordings:
- Sirens (emergency vehicles in Germany)
- Traffic (cars, road noise, horns)
- Voices (conversations, music, podcasts)

These additional sound categories were intentionally included to represent realistic environmental noise and improve the robustness of the model.

## Model
Although the dataset consists of multiple sound categories, the task is formulated as a **binary classification problem**:

- Siren  
- Not Siren  

All non-siren classes (traffic and voices) are combined into a single "Not Siren" category during training.

This approach simplifies the model and focuses on the primary goal: reliable detection of emergency sirens in real-world environments.

The model was developed using Edge Impulse


## Installation and Setup

