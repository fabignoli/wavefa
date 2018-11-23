//---------------------------------------------------------------
// PROGRAME wavefa.ino
// Synthetiseur monophonique a table ondes
// choix de la table onde en memoire
// 2 oscillateurs
//---------------------------------------------------------------
// Reglage prevus
// 2 LFO
// ADSR
// Pitch oscillateur 1 et 2 sur + ou - 1 octave
// fine pich oscillateur 2
// offset forme onde 1 et 2
// changement table onde pour oscillateur 1 et 2
//---------------------------------------------------------------
// fonctionnalites
// jeu en midi
// prise en compte control change sur certaines fonctions
// sauvegarde preset
// mode unison avec 1 seul oscillateur en quadruplant l'addition
// avec un decalage de pich dans le calcul du sample
//---------------------------------------------------------------
// 01/11/13: LFO1 sur VCA
// 17/10/13: Lecture des formes ondes en EEPROM
// 16/10/13: sauvegarde et chargement des presets
// 15/10/13: Refonte des menus
// 13/10/13: Ajout du pitch bend
//---------------------------------------------------------------
#include <MemoryFree.h>

#define LEDOFF	PORTD &= ~(1<<3)
#define LEDON	PORTD |= (1<<3)
#define GATEOFF	PORTD &= ~(1<<2)
#define GATEON	PORTD |= (1<<2)

// Declaration echantillonnage
//----------------------------
uint16_t sample = 0;
uint16_t samplet = 0;				// Echantillon temporaire (pour changement de gain et ADSR)
uint16_t samplet1 = 0;				// Echantillon temporaire (pour changement de gain et ADSR)
volatile uint16_t VCA_12bits=0;
volatile uint16_t VCA=0;


// Varables enveloppe
//-------------------------------
//uint16_t val_ADSR[] = {0,0,1000,0};		// Valeur ADSR
uint16_t val_ADSR[] = {0,170,300,210};		// Valeur ADSR
uint16_t max_ADSR[] = {4000,4000,1000,4000};		// Valeur max ADSR en ms ou 1000 max pour sustain
byte paramadsr=1;		// Position dans ADSR
#define NONOTE  0		// Pas de note jouee
#define ATTAQUE 1		// Phase attaque en cours
#define DECAY   2		// Phase de decay en cours
#define SUSTAIN 3		// Phase de sustain en cours
#define RELEASE 4		// Phase de release en cours
#define GATED	5		// Declenchement de lenveloppe
#define NOGATED 6		// Relachement enveloppe
#define pATTAQUE 0		// Phase attaque en cours
#define pDECAY   1		// Phase de decay en cours
#define pSUSTAIN 2		// Phase de sustain en cours
#define pRELEASE 3		// Phase de release en cours
byte ADSR;				// Etat actuel de l'enveloppe ADSR
/*
float incr_attaque;		// Incrementation pour attaque
float decr_decay;		// Decrementation pour arriver au sustain
float decr_release;		// Decrementation pour le release
float vol_sustain;		// Volume du sustain
float last_VCA_Mult=0.0;	// Sauvegarde du coefficient VCA
*/

uint16_t last_VCA_12bits;
uint16_t decr_release2;
uint16_t decr_decay2;
uint16_t incr_attaque2;
uint16_t vol_sustain2;



