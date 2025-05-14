#include <Arduino.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <WebSocketsClient.h>

#include "logger.h"
#include "secrets.h"
#include "utils.h"

const char *target_mac_address = "F4:6D:04:9C:93:F8";

// Set web server port number to 80
WiFiServer web_server(80);

// set up WiFiUDP to send WOL messages
WiFiUDP UDP;
WakeOnLan WOL(UDP);

// Variable to store the HTTP request
String header;

// Timing for handling HTTP
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void wakeMyPC()
{
	WOL.setRepeat(3, 100);
	WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
	WOL.sendMagicPacket(target_mac_address, 9); // Send Wake On Lan packet with the above MAC address. Default to port 9.
}

void setup()
{
	Logger::init(115200);
	Logger::setLevel(LogLevel::INFO);

	Logger::infoln("Starting...");

	Utils::setupWIFI(
		C_WIFI_SSID,
		C_WIFI_PASSWORD,
		C_WIFI_LOCAL_IP,
		C_WIFI_GATEWAY_IP,
		C_WIFI_SUBNET_MASK,
		C_WIFI_PRIMARY_DNS,
		C_WIFI_SECONDARY_DNS
	);

	web_server.begin();
}

void loop()
{
	WiFiClient client = web_server.available(); // Listen for incoming clients

	if (client)
	{ // If a new client connects,
		currentTime = millis();
		previousTime = currentTime;
		Serial.println("New Client."); // print a message out in the serial port
		String currentLine = "";	   // make a String to hold incoming data from the client
		while (client.connected() && currentTime - previousTime <= timeoutTime)
		{ // loop while the client's connected
			currentTime = millis();
			if (client.available())
			{							// if there's bytes to read from the client,
				char c = client.read(); // read a byte, then
				Serial.write(c);		// print it out the serial monitor
				header += c;
				if (c == '\n')
				{ // if the byte is a newline character
					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0)
					{
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println("Connection: close");
						client.println();

						// Theck if the client asked to turn on PC
						if (header.indexOf("GET /on") >= 0)
						{
							Serial.println("Client requested to turn on the copmuter");
							wakeMyPC();
						}

						// Display the HTML web page
						client.println("<!DOCTYPE html><html>");
						client.println("<head><title>WakeOnLan</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
						client.println("<link rel=\"icon\" href=\"data:,\">");
						// CSS to style the on/off buttons
						// Feel free to change the background-color and font-size attributes to fit your preferences
						client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color: #0b0c1b; color: white;}");
						client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
						client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
						client.println(".button2 {background-color: #555555;}</style></head>");

						// Web Page Heading
						client.println("<body><h1>WakeOnLan</h1>");

						client.println("<p><a href=\"/on\"><button class=\"button\">Turn On Computer</button></a></p>");

						client.println("</body></html>");

						// The HTTP response ends with another blank line
						client.println();
						// Break out of the while loop
						break;
					}
					else
					{ // if you got a newline, then clear currentLine
						currentLine = "";
					}
				}
				else if (c != '\r')
				{					  // if you got anything else but a carriage return character,
					currentLine += c; // add it to the end of the currentLine
				}
			}
		}
		// Clear the header variable
		header = "";
		// Close the connection
		client.stop();
		Serial.println("Client disconnected.");
		Serial.println("");
	}
}