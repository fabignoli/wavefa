//-------------------------------------
// procedures de gestion des enveloppes
// programme principal wavefa.ino
//-------------------------------------


//-------------------------------------------------
// Procedure de calcul pour variable ADSR
// Evite d'alourdir la procedure gestion adsr
// a chaque occurence dans loop
// le coefficient calcule est applique sur le DAC2
// qui pilote le Vref du DAC1
//-------------------------------------------------
// cette procedure est appellee a chaque changement
// manuel d'une des 4 valeurs de l'ADSR et a chaque
// changement de note
//-------------------------------------------------
void calc_adsr(void) {
	// Increment attaque
	//------------------
	if (val_ADSR[pATTAQUE]) {						// Si valeur attaque >0
		incr_attaque2=1.0/val_ADSR[pATTAQUE]*4095;	// Calcul increment attaque
	}
	
	// calcul valeur sustain sur 12 bits
	//----------------------------------
	vol_sustain2=val_ADSR[pSUSTAIN]/1000.0*4095;
	
	// Calcul decrement du decay
	//--------------------------
	if (val_ADSR[pDECAY]) {									// Si valeur de DECAY > 0
		decr_decay2=(4095-vol_sustain2)/val_ADSR[pDECAY];	// Calcul du decrement de decay
	}
	
	// Calcul decrement du release
	//----------------------------
	if (val_ADSR[pRELEASE]) {								// Si valeur de release
		decr_release2=last_VCA_12bits/val_ADSR[pRELEASE];	// Calcul du decrement de release
	}	
	else {
		decr_release2=last_VCA_12bits;						// Decrement egal a la derniere valeur du coefficient VCA
	}
}


//---------------------------------------------------------------
// Routine gestion enveloppe
// fait les calculs en fonction de la variable globale ADSR
// qui peut prendre les valeurs suivantes
// NONOTE : aucune note jouee, synthe a l'arret
// GATED  : detection appui de note
// ATTAQUE : phase attaque de l'enveloppe
// DECAY   : phase decay
// SUSTAIN : phase systain
// NOGATED : preparation au release toutes les notes sont relachees
// RELEASE : phase de release. Quand terminee, on passe en NONOTE
//---------------------------------------------------------------
void gestion_adsr(void)
{
int tempvca;
	// Si aucune note jouee
	// on sort
	if (ADSR==NONOTE) {
		VCA_12bits=0;
		return;
	}
	// Si enveloppe juste triggee
	// on passe a la phase attaque
	if (ADSR==GATED) {
		VCA_12bits=0;
		last_VCA_12bits=0;
		calc_adsr();
		ADSR=ATTAQUE;	// Phase attaque declenchee
	}
	// Si phase attaque
	if (ADSR==ATTAQUE) {
		// Calcul du coefficient multiplicateur
		if (!val_ADSR[pATTAQUE]) {
			VCA_12bits=4095;
		}
		else {
			VCA_12bits+=incr_attaque2;
		}
		if (VCA_12bits>=4095){
			VCA_12bits=4095;
			ADSR=DECAY;
		}
	}
	// Si phase decay
	if (ADSR==DECAY) {
		if (!val_ADSR[pDECAY]) {
			VCA_12bits=vol_sustain2;
			ADSR=SUSTAIN;
		}
		else {
			// SI le coefficient est toujours superieur au sustain
			// alors on decrement
			if (VCA_12bits > vol_sustain2) {
				VCA_12bits-=decr_decay2;
			}
			// Sinon on est arrive au sustain
			else {
				ADSR=SUSTAIN;
			}
		}
	}
	// Si phase sustain
	if (ADSR==SUSTAIN) {
		return;
	}
	// Si phase relachement de touche
	// preparation du release
	if (ADSR==NOGATED) {
		// Reglage du coeff mult initial
		// Normalement cest deja ok
		// Si sustain 0, coeff=0 sinon on calcul le sustain initial
		last_VCA_12bits=VCA_12bits;
		calc_adsr();
		ADSR=RELEASE;
	}
	
	// Si phase release
	if (ADSR==RELEASE) {
		// Cas particulier
		// Si release=0 on arrete tout
		if (val_ADSR[pRELEASE]==0) {
			ADSR=NONOTE;
			VCA_12bits=0;
			return;
		}
		// Arrive ici release a une vrai valeur
		// Le VCA est deja initialise au niveau de sustain
		// Il faut diminuer le son jusqu'a 0
/*
		if ((VCA_12bits - decr_release2)>0) {
			VCA_12bits -= decr_release2;
			if (VCA_12bits >4095) {
				VCA_12bits=0;
				ADSR=NONOTE;
			}
		}
		else {
			VCA_12bits = 0;
			ADSR=NONOTE;
		}
*/		
		tempvca=VCA_12bits - decr_release2;
		if (tempvca >0) {
			VCA_12bits=tempvca;
		}
		else {
			VCA_12bits=0;
			ADSR=NONOTE;
		
		}
	return;
	}
}
