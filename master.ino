/*******************************************************************************************
 **                                                                                       **
 ** Author: Jonas Burkhalter                                                              **
 ** Date: 30. March 2021                                                                  **
 ** Github: https://github.com/jonas-burkhalter/jsa.timemachine                           **
 ** Version: 0.1                                                                          **
 **                                                                                       **
 ** Libraries:                                                                            **
 ** uMQTTBroker: https://github.com/martin-ger/uMQTTBroker                                **
 ******************************************************************************************/

///////////////////
// CONFIGURATION //
///////////////////
int number = 0;

// Wifi //
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
const char *SSID = "timemachine";
const char *PASSWORD = "altisberg";

// Server //
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

// Broker //
#include "uMQTTBroker.h"
uMQTTBroker broker;
unsigned long heartbeat_previous;
const int HEARTBEAT_TIMEOUT = 1000;
const char *TOPIC_HEARTBEAT = "timemachine/heartbeat";

// Trigger //
const int TRIGGER_PIN = D1;

///////////
// Setup //
///////////
void setup() {
  Serial.begin(115200);
  Serial.println("POWER: " + String(POWER));
  
  // Wifi //
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);
  
  // Broker //
  broker.init();

  // Server //
  server.on("/", handleRoot);
  server.on("/getnumber", handleGetNumber);
  server.on("/setnumber", handleSetNumber);
  server.begin();

  // Trigger //
  pinMode(TRIGGER_PIN, INPUT);
}
  
//////////
// Loop //
//////////
void loop() {  
  // Broker //
  heartbeat();
  
  // Server //
  server.handleClient();

  // Trigger //
  if(digitalRead(TRIGGER_PIN) == HIGH){
    // TODO
    // broker.publish("timemachine/number", (String)number); 
  } 
}

////////////
// Broker //
////////////
void heartbeat() {
  unsigned long now = millis();
  if ((now - heartbeat_previous) >= HEARTBEAT_TIMEOUT) {
    Serial.println("Broker.heartbeat: " + (String)now);
    broker.publish(TOPIC_HEARTBEAT, (String)now); 
    heartbeat_previous = now;
  }
}

