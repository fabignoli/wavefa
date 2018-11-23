//---------------------------------------
// procedures de gestion des tables ondes
// programme principal wavefa.ino
//---------------------------------------



//---------------------------------------------
// MAJ des wavetables
// Parametre 1 choix de la forme onde (0 max)
// numero de la table aharger (0 ou 1)
//---------------------------------------------
void maj_wavetables(byte typetable,byte numerotable) {
uint16_t decalage=numerotable*256;
int i;
int myvalue = 255;
int OffsetWave;
	switch(typetable) {
		// Square wave (et PWM)
		case 0 :
			 for (i=0; i<pwm[numerotable]; i++)
				 wavetables[i+decalage]=255;	
			 for (i=pwm[numerotable]; i<256; i++)
				 wavetables[i+decalage]=0;	
			break;
		// Sawtooh wave
		case 1 :
			for (i = 0; i < 256; ++i) {
				wavetables[i+decalage] = i; // sawtooth
			}
			break;
		// Triangle wave
		case 2 : 
			for (i = 0; i < 128; ++i) {
			wavetables[i + decalage] = i * 2;
			}
			
			for (i = 128; i < 256; ++i) {
			  wavetables[i + decalage ] = myvalue;
			  myvalue -= 2;
			}		
			break;
		
		case 3  : // Sine wave
		case 4  : // Digi wave 1 
		case 5  : // Digi wave 2 
		case 6  : // Digi wave 3 
		case 7  : // Digi wave 4 
		case 8  : // Digi wave 5 
		case 9  : // Digi wave 6 
		case 10 : // Tan wave 1
		case 11 : // Tan wave 2
		case 12 : // Tan wave 3
		case 13 : // Tan wave 4
		case 14 : // Stair Exp
		case 15 : // Hard
		case 16 : // Soft
		case 17 : // Fuzz Square
		case 18 : // Spike Fred
		case 19 : // Square steps
		
			OffsetWave=(typetable-3)*256;
			for (i=0; i<256; i++)
				wavetables[i+decalage]=pgm_read_byte_near(WAVES1+OffsetWave+i);
			break;
	} 
	// ON a 20 table en mémoire flash ( de 0 a 19)
	// Si la table demandee est > 19
	// alors on a une lecture de wave dans la partie EEPROM
	if (typetable>19) {
		OffsetWave=8192+((typetable-20)*256);
		for (i=0; i<256; i++)
			wavetables[i+decalage]=eeread_byte( 0x50, OffsetWave++);
	}

}

//-------------------------------------------------------------------
// Mise a jour de la table onde pour le PWM uniquement
// afin d'accelerer si en mode PWM et pas passer dans init_wavetables
//-------------------------------------------------------------------
void maj_pwm(byte numerotable,byte maxpwm) {
uint16_t decalage=numerotable*256;
	if (numerotable > 1)
		return;
	for (int i=0; i<maxpwm; i++)
		wavetables[i+decalage]=255;	
	for (int i=maxpwm; i<256; i++)
		wavetables[i+decalage]=0;	

}

//------------------------------------------------------------
// Reset du pitch apres changement destination lfo par exemple
//------------------------------------------------------------
void reset_pitch(void) {
	pitchOsc1=0;
	pitchOsc2=0;
}

//----------------------------------------------------------
// Reset du pwm apres changement destination lfo par exemple
//----------------------------------------------------------
void reset_pwm(void) {
	if (notable[0]==0)
		maj_pwm(0,pwm[0]);
	if (notable[1]==0)
		maj_pwm(1,pwm[1]);
}

//---------------------------------------------------------------------------
// Reset des parametres de phase apres changement destination lfo par exemple
//---------------------------------------------------------------------------
void reset_phase(void) {
	phase1=0;
	phase2=0;
}

//-------------------------------------------------------------------------
// Reset des parametres de vca apres changement destination lfo par exemple
//-------------------------------------------------------------------------
void reset_vca(void) {
	vcaOsc=0;
}
