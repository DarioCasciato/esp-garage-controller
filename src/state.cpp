// =============================================================================
// Template-Project | State
// =============================================================================

#include "state.h"
#include "hardware.h"
#include "configurations.h"
#include "Timer.h"
#include "../utils/Arduino/gpioHandler.h"
#include "../utils/Connectivity/ESPWiFi/espWiFi.h"
#include "Logging.h"
#include <PubSubClient.h>

//------------------------------------------------------------------------------

const char* TOPIC_TRIGGER = "garage/trigger";  // MQTT Topic

namespace State
{
    WiFiClient espClient;
    PubSubClient mqttClient(espClient);



    States state = States::st_idle;

    void stateDriver()
    {
        switch (State::state)
        {
        case State::st_connect: stateConnectWifi(); break;
        case State::st_idle: stateIdle(); break;
        case State::st_error: stateError(); break;

        default:    // catch invalid state (implement safety backup)
        goto exception;
            break;
        }

        return;

        exception:
            for(;;) { delay(1000); }
    }


    // State implementations (can also be moved to separate files)
    void stateConnectWifi()
    {
        Logging::log("Attempting to connect to WiFi...");

        if(Wifi::establish(WIFI_SSID, WIFI_PASSWORD))
        {
            State::state = States::st_idle;
            Logging::log("WiFi connected successfully.");
        }

        delay(1000);
    }

    void stateConnectMQTT()
    {
        Logging::log("Attempting to connect to MQTT Broker...");

        mqttClient.setServer(MQTT_HOST, MQTT_PORT);
        mqttClient.setCallback(onMqttMessage);

        if(mqttClient.connect("ESP_Garage_Controller"))
        {
            State::state = States::st_idle;
            Logging::log("MQTT connected successfully.");

            // Subscribe to topics
            mqttClient.subscribe(TOPIC_TRIGGER);
        }
        else
        {
            Logging::log("MQTT connection failed, rc=%d", mqttClient.state());
        }

        delay(1000);
    }

    void stateIdle()
    {
        // Handle MQTT client loop
        if (!mqttClient.connected() || !Wifi::isConnected())
        {
            State::state = States::st_connect;
            return;
        }

        mqttClient.loop();
    }

    void stateError()
    {

    }
} // namespace State

//------------------------------------------------------------------------------

// Other Functions


void pulseRelay()
{
    GPIO::setPort(Hardware::Port::Relay, HIGH);
    delay(RELAY_TIMESPAN);
    GPIO::setPort(Hardware::Port::Relay, LOW);
}

void onMqttMessage(char* topic, byte* payload, unsigned int length)
{
    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    Logging::log("MQTT Message received on topic %s: %s", topic, message.c_str());

    if (String(topic) == TOPIC_TRIGGER && message == "PULSE")
    {
        pulseRelay();
        Logging::log("Relay pulsed due to MQTT trigger.");
    }
}