// Variables gestion du VCA
//-------------------------
//volatile float VCA_Mult=0.0;			// Coefficient multiplicateur pour le VCA
volatile byte VCA_OSC1=100;			// Coefficient multiplicateur pour l'onde 1
volatile byte VCA_OSC2=100;			// Coefficient multiplicateur pour l'onde 1
// Variable LFO
//-------------
unsigned char daclevel=0;
#define FLOW1	16					// Valeur timer lent
#define FLOW2	32					// Valeur timer lent 2 pour encodeur
unsigned char ls_cnt1=0;			// timer len
boolean f_lowspeed=false;			// Flag execution tache lente
unsigned int tcnt1;               	// Sauvegarde valeur counter 1
byte step_lfo=0;					// Modulation en cours
#define MODPWM	0					// Modulation du PWM
#define MODPITCH 1					// Modulation du PITCH
#define MODPHASE 2					// Modulation de la phase
#define MODVCA 3					// Modulation de l'amplitude
#define MAXMODS	  3					// Nombre de modulation -1
volatile byte inclfo1=0;			// Pointeur table sin LFO1
volatile byte inclfo2=0;			// Pointeur table sin LFO2
volatile byte lfo1=127;				// frequence du LFO 1
volatile byte lfo2=127;				// frequence du LFO 2
byte val_lfo1=0;					// valeur du lfo 1
byte val_lfo2=0;					// valeur du lfo 2
byte last_val_lfo1=0;				// precedente valeur du lfo 1
byte last_val_lfo2=0;				// precedente valeur du lfo 2
byte depth_lfo1=127;				// profondeur lfo1
byte depth_lfo2=127;				// profondeur lfo2
byte dest_lfo1=0;					// profondeur lfo1
byte dest_lfo2=1;					// profondeur lfo2
byte paramlfo1=1;
byte paramlfo2=1;
byte myByte1,myByte2;				// Byte utilisateur
int bend=0;							// valeur du bend
int bendOsc1=0;						// Pitch bend oscillateur 1
int bendOsc2=0;						// Pitch bend oscillateur 2
int pitchOsc1=0;					// modulation pitch oscillateur 1
int pitchOsc2=0;					// modulation pitch oscillateur 2
int vcaOsc=0;						// Modulation VCA des 2 oscillateurs (global sur tension DAC 1)
byte phase1=0;						// Phase oscillateur 1
byte phase2=0;						// Phase oscillateur 2



unsigned char ls_lfo1=0;			// timer lfo1
unsigned char ls_lfo2=0;			// timer lfo2

// Variables acceleration convertisseur ADC
/*
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
*/


// Declaration pour les tables ondes
//----------------------------------
#include "wavefa.h"
uint8_t wavetables[512]; 		// 2 wavetables de 256 octects concatenees
// Valeur des increments de chaque oscillateur
// La valeur est : (freq * 2^16 /15625) ou (freq * 65536 / 15525)
volatile uint16_t incr[] = {1097, 1644};// Increment des oscillateurs (C4 et E4 a l'initialisation)
volatile uint16_t pos[] = {0,0,0,0};	// position des oscillateurs
//volatile byte OffsetTable[] = {0,0}; 	// Offset pour parcourir la table d'onde
#define MAXTABLES 43

//byte notable[] = {0,0};					// Numero table onde a charger ou a calculer (sinus par defaut)
byte notable[] = {1,12};					// Numero table onde a charger ou a calculer (sinus par defaut)
byte pwm[] = {126,132};					// valeur de PWM (128= square, valeur min 10, valeur max 245)
byte unison;							// Mode unison 
byte decunison1,decunison2;				// decalage des accumulateurs en unison
byte depth_unison=1;					// Profondeur unison
#define PRESETSIZE 64					// taille d'un preset

// Variables presets
// -----------------
byte noPreset=0;                        // Numero du preset courant
char nomPreset[18];                     // Nom du preset courant
byte posCarPreset=0;                    // Position x edition nom preset

//----------------------------------------------
// Bibliotheque et definitions pour gerer le LCD
//----------------------------------------------
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif
LiquidCrystal_I2C lcd(0x27,16,2);  // sLCD adress 0x27, 16 car, 2 ligne
byte rafLCD;                        // Flag rafraichissement LCD
boolean raflcd1=false;				// Flag rafraichissement dans les menus
boolean raflcd2=false;				// flag supplementaire

uint8_t symbup[8]  = {0x4,0xe,0xf5,0x4,0x4,0x4,0x4};    //car fleche up
uint8_t symbdown[8]  = {0x4,0x4,0x4,0x4,0xf5,0xe,0x4};  //car fleche down
#define CAR_RIGHT 126   // Numero car fleche vers la droite 
#define CAR_LEFT  127   // Numero car fleche vers la gauche
#define CAR_UP    0     // Numero car fleche vers le haut
#define CAR_DOWN  1     // Numero car fleche vers le bas
#define CAR_SEP   124   // Trait vertical de séparation

