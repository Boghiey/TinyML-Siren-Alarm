#include <siren-detect_inferencing.h>
#define EIDSP_QUANTIZE_FILTERBANK 0
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 4

#include <PDM.h>
#include <siren-detect_inferencing.h> // Library

/** Audio structures */
typedef struct {
  signed short *buffers[2];
  unsigned char buf_select;
  unsigned char buf_ready;
  unsigned int buf_count;
  unsigned int n_samples;
} inference_t;

static inference_t inference;
static bool record_ready = false;
static signed short *sampleBuffer;
static bool debug_nn = false;
static int print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);

// Timer variables for LED hold time
unsigned long lastRedTime = 0;
unsigned long lastYellowTime = 0;
const unsigned long LED_HOLD_TIME =
    1000; // Time in milliseconds for LED signal

void setup() {
  Serial.begin(115200);
  Serial.println("Siren detector with LED alarm started...");

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  // Startup sequence (LED test)
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDB, LOW);
  digitalWrite(LEDG, HIGH);
  delay(500);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDB, LOW);
  digitalWrite(LEDG, HIGH);
  delay(500);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDB, HIGH);
  digitalWrite(LEDG, HIGH);
  delay(500);

  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDB, HIGH);
  digitalWrite(LEDG, HIGH);

  if (microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false) {
    ei_printf("ERR: Microphone error\n");
    return;
  }
}

void loop() {
  bool m = microphone_inference_record();
  if (!m)
    return;

  signal_t signal;
  signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
  signal.get_data = &microphone_audio_signal_get_data;
  ei_impulse_result_t result = {0};

  EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
  if (r != EI_IMPULSE_OK)
    return;

  if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)) {

    bool currentSicher = false;
    bool currentUnsicher = false;

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
      if (strcmp(result.classification[ix].label, "sirene") == 0) {
        float wert = result.classification[ix].value;

        if (wert > 0.95) {
          currentSicher = true;
        } else if (wert > 0.80) {
          currentUnsicher = true;
        }
      }
    }

    // --- Update stopwatch ---
    unsigned long currentMillis = millis(); // Get current time

    if (currentSicher) {
      lastRedTime = currentMillis; // Reset stopwatch for Red
      Serial.println(">>> ALARM TRIGGER: Siren detected!");
    } else if (currentUnsicher) {
      lastYellowTime = currentMillis; // Reset stopwatch for Yellow
      Serial.println(">>> WARNING TRIGGER: Siren possibly detected!");
    }

    // Control LEDs based on stopwatch ---
    // If the last Red alarm was less than 1 second ago -> KEEP RED ON
    if (currentMillis - lastRedTime < LED_HOLD_TIME) {
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDB, HIGH);
      digitalWrite(LEDG, HIGH);

      // Otherwise: If the last Yellow warning was less than 1 sec ago ->
      // KEEP YELLOW ON
    } else if (currentMillis - lastYellowTime < LED_HOLD_TIME) {
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDB, LOW);
      digitalWrite(LEDG, HIGH);

      // Otherwise: Both timers expired -> BACK TO GREEN
    } else {
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDB, LOW); // LEDB is our Green
      digitalWrite(LEDG, HIGH);
    }

    print_results = 0;
  }
}

static void pdm_data_ready_inference_callback(void) {
  int bytesAvailable = PDM.available();
  int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);
  if (record_ready == true) {
    for (int i = 0; i < bytesRead >> 1; i++) {
      inference.buffers[inference.buf_select][inference.buf_count++] =
          sampleBuffer[i];
      if (inference.buf_count >= inference.n_samples) {
        inference.buf_select ^= 1;
        inference.buf_count = 0;
        inference.buf_ready = 1;
      }
    }
  }
}

static bool microphone_inference_start(uint32_t n_samples) {
  inference.buffers[0] =
      (signed short *)malloc(n_samples * sizeof(signed short));
  inference.buffers[1] =
      (signed short *)malloc(n_samples * sizeof(signed short));
  sampleBuffer =
      (signed short *)malloc((n_samples >> 1) * sizeof(signed short));
  if (sampleBuffer == NULL)
    return false;
  inference.buf_select = 0;
  inference.buf_count = 0;
  inference.n_samples = n_samples;
  inference.buf_ready = 0;
  PDM.onReceive(&pdm_data_ready_inference_callback);
  PDM.setBufferSize((n_samples >> 1) * sizeof(int16_t));
  if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY))
    ei_printf("Failed to start PDM!");
  PDM.setGain(127);
  record_ready = true;
  return true;
}

static bool microphone_inference_record(void) {
  if (inference.buf_ready == 1)
    return false;
  while (inference.buf_ready == 0)
    delay(1);
  inference.buf_ready = 0;
  return true;
}

static int microphone_audio_signal_get_data(size_t offset, size_t length,
                                            float *out_ptr) {
  numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset],
                        out_ptr, length);
  return 0;
}