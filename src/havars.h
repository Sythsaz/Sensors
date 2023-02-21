// This is the topic this program will send the state of this device to.
String stateTopic = "arduino" + "/stairs/motion/state";

void sendMQTTTemperatureDiscoveryMsg()
{
    // This is the discovery topic for this specific sensor
    String discoveryTopic = "homeassistant/sensor/arduino" + "/stairs/motion/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["name"] = "Arduino " + "Stairs" + "Motion";
    doc["stat_t"] = stateTopic;
    doc["dev_cla"] = "motion";
    doc["frc_upd"] = true;
    // I'm sending a JSON object as the state of this MQTT device
    // so we'll need to unpack this JSON object to get a single value
    // for this specific sensor.
    doc["val_tpl"] = "{{ value_json.motion|default(0) }}";

    size_t n = serializeJson(doc, buffer);

    client.publish(discoveryTopic.c_str(), buffer, n);
}