// Gestion du midi
//----------------
#include <MIDI.h>   // Midi Library
#define MONO    0   // note on sur note active -> pas de GATE 
#define RETRIG  1   // note on sur note actice -> redeclenchement dun GATE

// Midi
byte LastCountNote; // Derniere valeur de midi note pour voir la difference                      
byte ModeMidi;      // Mode MONO ou RETRIG
byte CountNote=0;	// Nombre de note jouees sur le clavier
byte lastNote;		// derniere note jouee
byte semi[] = {12,12};	
// ENCODEURS ET BOUTONS
//---------------------
int pinA[] = {5,7};               	// Pins A des encodeurs
int pinB[] = {4,6};               	// Pins B des encodeurs
volatile int lastEncoded[] = {0,0}; // Dernieres valeur lue encodeurs
volatile int counter[] = {0,0};     // Valeur courante des encodeurs
int bcounter[] = {0,0};             // valeurs encodeurs divisees par 4
int lcounter[] = {0,0};             // valeurs précédentes des encodeurs
byte valbouton;                     // Numero du bouton appuye
byte lastBoutonNav=0;       // Stockage du dernier etat dun bouton
byte valencodeurs;                  // numero encodeur et sens
int enc1=0;
int enc2=0;
#define ENC1DOWN  1     // Encodeur 1 down
#define ENC2DOWN  2     // Encodeur 2 down
#define ENC1UP    11    // Encodeur 1 up
#define ENC2UP    12    // Encodeur 2 up

// menu racine
//------------
#define MENURACEDIT 0	  	// Menu racine edition
#define MENURACLOAD 1	  	// Menu racine load preset
#define MENURACSAVE 2	  	// Menu racine save preset
#define MINMENURAC 0	  	// maximum de menu a la racine
#define MAXMENURAC 2	  	// maximum de menu a la racine
// menu edition
//-------------
#define CHEDITWAVE   10     // Choix edition table onde
#define CHEDITAMPS   11     // Choix edition amplitude oscillateurs
#define CHEDITONE    12     // Choix edition accord oscillateurs
#define CHEDITPWM    13     // Choix edalition PWM
#define CHEDITADSR   14   	// Choix edition ADSR
#define CHEDITUNISON 15   	// Choix edition UNISON
#define CHEDITLFO1   16   	// Choix edition LFO1
#define CHEDITLFO2   17     // Choix edition LFO2
#define CHEDITDEBUG  18		// Choix debug
#define MINMENUREG 10	  	// minimum de menu pour les reglages
#define MAXMENUREG 18	  	// maximum de menu pour les reglages


#define EDITWAVE   110      // Mode edition table onde
#define EDITAMPS   111      // Mode edition amplitude oscillateurs
#define EDITONE    112      // Mode edition accord oscillateurs
#define EDITPWM    113      // Mode edalition PWM
#define EDITADSR   114   	// Mode edition ADSR
#define EDITUNISON 115   	// Mode edition UNISON
#define EDITLFO1   116   	// Mode edition LFO1
#define EDITLFO2   117      // Mode edition LFO2
#define EDITDEBUG  118		// Menu debug

// Menu presets (load,save,name)
//------------------------------
#define MENUPRESETSLOAD 20	// Menu presets chargement de preset
#define MENUPRESETSSAVE 21	// Menu presets sauvegarde de preset
#define MENUPRESETSEDIT 22	// Menu presets edition de nom
#define MINMENUPRESETS  20	// Valeur mini menu presets
#define MAXMENUPRESETS  22	// VAleur maxi menu presets

#define MENUPRESETS 5 	
#define EDITSETUP 2     	// Mode edition Name PRESET
byte modeMenu=0;
#define B_MENU   1     // Numero bouton menu SETUP
#define B_ENC1   2     // Numero bouton encodeur 1
#define B_ENC2   3     // Numero bouton encodeur 2
#define B_MAINT  9
byte refreshMenu=1;
char BuffAff[24];  		// buffer affichage pour PROGMEM entre autre

// Divers
int myInt;

