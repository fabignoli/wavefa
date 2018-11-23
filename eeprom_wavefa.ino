//------------------------------------------
// procedures de gestion de l'EEPROM 24LC256
// programme principal wavefa.ino
//------------------------------------------

//-------------------------------------------------------------
// Routine ecriture d'un octet en eeprom
// Param 1 adresse I2C (defaut 0x50 avec pins 1,2,3 a la masse)
// param 2 adresse memoire a ecrire (de 0 à 32767 pour 24LC256)
// param 3 octet a ecrire
//-------------------------------------------------------------

void eewrite_byte( int deviceaddress, unsigned int eeaddress, byte data ) 
{
int rdata = data;
  Wire.beginTransmission(deviceaddress);// Selection du device
  Wire.write((int)(eeaddress >> 8));    // MSB
  Wire.write((int)(eeaddress & 0xFF));  // LSB
  Wire.write(rdata);                    // Ecriture
  Wire.endTransmission();               // Fin de transmission
  delay(5);                             // Attente pour flush
}

//-------------------------------------------------------------
// Routine lecture d'un octet en eeprom
// Param 1 adresse I2C (defaut 0x50 avec pins 1,2,3 a la masse)
// param 2 adresse memoire à lire (de 0 à 32767 pour 24LC256)
//-------------------------------------------------------------
byte eeread_byte( int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);      // Selection du device
  Wire.write((int)(eeaddress >> 8));          // MSB
  Wire.write((int)(eeaddress & 0xFF));        // LSB
  Wire.endTransmission();                     // Fin de transmission
  Wire.requestFrom(deviceaddress,1);          // Requete 1 octet
  if (Wire.available()) rdata = Wire.read();  // Lecture tant que possible
    return rdata;                             // Retourner la donnee
}

//---------------------------------------
// Sauvegarde dun preset
// param 1 :  numero de la banque (O a 3)
//---------------------------------------
void save_preset(byte mypreset)
{
int i;
int myaddress=0;

    // Initialisation de l'adresse du preset
    myaddress = mypreset*PRESETSIZE;
    // Nom du preset
    for (i=0;i<16;i++) { 
      eewrite_byte( 0x50, myaddress++, nomPreset[i]);					// Nom du preset
    }
	
	// numero de tables
    eewrite_byte( 0x50, myaddress++, notable[0]);						// Numero table onde 1
    eewrite_byte( 0x50, myaddress++, notable[1]);						// Numero table onde 2
	
    // Enveloppe
    eewrite_byte( 0x50, myaddress++, highByte(val_ADSR[pATTAQUE]));		// MSB attaque
    eewrite_byte( 0x50, myaddress++, lowByte(val_ADSR[pATTAQUE]));		// LSB attaque
    eewrite_byte( 0x50, myaddress++, highByte(val_ADSR[pDECAY]));		// MSB decay
    eewrite_byte( 0x50, myaddress++, lowByte(val_ADSR[pDECAY]));		// LSB decay
    eewrite_byte( 0x50, myaddress++, highByte(val_ADSR[pSUSTAIN]));		// MSB sustain
    eewrite_byte( 0x50, myaddress++, lowByte(val_ADSR[pSUSTAIN]));		// LSB sustain
    eewrite_byte( 0x50, myaddress++, highByte(val_ADSR[pRELEASE]));		// MSB release
    eewrite_byte( 0x50, myaddress++, lowByte(val_ADSR[pRELEASE]));		// LSB release
	
	// Unison
    eewrite_byte( 0x50, myaddress++, unison);			// Mode unison (on ou off)
    eewrite_byte( 0x50, myaddress++, depth_unison);		// Profondeur unison de 1 a 5
	
	// PWM
    eewrite_byte( 0x50, myaddress++, pwm[0]);			// Pwm oscillateur 1
    eewrite_byte( 0x50, myaddress++, pwm[1]);			// Pwm oscillateur 2
	
	// Amps
    eewrite_byte( 0x50, myaddress++, VCA_OSC1);			// Ampli osc1 (25, 50 ou 100%)
    eewrite_byte( 0x50, myaddress++, VCA_OSC2);			// Ampli osc2 (25, 50 ou 100%)
	
	// Semi-tone
    eewrite_byte( 0x50, myaddress++, semi[0]);			// semitone osc1 en note midi (de 0 a 24)
    eewrite_byte( 0x50, myaddress++, semi[1]);			// semitone osc2 en note midi (de 0 a 24)
	
	// Lfos
    eewrite_byte( 0x50, myaddress++, lfo1);				// Vitesse lfo1
    eewrite_byte( 0x50, myaddress++, lfo2);				// Vitesse lfo2
    eewrite_byte( 0x50, myaddress++, depth_lfo1);		// profondeur lfo1
    eewrite_byte( 0x50, myaddress++, depth_lfo2);		// profondeur lfo2
    eewrite_byte( 0x50, myaddress++, dest_lfo1);		// destination lfo1
    eewrite_byte( 0x50, myaddress++, dest_lfo2);		// destination lfo2
	
}




