//------------------------------
// procedures de gestion du midi
// programme principal wavefa.ino
//-------------------------------



//--------------------------- 
// HANDLER NOTE ON MIDI
//--------------------------- 
void MyHandleNoteOn(byte channel, byte pitch, byte velocity) {

	// Si velocity == 0 alors on a recu un note off
	//---------------------------------------------
	if (velocity==0) {
		MyHandleNoteOff(channel,pitch,velocity);
		// gestion du note off et sortie du handler
		return;
	}
	CountNote++;    // Incrementation du nombre de note actives
	// Si mode mono et premiere note jouee
	// on calcul l'increment pour l'accumulateur
	// On redeclenche l'attaque
	// (donc si mono et plus que 1 note, seule la premiere sera prise en compte)
	if (ModeMidi==MONO) {		// Si mode mono pur (la premiere note reste)
		if (CountNote==1) {		// Si une seule note appuyee, on calcul
			ADSR=GATED;			// declenchement ADSR
			lastNote=pitch;		// memorisation derniere note
			//PORTD |= (1<<3);	// Allumage de la led
			LEDON;
			GATEON;
			incr[0] = pgm_read_word_near( MidiIncr + pitch + semi[0] - 12);
			incr[1] = pgm_read_word_near( MidiIncr + pitch + semi[1] - 12);
		}
		return;					
	}
	// Si mode RETRIG et qu'une note etait deja appuyee avant celle en cours
	// il faut redeclencher le gate 
	// on recalcul l'increment avec la nouvelle note
	if (ModeMidi==RETRIG and CountNote>=1) {
		lastNote=pitch;			// Memorisation derniere note
		ADSR=GATED;				// declenchement ADSR
		//PORTD |= (1<<3);		// Allumage de la led
		LEDON;
		GATEON;
		incr[0] = pgm_read_word_near( MidiIncr + pitch + semi[0] - 12);
		incr[1] = pgm_read_word_near( MidiIncr + pitch + semi[1] - 12);
	}
}

//---------------------------- 
// HANDLER NOTE OFF MIDI -> CV
//---------------------------- 
void  MyHandleNoteOff(byte channel, byte pitch, byte velocity) {
    CountNote--;              // Decrement du nombre de note
    if (CountNote ==255)         // Test si negatif (impossible normalement)
        CountNote=0;          // Si c'est le cas RAZ
	if (!CountNote) {
		//ADSR=NONOTE;
		ADSR=NOGATED;			// Fin de cycle ADSR
		//PORTD &= ~(1<<3);		// extinction de la led
		LEDOFF;
		GATEOFF;
	}
}

//----------------
// HANDLER CC MIDI
//----------------
void MyHandleCtrl(byte channel, byte number, byte value) {
	return;
}

//-------------------
// HANDLER PITCH-BEND
//-------------------
void MyHandlePitchBend (byte channel, int value) {
	bend = value>>4-512;
	if (bend>384) bend = 384;
	if (bend<-384) bend = -384;
	// bendOsc1=pgm_read_word_near( MidiIncr + lastNote + semi[0] - 12)/384*bend;
	// bendOsc2=pgm_read_word_near( MidiIncr + lastNote + semi[1] - 12)/384*bend;
	if (!bend) {
		bendOsc1=0;
		bendOsc2=0;
		return;
	}
	if (bend>0) {
		bendOsc1=pgm_read_word_near( MidiIncr + lastNote + semi[0] - 12)/384.0*bend;
		bendOsc2=pgm_read_word_near( MidiIncr + lastNote + semi[1] - 12)/384.0*bend;
		return;
	}
	if (bend<0) {
	    bend*=-1;
		bendOsc1=pgm_read_word_near( MidiIncr + lastNote + semi[0] - 12)/768.0*bend;
		bendOsc2=pgm_read_word_near( MidiIncr + lastNote + semi[1] - 12)/768.0*bend;
		bendOsc1*=-1;
		bendOsc2*=-1;
	}
	
}