//------------------------------------
// Routine interruption du TIMER 2
// permet de generer les waves
// et envoyer en temps reel sur le DAC
//------------------------------------
ISR(TIMER2_COMPA_vect) {
byte P0,P1,P2,P3;
    OCR2A = 127;
	ls_cnt1++;
	ls_lfo1++;
	ls_lfo2++;
	// Si valeur de lfo1 !=0
	if (lfo1) {
		// Comparer le timer lfo1 avec la valeur reglee
		// Si superieure ou egale incrementer pointeur onde lfo
		// La valeur finale du lfo sera recherchee grace au pointeur
		// avec la forme onde choisie ailleurs.
		if (ls_lfo1 >= lfo1) {
			ls_lfo1=0;
			inclfo1++;
		}
	}
	// Meme chose pour lfo2
	if (lfo2) {
		if (ls_lfo2 >= lfo2) {
			ls_lfo2=0;
			inclfo2++;
		}
	}
	
	// Activation du flag des taches plus lente
	if (ls_cnt1>=FLOW1) {
		f_lowspeed = true;
		ls_cnt1=0;
	}
	// Si aucune note a jouer
	// on sort de l'ISR inutile de perdre du temps
	// if (ADSR==NONOTE)
		// return;
	
	//-----------------------------------------------
	// Calcul des increments de base sans mode unison
	//-----------------------------------------------
	//----------------------
	pos[0] += incr[0]+bendOsc1+pitchOsc1;					// Increment wavetable 1
    pos[1] += incr[1]+bendOsc2+pitchOsc2;					// Increment wavetable 2
	P0=highByte(pos[0])+phase1;		// On garde l'octet de poids fort pour l'index de position dans la table onde * ampli oscillateur
	P1=highByte(pos[1])+phase2;	

	//-------------------------------------------------------------------
	// Calcul de lechantillon 8 bits en fonction du mode unison ou normal
	//-------------------------------------------------------------------
	if (unison==1) {
		// on calcule les 2 oscillateurs supplementaire en unison
		pos[2] += incr[0]-decunison1+bendOsc1+pitchOsc1;		// Increment wavetable 1 (pour unison)
		pos[3] += incr[1]+decunison2+bendOsc2+pitchOsc2;	    // Increment wavetable 2 (pour unison)
		P2=highByte(pos[2])+phase1;	
		P3=highByte(pos[3])+phase2;	
		// 4 ondes ajoutees pour fabriquer l'echantillon
		// P0 table 1, P1+256 table 2
		// P2 table 1 avec decrement unison et P3 table 2 avec uincrement unison
		// les increments unison sont calcules en random dans la boucle principale
		switch(VCA_OSC1) {
			case 100:
				samplet = wavetables[P0] + wavetables[P2];
				break;
			case 50:
				samplet = (wavetables[P0] + wavetables[P2]) >> 1;
				break;
			case 25:
				samplet = (wavetables[P0] + wavetables[P2]) >> 2;
				break;
		}
		
		switch(VCA_OSC2) {
			case 100:
				samplet1 = wavetables[P1+256] + wavetables[P3+256];
				break;
			case 50:
				samplet1 = (wavetables[P1+256] + wavetables[P3+256]) >> 1;
				break;
			case 25:
				samplet1 = (wavetables[P1+256] + wavetables[P3+256]) >> 2;
				break;
				
		}
		samplet+= samplet1;
	}
	else {
		switch(VCA_OSC1) {
			case 100:
			samplet = wavetables[P0];
				break;
			case 50:
			samplet = wavetables[P0] >> 1;
				break;
			case 25:
			samplet = wavetables[P0] >> 2;
				break;
		}
		
		switch(VCA_OSC2) {
			case 100:
			samplet1 = wavetables[P1+256];
				break;
			case 50:
			samplet1 = wavetables[P1+256] >> 1;
				break;
			case 25:
			samplet1 = wavetables[P1+256] >> 2;
				break;
				
		}

		// Echantillon construit avec les 2 tables ondes
		// PO index dans debut de la table
		// P1 index dans la deuxieme partie de la table
		samplet += samplet1;
	}
	
	// Fabrication de l'echantillon final
	//sample=(samplet <<2)*VCA_Mult;					// Augmenter un peu le gain (x4) et regler amplitude du VCA global
	sample=(samplet <<2);					// Augmenter un peu le gain (x4) et regler amplitude du VCA global

	//--------------------------------------
	// Recopie de lechantillon dans le DAC 1
	//--------------------------------------
    PORTB &= ~(1<<1);						// Chip select
    SPDR = highByte(sample) | 0x70;			// Copie MSB + 4 bits programmation du DAC (buffered, 1x gain et active mode)
    while (!(SPSR & (1<<SPIF)));			// Attente de la copie
    SPDR = lowByte(sample);					// Copie du LSB
    while (!(SPSR & (1<<SPIF)));			// Attente de la copie
    PORTB |= (1<<1);						// Liberation du chip
	
	//-------------------------------
	// Programmation du VCA
	// Sortie du DAC2 en Vref du DAC1
	//-------------------------------
	VCA=VCA_12bits-vcaOsc;
	if (VCA < 0)
		VCA=0;
	
	PORTB &= ~(1<<0);						// Chip select
	SPDR = highByte(VCA) | 0x70;		// Copie MSB + 4 bits programmation du DAC (buffered, 1x gain et active mode)
	while (!(SPSR & (1<<SPIF)));			// Attente de la copie
	SPDR = lowByte(VCA);				// Copie du LSB
	while (!(SPSR & (1<<SPIF)));			// Attente de la copie
	PORTB |= (1<<0);						// Liberation du chip

		
	// PORTB &= ~(1<<0);						// Chip select
	// SPDR = highByte(VCA_12bits) | 0x70;		// Copie MSB + 4 bits programmation du DAC (buffered, 1x gain et active mode)
	// while (!(SPSR & (1<<SPIF)));			// Attente de la copie
	// SPDR = lowByte(VCA_12bits);				// Copie du LSB
	// while (!(SPSR & (1<<SPIF)));			// Attente de la copie
	// PORTB |= (1<<0);						// Liberation du chip
	
}