//-----------------------------------------------------------
// charge un nom du preset dans la variable globale nomPreset
//-----------------------------------------------------------
void load_preset_name(byte mypreset) {
int i;
int myaddress=0;

    memset(nomPreset,0,sizeof(nomPreset));
    // Initialisation de l'adresse du preset
    myaddress=(mypreset*PRESETSIZE);
    // Nom du preset
    for (i=0;i<16;i++) {
      nomPreset[i]=eeread_byte( 0x50, myaddress++);
	  MIDI.read();
    }
}

//-----------------------------------------------
// Chargement dun preset
// param 1 :  numero de la banque (O a 3)
// param 2 :  numero du preset (0 à 127)
// Charge les donnees dans les variables globales
//-----------------------------------------------
void load_preset(byte mypreset)
{
int i;
int myaddress=0;

    // Initialisation de l'adresse du preset
    myaddress=mypreset*PRESETSIZE;
    // Nom du preset
    memset(nomPreset,0,sizeof(nomPreset));
    for (i=0;i<16;i++) {
      nomPreset[i]=eeread_byte( 0x50, myaddress++);
    }
	
	// numero de tables
    notable[0]=eeread_byte( 0x50, myaddress++);						// Numero table onde 1
    notable[1]=eeread_byte( 0x50, myaddress++);						// Numero table onde 2
	
    // Enveloppe
    myByte1=eeread_byte( 0x50, myaddress++);
    myByte2=eeread_byte( 0x50, myaddress++);
    val_ADSR[pATTAQUE]=word(myByte1,myByte2);
    myByte1=eeread_byte( 0x50, myaddress++);
    myByte2=eeread_byte( 0x50, myaddress++);
    val_ADSR[pDECAY]=word(myByte1,myByte2);
    myByte1=eeread_byte( 0x50, myaddress++);
    myByte2=eeread_byte( 0x50, myaddress++);
    val_ADSR[pSUSTAIN]=word(myByte1,myByte2);
    myByte1=eeread_byte( 0x50, myaddress++);
    myByte2=eeread_byte( 0x50, myaddress++);
    val_ADSR[pRELEASE]=word(myByte1,myByte2);
	
	// Unison
    unison=eeread_byte( 0x50, myaddress++);				// Mode unison (on ou off)
    depth_unison=eeread_byte( 0x50, myaddress++);		// Profondeur unison de 1 a 5
	
	// PWM
    pwm[0]=eeread_byte( 0x50, myaddress++);				// Pwm oscillateur 1
    pwm[1]=eeread_byte( 0x50, myaddress++);				// Pwm oscillateur 2

	// Amps
    VCA_OSC1=eeread_byte( 0x50, myaddress++);			// Ampli osc1 (25, 50 ou 100%)
    VCA_OSC2=eeread_byte( 0x50, myaddress++);			// Ampli osc1 (25, 50 ou 100%)
	
	// Semi-tone
    semi[0]=eeread_byte( 0x50, myaddress++);			// semitone osc1 en note midi (de 0 a 24)
    semi[1]=eeread_byte( 0x50, myaddress++);			// semitone osc2 en note midi (de 0 a 24)
	
	// Lfos
    lfo1=eeread_byte( 0x50, myaddress++);				// Vitesse lfo2
    lfo2=eeread_byte( 0x50, myaddress++);				// Vitesse lfo2
    depth_lfo1=eeread_byte( 0x50, myaddress++);			// profondeur lfo1
    depth_lfo2=eeread_byte( 0x50, myaddress++);			// profondeur lfo2
    dest_lfo1=eeread_byte( 0x50, myaddress++);			// destination lfo1
    dest_lfo2=eeread_byte( 0x50, myaddress++);			// destination lfo2
	maj_wavetables(notable[0],0);						// Initialisation table 1
	maj_wavetables(notable[1],1);		
}
