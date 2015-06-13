/*
 * main.c
 * 
 * Main-Programm zum Ansteuern von Kamera, Blitz und Venitlen
 * 
 * Copyright 2015  <Markus May>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

//GPIO PINs festlegen
#define ausloeser 17
#define blitz 6
#define ventil1 23
#define ventil2 18

//Dateipfad festlegen
const char* filename = "DropShoot.txt";
const int maxanz = 100;

//Funktionen deklarieren
//Liest die Anzahl von Aktionen aus Datei
int actions_in_file();
//Aufteilen eines Strings nach einem bestimmten Zeichen
void splitline(string *, string); 

int main(int argc, char **argv)
{
	printf("Setup GPIO \n");
	wiringPiSetupGpio();
	
	printf("Setup Pins \n");
	pinMode(ausloeser, OUTPUT);
	pinMode(blitz, OUTPUT);
	pinMode(ventil1, OUTPUT);
	pinMode(ventil2, OUTPUT);
	
	digitalWrite(ausloeser,LOW);
	
	string line;
	int number_of_actions = 0;
	int c;
	while(true){
		system("/bin/stty raw");
		c = getchar();
		system("/bin/stty cooked");
		
		if(c== 116){
			ifstream file (filename);
			if (file.is_open()){
			//Zähle Linien
			number_of_actions = actions_in_file();
	        cout << "Anzahl Aktionen: " << number_of_actions << endl;
	        
	        //Lese Aktionen
	        int ntmp = number_of_actions;
	        
	        //Array, dass alle Aktionen enthält
			string arr_action[ntmp][3];		
		    int i = 0;	
			while(file.good()){			
				if(getline(file, line))	{ 					
					int k = 0;
					splitline(arr_action[i], line);
					i++;	
				}					
			}
			file.close();
			
			//Führe Aktionen aus
			cout << "Starte Aktionen" << endl;
			//Durchlaufen des Arrays mit den Aktionen
			for(int k = 0; k < number_of_actions; k++){
				if(arr_action[k][0] == "c"){
					cout << "Kameravorhang oeffnen/schliessen" << endl;
					//Kamera auslösen
					digitalWrite(ausloeser, atoi(arr_action[k][1].c_str()));
				}
				else if(arr_action[k][0] == "w"){
					cout << "Warten" << endl;
					//Warten
					delay(atoi(arr_action[k][2].c_str()));	
				}
				else if (arr_action[k][0] == "f"){
					cout << "Blitz ausloesen" << endl;
					//Blitz auslösen	
					digitalWrite(blitz, HIGH);
					delay(atoi(arr_action[k][2].c_str()));
					digitalWrite(blitz, LOW);					
				}
				else if  (arr_action[k][0] == "d1"){
					cout << "Tropfen 1" << endl;
					//Ventil 1 auslösen
					digitalWrite(ventil1, HIGH);
					delay(atoi(arr_action[k][2].c_str()));
					digitalWrite(ventil1, LOW);
				}
				else if  (arr_action[k][0] == "d2"){
					cout << "Tropfen 2" << endl;
					//Ventil 2 auslösen
					digitalWrite(ventil2, HIGH);
					delay(atoi(arr_action[k][2].c_str()));
					digitalWrite(ventil2, LOW);
				}
				else if  (arr_action[k][0] == "d12"){
					cout << "Tropfen 1 und 2" << endl;
					//Ventil 1 und 2 gleichzeitig auslösen
					digitalWrite(ventil1, HIGH);
					digitalWrite(ventil2, HIGH);
					delay(atoi(arr_action[k][2].c_str()));
					digitalWrite(ventil1, LOW);
					digitalWrite(ventil2, LOW);
				}
				
			}
			cout << "Ende Aktion" << endl << endl;
			}else{
				cout << "Datei nicht gefunden" << endl;
			}
		}else if(c == 27){
			//Wenn ESC gedrückt wurde, wird das Programm benndet
			cout << "Programm beenden" <<endl;
			return 0;
		}
	}
	return 0;
}

//Öffnet die Datei und leist die Anzahl der Zeilen aus
int actions_in_file(){
	ifstream file (filename);
	string line;
	int cnt = 0;
	if (file.is_open()){
		//Zähle Linien
		while(!file.eof()){
            if(getline(file,line)){
            	cnt++;
			}
        }	
	}
	return cnt;
}

//Nimmt einen String entgegen und zerlegt ihn in ein Array, Trennzeichen ist das Komma
void splitline(string *arrline, string line){
	//string arr[3];
	string search = ",";
	int spacePos;
	int currPos = 0;
	int k = 0;
	int prevPos = 0;
	
	do{
	    spacePos = line.find(search,currPos);
	    if(spacePos >= 0)
	    {
	        currPos = spacePos;
	        arrline[k] = line.substr(prevPos, currPos - prevPos);
	        currPos++;
	        prevPos = currPos;
	        k++;
	    }
	}while( spacePos >= 0);
	arrline[k] = line.substr(prevPos,line.length());
}
