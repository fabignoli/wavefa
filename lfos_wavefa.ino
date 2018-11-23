//-------------------------------
// procedures de gestion des LFOS
// programme principal wavefa.ino
//-------------------------------

//----------------------------------
// Modulation de la largeur du carre
//----------------------------------
void lfos_pwm(void) {
unsigned int myvalue;
	// si lfo1 actif et que table 0 sur square
	if (lfo1 && notable[0]==0) {
		// Verifier si pwm pour lfo1
		if (dest_lfo1==MODPWM) {
			myvalue=val_lfo1*depth_lfo1;
			myvalue= myvalue >> 8;
			if (pwm[0]+myvalue <250) {
				myvalue+=pwm[0];
				maj_pwm(0,myvalue);
			}
		}
	}
	// si lfo2 actif et que table 1 sur square
	if (lfo2 && notable[1]==0) {
		// Verifier si pwm pour lfo2
		if (dest_lfo2==MODPWM) {
			myvalue=val_lfo2*depth_lfo2;
			myvalue= myvalue >> 8;
			if (pwm[1]+myvalue <250) {
				myvalue+=pwm[1];
				maj_pwm(1,myvalue);
			}
		}
	}
}

//--------------------------------------------
// Modulation de la frequence des oscillateurs
//--------------------------------------------
void lfos_pitch(void) {
	if (lfo1) {
		if (dest_lfo1==MODPITCH) {
			pitchOsc1=val_lfo1*depth_lfo1;
			pitchOsc1=pitchOsc1 >> 8;
			
		}
	}
	if (lfo2) {
		if (dest_lfo2==MODPITCH) {
			pitchOsc2=val_lfo2*depth_lfo2;
			pitchOsc2=pitchOsc2 >> 8;
		}
	}
}

//----------------------------------------
// Modulation de la phase des oscillateurs
//----------------------------------------
void lfos_phase(void) {
int myvalue;
	if (lfo1) {
		if (dest_lfo1==MODPHASE) {
			myvalue=val_lfo1*depth_lfo1;
			phase1=myvalue >> 8;
		}
	}
	if (lfo2) {
		if (dest_lfo2==MODPHASE ) {
			myvalue=val_lfo2*depth_lfo2;
			phase2=myvalue >> 8;
		}
	}
}

//----------------------------------------
// Modulation de l'amplitude du vca
//----------------------------------------
void lfos_vca(void) {
int myvalue;
	if (lfo1) {
		if (dest_lfo1==MODVCA) {
			myvalue=val_lfo1*depth_lfo1;
			//vcaOsc=myvalue >> 5;
			vcaOsc=myvalue >> 4;
		}
		
	}
}

//-----------------------------
// gestion des lfos
// lecture de la valeur
// distribution des modulations
//-----------------------------
void gestion_lfos(void) {
	// Calcul de la valeur actuelle des lfos
	// le pointeur est incrementé en fonction de la vitesse du lfo
	// dans l'ISR principale. on a un pointeur entre 0 et 255
	// on utilise une forme d'onde en dent de scie qui varie de 0 à 127 
	// puis de 127 à 0
	if (inclfo1<128)
		val_lfo1=inclfo1;
	else
		val_lfo1=255-inclfo1;
	// Meme chose pour lfo2	
	if (inclfo2<128)
		val_lfo2=inclfo2;
	else
		val_lfo2=255-inclfo2;
			
		switch(step_lfo) {
			case MODPWM:
				lfos_pwm();
				break;
			case MODPITCH:
				lfos_pitch();
				break;
			case MODPHASE:
				lfos_phase();
				break;
			case MODVCA:
				lfos_vca();
				break;
			default:
				break;
		}
		step_lfo++;
		if (step_lfo >MAXMODS)
			step_lfo=0;
}
