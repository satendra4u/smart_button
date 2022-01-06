// This #include statement was automatically added by the Particle IDE.
#include <SparkFunMAX17043.h>

// This #include statement was automatically added by the Particle IDE.
#include "SparkFunMAX17043/SparkFunMAX17043.h"

// Battery test
// By Satyendra Kumar
#include "application.h"    // http://docs.xamarin.com/ios/tutorials/Working_with_the_File_System
#define lowBattery 10       // %


SYSTEM_THREAD(ENABLED);
STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));


//--SYSTEM_MODE(MANUAL);
#define Button_Switch D2
#define LED D7

double voltage = 0;
double soc = 0;
bool alert;

// Settings
#define debounceTime 500        //  milliseconds
#define updatePeriod 1000       //  milliseconds
#define sleepAfter 30           //  Seconds
#define sleepTime 2             //  Minutes
#define lowBattery 10           //  %


// Data to retain in deep sleep
retained long int timeSleep = 0;
retained volatile int pulseCount = 0;
retained volatile int pulseToday = 0;
bool runOnce = TRUE;
long int timeStartup = 0;

// Other variables
int prevCount = pulseCount;
long lastPulse;
long lastUpdate;

bool battery;

String data = ""; //used for all publish statements


void setup()
{
    
    pins_init();
                    
    // To read the values from a browser, go to:
	// http://api.particle.io/v1/devices/{DEVICE_ID}/{VARIABLE}?access_token={ACCESS_TOKEN}

	// Set up the MAX17043 LiPo fuel gauge:
	lipo.begin(); // Initialize the MAX17043 LiPo fuel gauge

	// Quick start restarts the MAX17043 in hopes of getting a more accurate
	// guess for the SOC.
	lipo.quickStart();

	// We can set an interrupt to alert when the battery SoC gets too low.
	// We can alert at anywhere between 1% - 32%:
	lipo.setThreshold(10); // Set alert threshold to 10%.        
	
	battery = FALSE;
           
}



// Spark loop
void loop()
    {
  
       // Particle.publish("The value of runOnce", String(runOnce),60,PRIVATE);
        
       if (Particle.connected() && runOnce)
        {
                Particle.publish("Button is on !", "Online", 60, PRIVATE);
                runOnce = FALSE;
        }   
        
                digitalWrite(LED, HIGH);
                delay(15000);
                digitalWrite(LED, LOW);
        
                timeStartup = Time.now();
                int difference = (int) (timeStartup - timeSleep);
                Particle.publish("DIFF", String(difference), 60, PRIVATE);
        
        if (difference >= 134 && difference <= 136)
        {
                battery = TRUE;
        }
        else
        {
                battery = FALSE;
        }
        //Particle.publish("Battery_Status",String(battery),60, PRIVATE);
            
        // Run once after cloud connected
        if (battery){
                
                lipo.begin();
                lipo.wake();
                lipo.quickStart();
                lipo.setThreshold(lowBattery);
            
                if (lipo.getAlert()) Particle.publish("Low Battery", NULL, 60, PRIVATE);
    
                Particle.publish("Status", data.format("Battery %1.2fV %2.1f%% " , lipo.getVoltage(), lipo.getSOC()), 60, PRIVATE);
    
                String slept = Time.format(timeSleep, "%I:%M.%S%p");
                String woke = Time.format(timeStartup, "%I:%M.%S%p");
                String diff = String(timeStartup - timeSleep);
            
                Particle.publish("SleepInfo", String("Slept at " + slept + ", Woke at " + woke + ", Difference of" + diff + "seconds"), 60, PRIVATE);
    
             
                delay(1500);
        }
        else 
        {
               //Particle.publish("Button", "On", 60, PRIVATE);
                digitalWrite(LED, HIGH);
                delay(15000);
                digitalWrite(LED, LOW);
         
        }

       if (Particle.connected()) {
        
                lipo.sleep();
                timeSleep = Time.now();
                System.sleep(Button_Switch, FALLING, (sleepTime * 60)); 
        }
     
}

// initialize our pins

void pins_init()
{
  pinMode(Button_Switch, INPUT_PULLUP);
  pinMode(LED, OUTPUT);    //The Piezo Buzzer is an Output
}



