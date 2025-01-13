#include <SPI.h>
#include <RadioLib.h>

// Initialize the SX1262 radio module
SX1262 radio = new Module(3, 20, 15, 2, SPI1, RADIOLIB_DEFAULT_SPI_SETTINGS);

const size_t MAX_QUEUE_SIZE = 10;
const size_t MAX_MSG_LENGTH = 255;

char messageQueue[MAX_QUEUE_SIZE][MAX_MSG_LENGTH];
size_t queueHead = 0;
size_t queueTail = 0;

bool transmitting = false;
volatile bool actionDone = false;
volatile bool receivedFlag = false;

void setFlag() {
  if (transmitting) {
    // Transmission has finished
    actionDone = true;
  } else {
    // Packet received
    receivedFlag = true;
  }
}

void enqueueMessage(const char* message) {
  if ((queueTail + 1) % MAX_QUEUE_SIZE == queueHead) {
    Serial.println("Queue full, cannot enqueue message.");
    return;
  }
  strncpy(messageQueue[queueTail], message, MAX_MSG_LENGTH - 1);
  messageQueue[queueTail][MAX_MSG_LENGTH - 1] = '\0';
  queueTail = (queueTail + 1) % MAX_QUEUE_SIZE;
  //Serial.println("Enqueued message: " + String(message));
}

char* dequeueMessage() {
  if (queueHead == queueTail) {
    Serial.println("Queue empty, no message to dequeue.");
    return nullptr;
  }
  char* message = messageQueue[queueHead];
  queueHead = (queueHead + 1) % MAX_QUEUE_SIZE;
  return message;
}

void transmit(const char* data) {
  Serial.println("[SX1262] Starting transmission of: " + String(data));
  int dataLength = strlen(data);
  if (dataLength > 256) {
    Serial.println("Data too long, truncating.");
    dataLength = 256;
  }
  int state = radio.startTransmit((uint8_t*)data, dataLength);
  //Serial.println("startTransmit() state: " + String(state));
  if (state != RADIOLIB_ERR_NONE) {
    Serial.println("[SX1262] startTransmit() failed, code " + String(state));
    radio.startReceive();
    transmitting = false;
  } else {
    transmitting = true;
    Serial.println("[SX1262] Message transferred");
  }
}

void receive() {
  if (receivedFlag) {
    receivedFlag = false;
    int length = radio.getPacketLength();
    if (length <= 1) {
      radio.startReceive();
      return;
    }
    uint8_t buffer[256];
    if (length > 256) length = 256;
    int state = radio.readData(buffer, length);
    if (state == RADIOLIB_ERR_NONE) {
      //Serial.println("[SX1262] Received packet!");
      Serial.println("[SX1262] RX Data: " + String((char*)buffer));
      //Serial.println("[SX1262] RSSI:\t" + String(radio.getRSSI()) + " dBm");
      //Serial.println("[SX1262] SNR:\t" + String(radio.getSNR()) + " dB");
    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      Serial.println("[SX1262] CRC error!");
    } else {
      Serial.println("[SX1262] Receive failed, code " + String(state));
    }
    radio.startReceive();
  }
}

void setup() {
  SPI1.setSCK(10);
  SPI1.setTX(11);
  SPI1.setRX(12);
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  SPI1.begin(false);

  Serial.begin(9600);
  while (!Serial) {}

  Serial.println("Start");

  Serial.println("[SX1262] Initializing ...");
  int initState = radio.begin(868.0);
  if (initState != RADIOLIB_ERR_NONE) {
    Serial.println("Radio init failed, code " + String(initState));
    while (true) {}
  }
  delay(1000);
  Serial.println("[SX1262] Radio init success");

  radio.setDio1Action(setFlag);

  radio.setFrequency(868.0);
  radio.setBandwidth(500.0);
  radio.setSpreadingFactor(12);
  radio.setCodingRate(8);
  radio.setPreambleLength(8);
  radio.setSyncWord(0x12);
  radio.setCRC(true);

  radio.startReceive();

  Serial.println("Just chat with the other devices.");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input.length() > 0) {
      enqueueMessage(input.c_str());
    }
  }

  if (!transmitting && queueHead != queueTail) {
    char* message = dequeueMessage();
    if (message != nullptr) {
      transmit(message);
    }
  }

  if (actionDone) {
    actionDone = false;
    transmitting = false;
    radio.startReceive();
    // Check if there are more messages to send
    if (queueHead != queueTail) {
      char* nextMessage = dequeueMessage();
      if (nextMessage != nullptr) {
        transmit(nextMessage);
        transmitting = true;
      }
    }
  }

  receive();
}