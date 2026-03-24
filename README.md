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
4. Output & Visual Feedback:
   - **Certain Siren (> 95% confidence)** → Triggers Red LED alarm (holds for 1s) and Serial output (`>>> ALARM TRIGGER: Siren detected!`)
   - **Uncertain Siren (> 80% confidence)** → Triggers Yellow LED warning (holds for 1s) and Serial output (`>>> WARNING TRIGGER: Siren possibly detected!`)
   - **No Siren (Normal state)** → LED remains Green
  
## Hardware
- Arduino Nano 33 BLE Sense  
- Built-in microphone  
- USB connection for power and data

## Software and Tools
- Edge Impulse (data collection, training, deployment)  
- Arduino IDE  

## Dataset
A custom dataset was created due to the lack of suitable existing datasets.  
**Note:** The model was currently trained specifically on **German emergency sirens**.

The dataset contains three types of audio recordings:
- Sirens (emergency vehicles in Germany)
- Traffic (cars, road noise, horns)
- Voices (conversations, music, podcasts)

These additional sound categories were intentionally included to represent realistic environmental noise and improve the robustness of the model. Notably, because car horns are explicitly grouped under the "Traffic" category, the model is highly resistant to false positives from honking cars.

## Model
Although the dataset consists of multiple sound categories, the task is formulated as a **binary classification problem**:

- Siren  
- Not Siren  

All non-siren classes (traffic and voices) are combined into a single "Not Siren" category during training.

This approach simplifies the model and focuses on the primary goal: reliable detection of emergency sirens in real-world environments.

The model was developed using Edge Impulse


## Installation and Setup

1. **Install the Arduino IDE**: Download and install the latest version from the [official website](https://www.arduino.cc/en/software).
2. **Install Board Support**:
   - Open Arduino IDE.
   - Go to **Tools > Board > Boards Manager**.
   - Search for **Arduino Mbed OS Nano Boards** and install it.
3. **Install the Edge Impulse Model Library**:
   - The TinyML model for this project is provided as an Arduino library named `ei-siren-detect-arduino-1.0.3.zip`.
   - In Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library...** and select the `ei-siren-detect-arduino-1.0.3.zip` file.
4. **Flash the Code**:
   - Open the `siren_alarm.ino` sketch.
   - Connect your **Arduino Nano 33 BLE Sense** via USB.
   - Select the correct board (**Arduino Nano 33 BLE**) and port under **Tools > Board** and **Tools > Port**.
   - Click **Upload** to compile and flash the code to the board.
5. **Monitor Output (optional)**:
   - Open the Serial Monitor (**Tools > Serial Monitor**) and set the baud rate to **115200**.
   - You should see the detector starting up and printing live classification results when sirens are detected.

## Learnings
- **Model Efficiency & Reusability**: Despite its small size, the model performs very well. It can easily be rebuilt, retrained, and reused using the same label structure.
- **Latency**: The real-time detection window takes approximately `1 second` to evaluate the audio and output a classification result.
- **Versatility**: The hardware prototype is extremely lightweight and portable. It can be used as an assistance system inside a car, attached to a bicycle, or even carried by pedestrians while walking.

## Future Work & Contribution Ideas
We welcome contributions and new ideas to expand the project! Some ideas for future development include:

- **Directional Detection**: Adding multiple microphones to calculate sound vectors and detect the exact direction an emergency vehicle is approaching from.
- **Accessibility Warnings**: Integrating the system into wearables (smart glasses, smartwatches) to provide visual or haptic feedback, allowing people with hearing disabilities to receive clear, immediate warnings.
- **Performance Upgrades**: Using a more powerful microcontroller or edge-AI device to further reduce inference latency.
- **Expanded Sound Profiles**: Creating a larger model with dedicated labels to distinctively classify other critical sounds (e.g., isolating a "car horn" class instead of grouping it in traffic).
- **Internationalization**: Expanding the training dataset to include international sirens (e.g., US, UK, France) for a globally applicable model.
- **Smart City Integration**: Syncing detection telemetry to the cloud to map emergency vehicle routes in real-time and optimize smart traffic light controls.
- **Ultra Low-Power Wake Words**: Implementing a basic analog sound trigger or an ultra-tiny cascade model to keep the main classifier in deep sleep, massively extending battery life for portable use.
