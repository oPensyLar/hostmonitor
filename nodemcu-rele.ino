#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>



struct host
{
   IPAddress host;
   String strHost;
   int downCounter;
   int rebootCounter;
   int powrForcCounter;
   int powrCounter;
   int lastSignal;
   int pin;
};


host anaranjada;
WiFiServer serv(56781);
int maxPings = 300;

int CheckDownHost(host equi)
{

    if(anaranjada.downCounter > maxPings)
    {

        anaranjada.downCounter = 0;
        anaranjada.rebootCounter++;
        maxPings = maxPings + 30;
        
        if(anaranjada.lastSignal == 0)
        {
          
            anaranjada.lastSignal = 1;
            anaranjada.powrCounter++;
            
            PowerMotherBoard(anaranjada.pin);
        }

        else
        {
           anaranjada.lastSignal = 0;
           anaranjada.powrForcCounter++;
           
           PowerForceMotherBoard(anaranjada.pin);
        }
        
    }

    return 0;

}



int PowerForceMotherBoard(int pin)
{

   pinMode(pin, OUTPUT);
   delay(9000);
   pinMode(pin, INPUT);
   
  return 0;
}




String IpAddress2String(const IPAddress& ipAddress)
{
  
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]);
  
}




void setup() 

{

  
  IPAddress ip (2, 2, 2, 6);
  anaranjada.host = ip;
  anaranjada.strHost  = IpAddress2String(ip);
  anaranjada.pin =  D2;
  //anaranjada.downCounter = 0;


  const char* pass = "2Y3oLzn9xF4FsQBVpo2Bu44sPfP7K8HA"; 
  const char* ssid = "vzKeMcNdLc9muxsA9D"; 


  Serial.begin(9600);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  pinMode(LED_BUILTIN, OUTPUT);
  
  serv.begin();

}


int BuildRespHost(WiFiClient client, host hosts)
{
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          
          
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head><title>Hosts Monitor </title>");
          client.println("<body>");
          client.println("<h1 align='center'>oPen syLar</h1>");
          client.println("<div style='text-align:center;'>");
          client.println("<br />");
                      

          String strTmp = "<div><b>Host:</b> "  + hosts.strHost +  "</div>";
          client.println(strTmp);

          strTmp = "<div><b>Down counter:</b> " + String(hosts.downCounter) + "</div>";
          client.println(strTmp);

          strTmp = "<div><b>Reboot counter: </b> " + String(hosts.rebootCounter) + " </div>";
          client.println(strTmp);


          strTmp = "<div><b>Power Force counter: </b> " + String(hosts.powrForcCounter) + " </div>";
          client.println(strTmp);
 


          strTmp = "<div><b>Power counter: </b> " + String(hosts.powrCounter) + " </div>";
          client.println(strTmp);

          
          client.println("<br />");
          

          client.println("<button onClick=\"javascript:window.open(window.location.protocol + '/d2=pfm')\">PowerForce MB</button>");           
          client.println("<button onClick=\"javascript:window.open(window.location.protocol + '/d2=pm')\">Power MB</button>");

          
          client.println("</div>");
          client.println("</body>");
          client.println("</html>");

          return 0;
            
}



int PowerMotherBoard(int pin)
{

   pinMode(pin, OUTPUT);
   delay(1000);
   pinMode(pin, INPUT);
   
   return 0;

}




int AnalizeReq(String strings, host equi)
{

  if(strings.indexOf("/d2=pm")  > -1)
  {
    PowerMotherBoard(equi.pin);
  }


  if(strings.indexOf("/d2=pfm")  > -1)
  {
    PowerForceMotherBoard(equi.pin);
  }

  
  return 0;
}



void loop() 
{

    CheckDownHost(anaranjada);
    
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);

    WiFiClient client = serv.available();


    if(client)
    {
            while(!client.available() && client.connected())
            {
                    delay(1);
            }
      
          client.flush();

          String strReq = client.readStringUntil('\r');

          AnalizeReq(strReq, anaranjada);
          BuildRespHost(client, anaranjada);
          
    }

    String strTmp;
    bool ret = Ping.ping(anaranjada.host);

    if(ret)
    {
      anaranjada.downCounter = 0;
    }
    

        else
        {
          anaranjada.downCounter++;
        }


   digitalWrite(LED_BUILTIN, HIGH);  
   delay(150);
   
}