//----------------------------------------------------- 
// HANDLER LECTURE ENCODEURS
// Procedure geree par timer 1
//----------------------------------------------------- 
ISR(TIMER1_OVF_vect) {  
int MSB;
int LSB;
int encoded;
int sum;
byte i;
	TCNT1 = tcnt1;      // Reload du timer
		
	for (i=0;i<2;i++) {  
		//MSB = digitalRead(pinA[i]); // MSB = most significant bit
		//LSB = digitalRead(pinB[i]); // LSB = least significant bit
		MSB = bitRead(PIND,pinA[i]); // MSB = most significant bit
		LSB = bitRead(PIND,pinB[i]); // LSB = least significant bit
		encoded = (MSB << 1) |LSB; // conversion
		sum  = (lastEncoded[i] << 2) | encoded; // Ajout a la valeur precedent
		// Test si on avance
		if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) 
		  counter[i]++;
		// Test si on recule  
		if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)    
		  counter[i]--;                                                       
		lastEncoded[i] = encoded;   // On memorise la valeur pour la prochaine fois
		bcounter[i]=counter[i]>>2;  // Compteur utilisateur divise par 4
	}		

}


//---------------------------------
// SETUP DES VARIABLES
// Preparation des PINS
// Initialisation des interruptions
//---------------------------------
void setup() {

	// Mode master SPI
	pinMode(10, OUTPUT);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(9,OUTPUT);		// Chip select DAC 1
	pinMode(8,OUTPUT); 		// Chip select DAC 2
	pinMode(2,OUTPUT);		// Gate out
	pinMode(3,OUTPUT);		// LED
	
	
	
	//---------------------------------
	// Preparation interruption timer 2
	//---------------------------------
    cli();										// Arret des interruptions
    TIMSK2 = (1 << OCIE2A);
    OCR2A = 127;								// Comparaison  en OVF a 127 

    TCCR2A = 1<<WGM21 | 0<<WGM20; 				// CTC mode, reset on match
    TCCR2B = 0<<CS22 | 1<<CS21 | 0<<CS20; 		// 8 prescaler 
												// La frequence est 16Mhz/8/128 = 15625hz
    SPCR = 0x50;								// Preparation SPI
    SPSR = 0x01;
    DDRB |= 0x2E;
    PORTB |= (1<<1);							// Chip select
    sei();										// Autorisation interruption

	// Initialisation des PIN pour les encodeurs
	//------------------------------------------
	for(int i=0;i<2;i++) {
		pinMode(pinA[i], INPUT);		// Entrees pour pins A
		pinMode(pinB[i], INPUT);		// Entrees pour pins B
		digitalWrite(pinA[i],HIGH);		// Pull-up ON pour pins A
		digitalWrite(pinB[i],HIGH);		// Pull-up ON pour pins B
	}
	
	// Initialisation timer 1 pour encodeurs
	//--------------------------------------
	TIMSK1 &= ~(1<<TOIE1);  
	// Timer 2 en mode normal
	TCCR1A &= ~((1<<WGM11) | (1<<WGM10));  
	TCCR1B &= ~(1<<WGM12);  
	// Mode overflow
	TIMSK1 &= ~(1<<OCIE1A);  
	// Prescaler a 64
	TCCR1B |= (1<<CS10)  | (1<<CS11) ; // Set bits  
	TCCR1B &= ~(1<<CS12);              // Clear bit  
	// Sauvegarde de la valeur pou reloader dans l'ISR
	tcnt1 = 65037;	// Pour 2ms
	
	// Chargement et mise en route du timer
	TCNT1 = tcnt1;  
	TIMSK1 |= (1<<TOIE1);

	// Initialisation variables diverses
	ModeMidi=RETRIG;							// Mode mono avec retrif enveloppe a chaque nouvelle note
	CountNote=0;								// Nombre de note appuyee
	// OffsetTable[0]=0;							// Offset dans table onde 1
	// OffsetTable[1]=0;							// Offset dans table onde 2
	unison=0;									// Mode unison off
	decunison1=0;
	decunison2=0;
	MIDI.begin(MIDI_CHANNEL_OMNI);
	MIDI.setHandleNoteOn(MyHandleNoteOn);       // handler note on 
	MIDI.setHandleNoteOff(MyHandleNoteOff);     // handler note off
	MIDI.setHandleControlChange(MyHandleCtrl);  // handler CC
	MIDI.setHandlePitchBend (MyHandlePitchBend);// handler pitch-bend
	/*
	// Redefinition du prescaler pour les ADC
	// set up the ADC
	ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  
	// you can choose a prescaler from above.
	// PS_16, PS_32, PS_64 or PS_128
	ADCSRA |= PS_64;    // set our own prescaler to 64 
	*/
	maj_wavetables(notable[0],0);						// Initialisation table 1
	maj_wavetables(notable[1],1);						// Initialisation table 2
	ADSR=NONOTE;								// Reset de ADSR
	
	lcd.init();                     			// init du lcd 
	lcd.backlight();                			// Retroeclairage
	lcd.createChar(0, symbup);      			// Creation fleche UP 
	lcd.createChar(1, symbdown);    			// Creation fleche DOWN
	lcd.home();                     			// curseur vers home
	lcd.print(F("*** WAVE FA  ***"));   		// Affichage titre
	lcd.setCursor(0,1);
	lcd.print(F("   Version 1.0  "));
	delay(1000);
	lcd.clear();
	lcd.print(F("Free: "));
	lcd.print(freeMemory(),DEC);
	delay(1000);
	load_preset(0);
	//  Mettre l'I2C à 400KHz
	// pas sur que ca soit compatible avec tous les LCD et materiel I2C
	// notamment l'eeprom
	TWBR=12;
	
}



//--------------------
// PROGRAMME PRINCIPAL
//--------------------
void loop() {
	// Lecture du port midi
	MIDI.read();
	// Gestion du mode unison si besoin
	if (unison) {
		decunison1=random(1,(incr[0]>>7)*depth_unison);	// Calcul du décalage 1
		decunison2=random(1,(incr[1]>>7)*depth_unison);	// Calcul du décalage 2
	}
	distrib_menu();
	
	
	//gestion_menu_reglages();
	// Execution des taches plus lentes
	// Le flag lowspeed est sette en fonction de FLOW
	if (f_lowspeed) {
		gestion_adsr();
		gestion_lfos();
	    f_lowspeed=false;
	}
	
}

