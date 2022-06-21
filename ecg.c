#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>
#include <string.h>
#include "ads1115.h"
#include <stdbool.h>
#include <stdlib.h>

//para pruebas de guardar en fichero. borrar luego
	FILE * fp;
bool tempint = false;

/* ADS1115 analogRead function
*===================================*/

int myAnalogRead (struct wiringPiNodeStruct *node, int pin) {
 
  int data[2];
  int value;

  // Start with default values
  int config = ADS1115_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1115_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1115_REG_CONFIG_DR_860SPS   | // 860 samples per second (max)
                    ADS1115_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
                    //ADS1115_REG_CONFIG_MODE_CONTIN;   // Continuous mode (not used here)
		 
  config |= ADS1115_REG_CONFIG_PGA_4_096V; // Set PGA/voltage range (ganancia).
  config |= ADS1115_REG_CONFIG_MUX_SINGLE_0; //usamos la entrada AIN0

  
  // Set 'start single-conversion' bit
  config |= ADS1115_REG_CONFIG_OS_SINGLE;
  
  // Sent the config data in the right order
  config = ((config >> 8) & 0x00FF) | ((config << 8) & 0xFF00);
  wiringPiI2CWriteReg16(node->fd, ADS1115_REG_POINTER_CONFIG, config);
 
  // Wait for conversion to complete. 
  usleep(2000); // (1/SPS rounded up)

  wiringPiI2CWrite(node->fd, ADS1115_REG_POINTER_CONVERT); // 
  data[0] = wiringPiI2CRead(node->fd);
  data[1] = wiringPiI2CRead(node->fd);
  value = ((data[0] << 8) & 0xFF00) | data[1];

  // wiringPi doesn't include stdint so everything is an int (int32), this should account for this
  if (value > 0x7FFF) {
    return (value - 0xFFFF);
  } else {
    return value;
  }
}

/* ADS1115 ADC setup:
 *    create ADS1115 device.
 *    id is the address of the chip (0x48 default)
*===============================================*/

int ads1115Setup(const int pinBase, int id) {
  struct wiringPiNodeStruct *node;

  node = wiringPiNewNode(pinBase,4);

  node->fd = wiringPiI2CSetup(id);
  node->analogRead = myAnalogRead;

  if (node->fd < 0) {
    return -1;
  } else {
    return 0;
  }
}


void sig_handler(int signum){
        if(signum == SIGALRM){ //si se ha producido una interrupcion por t=4ms, pongo el flag (tempint) a true
			tempint = true;
             
        }
}

void ini_timer(int tiempo)
{
        // Debe inicializar el tiempo
        struct itimerval time = {0};  

        // Establecer un temporizador único 
        time.it_value.tv_usec = tiempo;
        time.it_value.tv_sec = 0;
        time.it_interval.tv_sec = 0;
        time.it_interval.tv_usec = tiempo;

        signal(SIGALRM , sig_handler); //cuando venza la temporizacion llamara a la funcion sig_handler

        setitimer(ITIMER_REAL , &time , NULL); // 
  
}



void sendData (int fd, char *message)
{
serialPrintf(fd,message); //envia los datos en modo AT(remoto) al HAT
printf(message); //envia los datos en modo local

}

void parpadeo (void)
{
for (int i=0;i<5;i++)
  {	
  //Hace un parpadeo del LED al arrancar para informar al usuario que puede empezar la medida
    digitalWrite(17, HIGH);//ENCIENDO EL LED		
    usleep(100000); 
    digitalWrite(17, LOW);//APAGO EL LED 
    usleep(100000); 
  }
}


void iniHAT_sim8202 (int fd) {
		
		sendData(fd,"AT+CIPMODE=1\r\n");
		sleep(1);
		sendData(fd,"AT+NETOPEN\r\n");
		sleep(1);
		sendData(fd,"AT+CIPOPEN=0,\"UDP\",\"35.237.66.97\",8888,1880\r\n");
		sleep(1);
				}

int main(void) 
{
	
	int fd;
	double voltage;
	unsigned int timecont = 0;
	char tempvolt [10]; 
	char cuentastr [20];
	bool nuevaprueba = true;
	
	#define setTime (4000) //temporizacion periodica de 4 ms
	#define stopTime (0)
	
	
	fd = serialOpen("/dev/ttyUSB2",115200) ; //ABRE PUERTO USB CONECTADO AL HAT 

	ads1115Setup(100,ADS1115_ADDRESS); //inicializa ADS1115
  
	wiringPiSetupGpio(); // Inicializa wiringPi usando numeración Broadcom GPIO 
	pinMode(18, INPUT); // BOTON DE START
	pullUpDnControl(18, PUD_UP); // QUEREMOS PULLUP PORQUE AL PULSAR LEE UN O (NORMALMENTE ESTARA A 1)
	pinMode(17, OUTPUT);// LED MODO ON
	digitalWrite(17, LOW);//APAGO EL LED 
	 
	parpadeo();	
	
	// PRUEBA LOCAL PARA GUARDAR EN FICHERO
	//fp=fopen("/home/pi/grafica.txt","w");
	//fprintf(fp,"%s","time,voltage\n\r");
		
	while (1)
	{
		
		//Aqui se pregunta por el gpio del pulsador (START) y cuando este a 0 entra al for	
		if (!digitalRead(18))
		{
			usleep(50000); //retardo para evitar rebotes del pulsador
			digitalWrite(17, HIGH);//ENCIENDO EL LED
			 
			if (digitalRead(18)) // espero a que se suelte el pulsador (lee un 1)
				{
				iniHAT_sim8202 (fd); // inicializa HAT
				sendData(fd,"time,voltage\r\n"); //cabecera datos
				usleep(500000);
				ini_timer(setTime); //arrancar el temporizador de 4 ms
				nuevaprueba = true;
								
				while (nuevaprueba)
				{
					if (tempint) //ha vencido la temporización de 4ms
					{
						tempint = false;
						
				               // lee dato del conversor ADC y calcula el voltaje en función de la ganancia programada
						voltage = (int16_t) analogRead(100) * (4.096 / 32768);  
						gcvt(voltage,4,tempvolt); //convierte el valor de double a ascii
						sprintf (cuentastr,"%u", timecont);
						strcat(cuentastr,","); //concatenar string 
						strcat(cuentastr,tempvolt);
						//strcat(cuentastr,"\n\r");
						sendData(fd,cuentastr); //envia los datos
						timecont += 4; //cuenta de 4 en 4 ms
						
						//fprintf(fp,"%s",cuentastr); //guarda datos en fichero. borrar luego
						
						// aqui pregunto otra vez por el gpio del pulsador y si vuelve a pulsarse, acabar
						if (!digitalRead(18)) //al volver a pulsar el boton, fin de medidas y apagamos el led
						{
	
							ini_timer(stopTime); //para la temporización periódica
							digitalWrite(17, LOW);//APAGO EL LED 
							sleep(2); //espera 2 segundos para enviar +++ y finalizar modo transparente
							sendData(fd,"+++");//fin modo transparente
							sleep(2);
							sendData(fd,"AT+CIPCLOSE=O\r\n");
							sleep(1);
							sendData(fd,"AT+NETCLOSE\r\n");
							sleep(1);
						 
							//fclose(fp);//prueba local: cerrar fichero.
							//serialClose (fd) ; //cerramos el puerto serie
							parpadeo();	
							nuevaprueba = false;
							timecont = 0;
							//return 0;
						}
					}

				}
 
			}
		}
	}
	return 0;
}


