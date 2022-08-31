#include<ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#define rx D2
#define tx D3
float temp;
int mq9;
int mq2;
int i=0;
int load;
String inString = "";
String header;
int cnt=0;
SoftwareSerial nodeSerial (rx, tx); // RX | TX

int flag=0;
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


const char* ssid = "iot";// 
const char* password = "123456789";
char host[]="www.login.bulksmsgateway.in";
WiFiServer server(80);
 WiFiClient client;

void setup() {


  Serial.begin(9600);
   
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  //Serial.println("Web server running. Waiting for the ESP IP...");
  delay(1000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}


  //$T26M56Q61L294#
void string_filter(String buff)
  {
Serial.println(buff);
delay(50);
    if(buff[0]=='$')
    if(buff[1]=='T')
    if(buff[4]=='M')
    if(buff[7]=='Q')
    if(buff[10]=='L')
    if(buff[13]=='#')
    {    
      String temp1=buff.substring(2,4);
        String temp2=buff.substring(5,7);
        String temp3=buff.substring(8,10);
        String temp4=buff.substring(11,13);
        temp=temp1.toFloat();
        mq9=temp2.toInt();
        mq2=temp3.toInt();
        load=temp4.toInt();        
       Serial.println(temp);
       Serial.println(mq9);
       Serial.println(mq2);
       Serial.println(load);
       if(temp>=40 || mq9>=50 || mq2>=20)
       {
        if(flag==0)
        {
        sms();
        i+=1;
        if(i>=2)
        {
          flag=1;
        }
        }
       }  
       
  }
  }
  
void getdata() {
while (nodeSerial.available()) {

    char c = nodeSerial.read();

     if (c == '\n') {
                            string_filter(inString);
                            inString = "";
                            cnt=0;
                            break;
                 }
                else
                 {
                 inString +=c;
                 cnt++;
                  }
      
  
    }

}
// runs over and over again

void loop(){
  getdata();
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // Web Page Heading
            client.println("<body><h1>PREDICTION AND MONITORING OF MEDICAL STORAGE SYSTEM USING IOT AND ML</h1>");
            
                        client.println("<h3>Temperature = ");
            client.println(temp);
            client.println("<h3>MQ 9 = ");
            client.println(mq9);
                        client.println("<h3>MQ 2 = ");
            client.println(mq2);
            
                        client.println("<h3>Load  = ");
            client.println(load);
            client.println("</h3></body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
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
  void sms(){
    if (client.connect(host, 80)) {
    //Serial.println("connected");
   String url = "https://rest.nexmo.com/sms/json?from=18664797424&to=15734667123&api_key=cf15a5ea&api_secret=ZJ8wPv8RwYMDKVL1&text=";
  url +="Temperature=";
  url +=temp;
  url += "Mq9=";
  url +=mq9;
    url += "Mq2=";
  url +=mq2;
      url += "Load=";
  url +=load;
  // This will send the request to the server
  //Serial.print("Requesting URL: ");
  //Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
                 Serial.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  } else {
    
    //Serial.println("connection failed");
  }
  client.stop();
 }
  