////////////
// Server //
////////////
void handleRoot() {
  server.send(
    200, 
    "text/html", 
    "<!DOCTYPE html>"
    "<html lang='de'>"
    "  <head>"
    "    <meta charset='UTF-8'>"
    "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "    <title>Zeitmaschine</title>"
    "  </head>"
    "  <body>"
    "    <header>"
    "      <h1 onclick='getNumber()' style='cursor:pointer;'>Zeitmaschine &#8635;</h1>"
    "      <p id='year'>Jahr 0</p>"
    "    </header>"
    ""
    "    <main>"
    "      <form id='form'>"
    "        <h3>Neues Jahr</h3>"
    "        <input type='number' id='number' />"
    "        <input type='submit' value='Speichern'></input>"
    "      </form>"
    "    </main>"
    "    <footer>"
    "      <p>Wait a minute. Wait a minute Doc, are you telling me you built a time machine out of a DeLorean?</p>"
    "    </footer>"
    "  </body>"
    ""
    "  <script>"
    "        var form = document.getElementById('form');"
    "        var number = document.getElementById('number');"
    "        form.addEventListener('submit', function(evt) {"
    "            evt.preventDefault();"
    "            setNumber(number.value);"
    "        });"
    ""
    "        async function setNumber(number) {"
    "            let x = await fetch(`setnumber?number=${number}`);"
    "            document.getElementById('number').value = '';"
    "        }"
    ""
    "        async function getNumber() {"
    "            let x = await fetch(`getnumber`);"
    "            console.log(x);"
    "            let y = await x.text();"
    "            console.log(y);"
    "            document.getElementById('year').innerHTML = `Jahr ${y}`;"
    "        }"
    ""
    "        getNumber();"
    "  </script>"
    ""
    ""
    "<style>"
    "  /* Set the global variables for everything. Change these to use your own fonts/colours. */"
    "  :root {"
    "  "
    "    /* Set sans-serif & mono fonts */"
    "    --sans-font: -apple-system, BlinkMacSystemFont, 'Avenir Next', Avenir, 'Nimbus Sans L', Roboto, Noto, 'Segoe UI', Arial, Helvetica, 'Helvetica Neue', sans-serif;"
    "    --mono-font: Consolas, Menlo, Monaco, 'Andale Mono', 'Ubuntu Mono', monospace;"
    "  "
    "    /* Body font size. By default, effectively 18.4px, based on 16px as 'root em' */"
    "    --base-fontsize: 1.15rem;"
    "  "
    "    /* Major third scale progression - see https://type-scale.com/ */"
    "    --header-scale: 1.25;"
    "  "
    "    /* Line height is set to the 'Golden ratio' for optimal legibility */"
    "    --line-height: 1.618;"
    "  "
    "    /* Default (light) theme */"
    "    --bg: yellow;"
    "    --accent-bg: green;"
    "    --text: #212121;"
    "    --text-light: #585858;"
    "    --border: green;"
    "    --accent: green;"
    "  }"
    "  "
    "  /* Dark theme */"
    "  @media (prefers-color-scheme: dark) {"
    "    :root {"
    "      --bg: #212121;"
    "      --accent-bg: #2B2B2B;"
    "      --text: #DCDCDC;"
    "      --text-light: #ABABAB;"
    "      --border: #666;"
    "      --accent: #FFB300;"
    "    }"
    "  }"
    "  "
    "  html {"
    "    /* Set the font globally */"
    "    font-family: var(--sans-font);"
    "    font-size: 16px;"
    "  }"
    "  "
    "  /* Make the body a nice central block */"
    "  body {"
    "    color: var(--text);"
    "    background: var(--bg);"
    "    font-size: var(--base-fontsize);"
    "    line-height: var(--line-height);"
    "    display: flex;"
    "    min-height: 100vh;"
    "    flex-direction: column;"
    "    flex: 1;"
    "    margin: 0 auto;"
    "    max-width: 45rem;"
    "    padding: 0 .5rem;"
    "    overflow-x: hidden;"
    "    word-break: break-word;"
    "    overflow-wrap: break-word;"
    "  }"
    "  "
    "  /* Make the header bg full width, but the content inline with body */"
    "  header {"
    "    background: var(--accent-bg);"
    "    border-bottom: 1px solid var(--border);"
    "    text-align: center;"
    "    padding: 2rem .5rem;"
    "    width: 100vw;"
    "    position: relative;"
    "    left: 50%;"
    "    right: 50%;"
    "    margin-left: -50vw;"
    "    margin-right: -50vw;"
    "  }"
    "  "
    "  /* Remove margins for header text */"
    "  header h1,"
    "  header p {"
    "    margin: 0;"
    "  }"
    "  "
    "  /* Fix header line height when title wraps */"
    "  header h1 {"
    "    line-height: 1.1;"
    "  }"
    "  "
    "  footer {"
    "    margin-top: 4rem;"
    "    padding: 2rem 1rem 1.5rem 1rem;"
    "    color: var(--text-light);"
    "    font-size: .9rem;"
    "    text-align: center;"
    "    border-top: 1px solid var(--border);"
    "  }"
    "  "
    "  /* Format headers */"
    "  h1 {"
    "    font-size: calc(var(--base-fontsize) * var(--header-scale) * var(--header-scale) * var(--header-scale) * var(--header-scale));"
    "    margin-top: calc(var(--line-height) * 1.5rem);"
    "  }"
    "  "
    "  h3 {"
    "    font-size: calc(var(--base-fontsize) * var(--header-scale) * var(--header-scale));"
    "    margin-top: calc(var(--line-height) * 1.5rem);"
    "    text-align: center;"
    "  }"
    "  "
    "  input[type='submit'] {"
    "    border: none;"
    "    border-radius: 5px;"
    "    background: var(--accent);"
    "    font-size: 1rem;"
    "    color: var(--bg);"
    "    padding: .7rem .9rem;"
    "    margin: .5rem 0;"
    "    transition: .4s;"
    "  }"
    "  "
    "  input[type='submit']:focus,"
    "  input[type='submit']:enabled:hover{"
    "      opacity: .8;"
    "  }"
    "  input {"
    "    font-size: inherit;"
    "    font-family: inherit;"
    "    padding: .5rem;"
    "    margin-bottom: .5rem;"
    "    color: var(--text);"
    "    background: var(--bg);"
    "    border: 1px solid var(--border);"
    "    border-radius: 5px;"
    "    box-shadow: none;"
    "    box-sizing: border-box;"
    "    width: 100%;"
    "    appearance: none;"
    "    -moz-appearance: none;"
    "    -webkit-appearance: none;"
    "  }"
    "  @media only screen and (max-width: 720px) {"
    "    input {"
    "      width: 100%;"
    "    }"
    "  }"
    "  </style>"
    "</html>"
    );
}

void handleSetNumber() {
  number = server.arg("number").toInt();
  // TODO
  broker.publish("timemachine/number", (String)number); 
  server.send(200, "text/html");
}

void handleGetNumber() {
  server.send(200, "text/html", String(number));
}
