//-------------------------------------
// procedures de gestion des menus
// programme principal wavefa.ino
//-------------------------------------


//-------------------------------------------
// Lit les boutons et renvoie la valeurs lues
// Ne permet qu'une saisie quand un bouton
// est appuyé
//-------------------------------------------
byte readButtons() {
int c = 0;
	c=analogRead(A0);      // Lit lentree analogique 
	
   if (c>950) {
       lastBoutonNav=0;  // Remettre flag etat a zero
       return 0;           // Pas de bouton appuyé sortir
   }
  // Arrive ici normalement un bouton a ete appuye
  // for (int i=1; i<3; i++) {
    // MIDI.read();           // Lire midi pendant deboucing
    // delay(1);              // Attente pour deboucong
  // }
//  c=analogRead(A0);      // Lit lentree analogique 
  if (c>950) {
      lastBoutonNav=0;  // Remettre flag etat a zero
      return 0;           // Toujours pas de bouton appuye
  }
  if (lastBoutonNav>0)   // Si le dernier bouton n'a pas ete relache
      return 9;   // renvoit code+10 (touche 1 = 11, 2=12, etc)
  // Arrivé ici on est sur que le bouton a ete appuye
  if (c>=0 && c<40) {
      lastBoutonNav=3;
      return 3;           // bouton encodeur 2
  } 
  if (c>600 && c<700) {
      lastBoutonNav=1;
      return 1;           // bouton menu
  } 
  if (c>800 && c<900) {
      lastBoutonNav=2;
      return 2;           // bouton encodeur 1
  }                       
  return 0;
}

//--------------------------------------------
// Routine principale de lecture des encodeurs
//--------------------------------------------
byte readEncodeurs(void) {
byte i,val;
  val=0;
  // Lire les 2 encodeurs
  for (i=0;i<2;i++) {
    // si la valeur a variee pour un des encodeurs
    if (lcounter[i]!=bcounter[i]) {
      // Tester le send
      // Si UP valeur de 11 a 14
      if (bcounter[i]>lcounter[i])
        val=i+11;
      // Si down valeur de 1 a 4
      else
        val=i+1;
      lcounter[i]=bcounter[i];
      return val;
    }
  }
  return 0;
}

//------------------------------------
// Gestion menu choix des tables ondes
//------------------------------------
void gestion_menu_wave(void) {
	raflcd1=false;
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print(F("W1>"));
		if (notable[0]<20) {
			affOndesLcdPROGMEM(notable[0]);
		}
		else {
			lcd.print(F("WavProm "));
			digitsLcd(notable[0]-19,3);
		}
		lcd.setCursor(0,1);
		lcd.print(F("W2>"));
		if (notable[1]<20) {
			affOndesLcdPROGMEM(notable[1]);
		}
		else {
			lcd.print(F("WavProm "));
			digitsLcd(notable[1]-19,3);
		}
	}
	
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (notable[0]>=MAXTABLES)
				return;
			notable[0]++;
			maj_wavetables(notable[0],0);
			raflcd1=true;
			break;
		case ENC1DOWN:
			if (!notable[0])
				return;
			notable[0]--;
			maj_wavetables(notable[0],0);
			raflcd1=true;
			break;
		case ENC2UP:
			if (notable[1]>=MAXTABLES)
				return;
			notable[1]++;
			maj_wavetables(notable[1],1);
			raflcd1=true;
			break;
		case ENC2DOWN:
			if (!notable[1])
				return;
			notable[1]--;
			maj_wavetables(notable[1],1);
			raflcd1=true;
			break;
	}
	if (raflcd1) {
		lcd.setCursor(3,0);
		// affOndesLcdPROGMEM(notable[0]);
		// lcd.setCursor(3,1);
		// affOndesLcdPROGMEM(notable[1]);
		//lcd.setCursor(3,0);
		if (notable[0]<20) {
			affOndesLcdPROGMEM(notable[0]);
		}
		else {
			lcd.print(F("WavProm "));
			digitsLcd(notable[0]-19,3);
			lcd.print(F("  "));
		}
		lcd.setCursor(3,1);
		if (notable[1]<20) {
			affOndesLcdPROGMEM(notable[1]);
		}
		else {
			lcd.print(F("WavProm "));
			digitsLcd(notable[1]-19,3);
			lcd.print(F("  "));
		}
	}




	return;
}

//------------------
// Gestion menu ADSR
//------------------
void gestion_menu_adsr(void) {
	raflcd1=false;		// flag rafraichissement de la fleche pointeur
	raflcd2=false;		// flag rafraichissement de donnee
	
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("ATT<DEC SUS REL "));
		digitsLcdPos(val_ADSR[pATTAQUE], 0,1,3);
		MIDI.read();
		lcd.print(F("<"));
		digitsLcdPos(val_ADSR[pDECAY], 4,1,3);
		MIDI.read();
		lcd.print(F(" "));
		digitsLcdPos(val_ADSR[pSUSTAIN], 8,1,3);
		MIDI.read();
		lcd.print(F(" "));
		digitsLcdPos(val_ADSR[pRELEASE], 12,1,3);
		MIDI.read();
		paramadsr=1;
	}
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (paramadsr==4)
				return;
			raflcd1=true;
			lcd.setCursor(paramadsr*4-1,0);
			lcd.print(F(" "));
			lcd.setCursor(paramadsr*4-1,1);
			lcd.print(F(" "));			
			paramadsr++;
			break;
		case ENC1DOWN:
			if (paramadsr==1)
				return;
			raflcd1=true;	
			lcd.setCursor(paramadsr*4-1,0);
			lcd.print(F(" "));
			lcd.setCursor(paramadsr*4-1,1);
			lcd.print(F(" "));			
			paramadsr--;;
			break;
		case ENC2UP:
			if (val_ADSR[paramadsr-1]>=max_ADSR[paramadsr-1]) 
				return;
			if (val_ADSR[paramadsr-1]<50)
				val_ADSR[paramadsr-1]++;
			else 
				if (val_ADSR[paramadsr-1]<500)
					val_ADSR[paramadsr-1]+=10;
				else
					val_ADSR[paramadsr-1]+=100;
					
			if (val_ADSR[paramadsr-1]>max_ADSR[paramadsr-1]) 
				val_ADSR[paramadsr-1]=max_ADSR[paramadsr-1];
			raflcd2=true;
			calc_adsr();
			break;
		case ENC2DOWN:
			if (!val_ADSR[paramadsr-1])
				return;
			if (val_ADSR[paramadsr-1]<=50)
				val_ADSR[paramadsr-1]--;
			else 
				if (val_ADSR[paramadsr-1]<=500)
					val_ADSR[paramadsr-1]-=10;
				else
					val_ADSR[paramadsr-1]-=100;
			
			raflcd2=true;
			calc_adsr();
			break;
					
	}
	
	if (raflcd1) {
		MIDI.read();
		lcd.setCursor(paramadsr*4-1,0);
		lcd.print(F("<"));
		MIDI.read();
		lcd.setCursor(paramadsr*4-1,1);
		lcd.print(F("<"));
	}
	if (raflcd2) {
		digitsLcdPos(val_ADSR[paramadsr-1], (paramadsr-1)*4, 1, 3);
	}
	
	
	return;
}

//-------------------------
// Gestion menu mode unison
//-------------------------
void gestion_menu_unison(void) {
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		MIDI.read();
		affChaineLcd("Unison : ");
		OnOffLcd(unison,9,0);
		lcd.setCursor(0,1);
		affChaineLcd("Depth..: ");
		lcd.print(depth_unison,DEC);
		return;
	}
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (unison)
				return;
			unison=1;
			OnOffLcd(unison,9,0);
			break;
		case ENC1DOWN:
			if (!unison)
				return;
			unison=0;
			OnOffLcd(unison,9,0);
			break;
		case ENC2UP:
			if (depth_unison==5)
				return;
			depth_unison++;
			lcd.setCursor(9,1);
			lcd.print(depth_unison,DEC);
			break;
		case ENC2DOWN:
			if (depth_unison==1)
				return;
			depth_unison--;
			lcd.setCursor(9,1);
			lcd.print(depth_unison,DEC);
			break;
	}
	
	
	return;
}

//--------------------------------------------
// Gestion menu amplification wave 1 et wave 2
//--------------------------------------------
void gestion_menu_amps(void) {
	raflcd1=false;
	raflcd2=false;
	if (refreshMenu) {
		raflcd1=true;
		raflcd2=true;
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("AMP 1> "));
		lcd.setCursor(0,1);
		lcd.print(F("AMP 2> "));
	}
	// Lecture des encodeurs
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (VCA_OSC1>=100)
				return;
			if (VCA_OSC1==50)
				VCA_OSC1=100;
			else
				VCA_OSC1=50;
			raflcd1=true;
			break;
		case ENC1DOWN:
			if (VCA_OSC1==25)
				return;
			if (VCA_OSC1==100)
				VCA_OSC1=50;
			else
				VCA_OSC1=25;
			raflcd1=true;
			break;
		case ENC2UP:
			if (VCA_OSC2>=100)
				return;
			if (VCA_OSC2==50)
				VCA_OSC2=100;
			else
				VCA_OSC2=50;
			raflcd2=true;
			break;
		case ENC2DOWN:
			if (VCA_OSC2==25)
				return;
			if (VCA_OSC2==100)
				VCA_OSC2=50;
			else
				VCA_OSC2=25;
			raflcd2=true;
			break;
	}
	
	if (raflcd1) {
		digitsLcdPos(VCA_OSC1, 7, 0, 3);
	}
	if (raflcd2) {
		digitsLcdPos(VCA_OSC2, 7, 1, 3);
	}
	
}

//-----------------------
// Gestion menu semitones
//-----------------------
void gestion_menu_semitone(void) {
	raflcd1=false;
	raflcd2=false;
	
	if (refreshMenu) {
		raflcd1=true;
		raflcd2=true;
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("SEMI WV1> "));
		lcd.setCursor(0,1);
		lcd.print(F("SEMI WV2> "));
	}
	// Lire les encodeurs
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (semi[0]>=24)
				return;
			semi[0]++;
			raflcd1=true;
			break;
		case ENC1DOWN:
			if (!semi[0])
				return;
			semi[0]--;
			raflcd1=true;
			break;
		case ENC2UP:
			if (semi[1]>=24)
				return;
			semi[1]++;
			raflcd2=true;
			break;
		case ENC2DOWN:
			if (!semi[1])
				return;
			semi[1]--;
			raflcd2=true;
			break;
	}
	if (raflcd1) {
		myInt=semi[0]-12;
		lcd.setCursor(10,0);
		if (myInt<0) {
			lcd.print(F("-"));
			myInt*=-1;
		}
		else
			lcd.print(F(" "));
		MIDI.read();
		digitsLcdPos((byte)myInt,11,0,2);
		incr[0] = pgm_read_word_near( MidiIncr + lastNote + semi[0] - 12);
		
	}
	if (raflcd2) {
		myInt=semi[1]-12;
		lcd.setCursor(10,1);
		lcd.setCursor(10,1);
		if (myInt<0) {
			lcd.print(F("-"));
			myInt*=-1;
		}
		else
			lcd.print(F(" "));
		MIDI.read();
		digitsLcdPos((byte)myInt,11,1,2);
		incr[1] = pgm_read_word_near( MidiIncr + lastNote + semi[1] - 12);
	}
	
}


//--------------------------------------------
// Gestion menu pwm wave 1 et wave 2
//--------------------------------------------
void gestion_menu_pwm(void) {
	raflcd1=false;
	raflcd2=false;
	if (refreshMenu) {
		refreshMenu=0;
		raflcd1=true;
		raflcd2=true;
		lcd.clear();
		lcd.print(F("PWM 1> "));
		lcd.setCursor(0,1);
		lcd.print(F("PWM 2> "));
	}
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (pwm[0]>=250)
				return;
			pwm[0]+=2;
			raflcd1=true;
			break;
		case ENC1DOWN:
			if (pwm[0]<10)
				return;
			pwm[0]-=2;
			raflcd1=true;
			break;
		case ENC2UP:
			if (pwm[1]>=250)
				return;
			pwm[1]+=2;
			raflcd2=true;
			break;
		case ENC2DOWN:
			if (pwm[1]<10)
				return;
			pwm[1]-=2;
			raflcd2=true;
			break;
	}
	
	if (raflcd1) {
		if (notable[0]==0)
			maj_wavetables(0,0);
		digitsLcdPos(pwm[0],11,0,3);
	}
	if (raflcd2) {
		if (notable[1]==0)
			maj_wavetables(0,1);
		digitsLcdPos(pwm[1],11,1,3);
	}
}

//-------------------
// Gestion menu lfo 1
//-------------------
void gestion_menu_lfo1(void) {
	raflcd1=false;		// flag rafraichissement de la fleche pointeur
	raflcd2=false;		// flag rafraichissement de donnee
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("LFO1<"));
		MIDI.read();
		lcd.print(F("AMPL "));
		MIDI.read();
		lcd.print(F("DEST "));
		MIDI.read();
		// Affichage LFO
		if (!lfo1) {
			lcd.setCursor(1,1);
			lcd.print(F("Off"));
		}
		else {
			digitsLcdPos(lfo1, 1,1,3);
		}
		MIDI.read();
		lcd.print(F("<"));
		digitsLcdPos(depth_lfo1, 6,1,3);
		MIDI.read();
		lcd.print(F(" "));
		switch (dest_lfo1) {
			case MODPWM:
				lcd.print(F("Pwm1"));
				break;
			case MODPITCH:
				lcd.print(F("Tun1"));
				break;
			case MODPHASE:
				lcd.print(F("Pha1"));
				break;
			case MODVCA:
				lcd.print(F("Vca "));
				break;
		}
		paramlfo1=1;
	}
	valbouton=readButtons();
	switch(valbouton) {
		// si bouton menu
		// on revient au debut
		case B_ENC1:
			if (paramlfo1==1) {
				lfo1=0;
				raflcd2=true;
			}
			//return;
			break;
	}
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (paramlfo1==3)
				return;
			raflcd1=true;
			lcd.setCursor(paramlfo1*5-1,0);
			lcd.print(F(" "));
			lcd.setCursor(paramlfo1*5-1,1);
			lcd.print(F(" "));			
			paramlfo1++;
			break;
		case ENC1DOWN:
			if (paramlfo1==1)
				return;
			raflcd1=true;	
			lcd.setCursor(paramlfo1*5-1,0);
			lcd.print(F(" "));
			lcd.setCursor(paramlfo1*5-1,1);
			lcd.print(F(" "));			
			paramlfo1--;;
			break;
			
		case ENC2UP:
			// Verifier si le parametre courant n'est pas a son maximum
			if (paramlfo1==1) {
				if (lfo1==255)
					return;
				else
					lfo1++;
			}
			if (paramlfo1==2) {
				if (depth_lfo1==255)
					return;
				else
					depth_lfo1++;
			}
			if (paramlfo1==3) {
				if (dest_lfo1==MAXMODS)
					return;
				else
					dest_lfo1++;
			}
			raflcd2=true;
			break;
		case ENC2DOWN:
			// Verifier si le parametre courant n'est pas a son maximum
			if (paramlfo1==1) {
				if (lfo1==0)
					return;
				else
					lfo1--;
			}
			if (paramlfo1==2) {
				if (depth_lfo1==1)
					return;
				else
					depth_lfo1--;
			}
			if (paramlfo1==3) {
				if (dest_lfo1==0)
					return;
				else
					dest_lfo1--;
			}
			raflcd2=true;
			break;
					
	}
	// rafraichir le curseur
	// pointant vers le parametre a editer
	if (raflcd1) {
		MIDI.read();
		lcd.setCursor(paramlfo1*5-1,0);
		lcd.print(F("<"));
		MIDI.read();
		lcd.setCursor(paramlfo1*5-1,1);
		lcd.print(F("<"));
	}
	// Rafraichir les donnes qui viennent de changer
	if (raflcd2) {
		switch(paramlfo1) {
			case 1 :
				if (!lfo1) {
					lcd.setCursor(1,1);
					lcd.print(F("Off"));
					reset_pitch();
					reset_pwm();
					reset_vca();
					reset_phase();
				}
				else {
					digitsLcdPos(lfo1, 1,1,3);
				}			
				break;
			case 2:
				digitsLcdPos(depth_lfo1, 6,1,3);
				break;
			case 3:
				lcd.setCursor(10,1);
				switch (dest_lfo1) {
					case MODPWM:
						lcd.print(F("Pwm1"));
						reset_pitch();
						reset_phase();
						reset_vca();
						break;
					case MODPITCH:
						lcd.print(F("Tun1"));
						reset_phase();
						reset_pwm();
						reset_vca();
						break;
					case MODPHASE:
						lcd.print(F("Pha1"));
						reset_pitch();
						reset_pwm();
						reset_vca();
						break;
					case MODVCA:
						lcd.print(F("Vca "));
						reset_phase();
						reset_pitch();
						reset_pwm();
						break;
						
				}				

				break;
		}
	}
}

//-------------------
// Gestion menu lfo 2
//-------------------
void gestion_menu_lfo2(void) {
	raflcd1=false;		// flag rafraichissement de la fleche pointeur
	raflcd2=false;		// flag rafraichissement de donnee
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("LFO2<"));
		MIDI.read();
		lcd.print(F("AMPL "));
		MIDI.read();
		lcd.print(F("DEST "));
		MIDI.read();
		// Affichage LFO
		if (!lfo2) {
			lcd.setCursor(1,1);
			lcd.print(F("Off"));
		}
		else {
			digitsLcdPos(lfo2, 1,1,3);
		}
		MIDI.read();
		lcd.print(F("<"));
		digitsLcdPos(depth_lfo2, 6,1,3);
		MIDI.read();
		lcd.print(F(" "));
		switch (dest_lfo2) {
			case MODPWM:
				lcd.print(F("Pwm2"));
				break;
			case MODPITCH:
				lcd.print(F("Tun2"));
				break;
			case MODPHASE:
				lcd.print(F("Pha2"));
				break;
		}
		paramlfo2=1;
	}
	valbouton=readButtons();
	switch(valbouton) {
		// si bouton menu
		// on revient au debut
		case B_ENC1:
			if (paramlfo2==1) {
				lfo2=0;
				raflcd2=true;
			}
			//return;
			break;
	}
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (paramlfo2==3)
				return;
			raflcd1=true;
			lcd.setCursor(paramlfo2*5-1,0);
			lcd.print(F(" "));
			lcd.setCursor(paramlfo2*5-1,1);
			lcd.print(F(" "));			
			paramlfo2++;
			break;
		case ENC1DOWN:
			if (paramlfo2==1)
				return;
			raflcd1=true;	
			lcd.setCursor(paramlfo2*5-1,0);
			lcd.print(F(" "));
			lcd.setCursor(paramlfo2*5-1,1);
			lcd.print(F(" "));			
			paramlfo2--;;
			break;
			
		case ENC2UP:
			// Verifier si le parametre courant n'est pas a son maximum
			if (paramlfo2==1) {
				if (lfo2==255)
					return;
				else
					lfo2++;
			}
			if (paramlfo2==2) {
				if (depth_lfo2==255)
					return;
				else
					depth_lfo2++;
			}
			if (paramlfo2==3) {
				if (dest_lfo2==MAXMODS-1)
					return;
				else
					dest_lfo2++;
			}
			raflcd2=true;
			break;
		case ENC2DOWN:
			// Verifier si le parametre courant n'est pas a son maximum
			if (paramlfo2==1) {
				if (lfo2==0)
					return;
				else
					lfo2--;
			}
			if (paramlfo2==2) {
				if (depth_lfo2==1)
					return;
				else
					depth_lfo2--;
			}
			if (paramlfo2==3) {
				if (dest_lfo2==0)
					return;
				else
					dest_lfo2--;
			}
			raflcd2=true;
			break;
					
	}
	// rafraichir le curseur
	// pointant vers le parametre a editer
	if (raflcd1) {
		MIDI.read();
		lcd.setCursor(paramlfo2*5-1,0);
		lcd.print(F("<"));
		MIDI.read();
		lcd.setCursor(paramlfo2*5-1,1);
		lcd.print(F("<"));
	}
	// Rafraichir les donnes qui viennent de changer
	if (raflcd2) {
		switch(paramlfo2) {
			case 1 :
				if (!lfo2) {
					lcd.setCursor(1,1);
					lcd.print(F("Off"));
					reset_pitch();
					reset_pwm();
					reset_phase();
				}
				else {
					digitsLcdPos(lfo2, 1,1,3);
				}			
				break;
			case 2:
				digitsLcdPos(depth_lfo2, 6,1,3);
				break;
			case 3:
				lcd.setCursor(10,1);
				switch (dest_lfo2) {
					case MODPWM:
						lcd.print(F("Pwm2"));
						reset_pitch();
						reset_phase();
						//reset_vca();
						break;
					case MODPITCH:
						lcd.print(F("Tun2"));
						reset_phase();
						reset_pwm();
						//reset_vca();
						break;
					case MODPHASE:
						lcd.print(F("Pha2"));
						reset_pitch();
						reset_pwm();
						//reset_vca();
						break;
				}				
				break;
		}
	}
}

void gestion_menu_debug(void) {
	if (refreshMenu) {
		refreshMenu=0;
		lcd.setCursor(0,0);
		lcd.print(vcaOsc,DEC);
		lcd.print("   ");
	}
}


//-------------------------------------
// reset pour revenir au menu principal
//-------------------------------------
void reset_menu(void) {
	modeMenu=MINMENURAC;
	refreshMenu=1;
}

//----------------------------------
// Gestion edition dun nom de preset
// avant la sauvegarde
// Jeu de caractere utilisee :
// [Space]()*+-./0123456789
// ABCDEFGHIJKLMNOPQRSTUVWXYZ
// abcdefghijklmnopqrstuvwxyz
//----------------------------------
void gestionEncPresetsName(void) {
boolean affchar;
char mychar;
  affchar=false;
  switch(valencodeurs) {
        
      // caractere vers la gauche
      case ENC1DOWN:
        if (posCarPreset>0) {     // x=2 sur le premier caractere
          posCarPreset--;         // decrementer la position en x
          affchar=true;
        }
        break;
      // caractere vers la droite
      case ENC1UP:
        if (posCarPreset<15) {    // x=18 sur le dernier caractere
          posCarPreset++;         // Incrementer la position en x
          affchar=true;
        }
        break;
      // Changement du caractere courant
      // prendre le precedent dans le jeu
      // fait le tour complet
      case ENC2DOWN:
        mychar=nomPreset[posCarPreset];   // Recuperer le caractere courant
        switch(mychar) {
          case ' ':
            mychar='z';
            break;
          case 'a':
            mychar='Z';
            break;
          case 'A':
            mychar='9'; 
            break;
          case '-':
            mychar='+'; 
            break;
          case '(':
            mychar=' '; 
            break;
          default:
            mychar--;
            break;
        }                   
        nomPreset[posCarPreset]=mychar;     // Rafraichir nouveau caractere
        affchar=true;                         // Forcer l'affichage
        break;
      // Changement du caractere courant
      // prendre le suivant dans le jeu
      // fait le tour complet
      case ENC2UP:
        mychar=nomPreset[posCarPreset];   // Recuperer le caractere courant
        switch(mychar) {
          case ' ':
            mychar='(';
            break;
          case '+':
            mychar='-'; 
            break;
          case '9':
            mychar='A'; 
            break;
          case 'Z':
            mychar='a'; 
            break;
          case 'z':
            mychar=' ';
            break;
          default:
            mychar++;
            break;
        }                   
        nomPreset[posCarPreset]=mychar;     // Rafraichir nouveau caractere
        affchar=true;                         // Forcer l'affichage
        break;
  }
  if (affchar) {
      lcd.noCursor(); 
      lcd.setCursor(posCarPreset,1);        // Positionner le curseur
      lcd.print(nomPreset[posCarPreset]); // posCarPreset - 2 pour indicer
      lcd.setCursor(posCarPreset,1);        // repositionner le curseur
      lcd.cursor();
      MIDI.read();
  }
}


//-----------------------------------
// Gestion des sauvegardes de presets
//-----------------------------------
void gestion_menu_preset_edit(void) {
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("PRESETS>Name"));
		lcd.setCursor(0,1);
		affChaineLcd(nomPreset);
		lcd.setCursor(0,1);		
		lcd.cursor();
		return;
	}
	valbouton=readButtons();
	switch(valbouton) {
		// si bouton menu
		case B_MENU:
			lcd.noCursor();
			reset_menu();
			return;
			break;
		case B_ENC1:
			nomPreset[posCarPreset]=' ';
			lcd.setCursor(posCarPreset,1);
			lcd.print(F(" "));
			if (posCarPreset<15)
				 posCarPreset++;  
			break;
			
		case B_ENC2:
			lcd.noCursor();
			save_preset(noPreset);
			reset_menu();
			return;
			break;
	}		
	
	valencodeurs=readEncodeurs();
	gestionEncPresetsName();
	
}
//-----------------------------------
// Gestion des sauvegardes de presets
//-----------------------------------
void gestion_menu_preset_save(void) {
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("PRESETS>Save"));
		load_preset_name(noPreset);
		lcd.setCursor(0,1);
		affChaineLcd(nomPreset);		
		return;
	}
	valbouton=readButtons();
	switch(valbouton) {
		// si bouton menu
		case B_MENU:
			reset_menu();
			return;
			break;
		// si validation de preset a sauvegarder
		case B_ENC2:
			modeMenu=MENUPRESETSEDIT;
			posCarPreset=0;
			refreshMenu=1;
			return;
			break;
			
	}		
	
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (noPreset<127) {
				noPreset++;
				load_preset_name(noPreset);
				lcd.setCursor(0,1);
				affChaineLcd(nomPreset);
			}
			break;
		case ENC1DOWN:
			if (noPreset>0) {
				noPreset--;
				load_preset_name(noPreset);
				lcd.setCursor(0,1);
				affChaineLcd(nomPreset);
			}
			break;
	}	
}

//----------------------------------
// Gestion des chargement de presets
//----------------------------------
void gestion_menu_preset_load(void) {
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("PRESETS>Load"));
		load_preset_name(noPreset);
		lcd.setCursor(0,1);
		affChaineLcd(nomPreset);
		return;
	}
	valbouton=readButtons();
	switch(valbouton) {
		// si bouton menu
		case B_MENU:
			reset_menu();
			return;
			break;
		// si validation de preset
		case B_ENC2:
			load_preset(noPreset);
			return;
			break;
			
	}		
	
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (noPreset<127) {
				noPreset++;
				load_preset_name(noPreset);
				lcd.setCursor(0,1);
				affChaineLcd(nomPreset);
			}
			break;
		case ENC1DOWN:
			if (noPreset>0) {
				noPreset--;
				load_preset_name(noPreset);
				lcd.setCursor(0,1);
				affChaineLcd(nomPreset);
			}
			break;
	}	
}


//-----------------------------------------
// Choix du sous-menu a partir de la racine
// 0) choix tables ondes
// 1) reglage du volume des tables ondes
// 2) reglage semi-tone
// 3) reglage PWM
// 4) edition ADSR
// 5) mode unison
// 6) reglage LFO1 et destinations
// 7) reglage LFO2 et destinations
// 8) debug
//-----------------------------------------
void gestion_menu_synthe(void) {
	raflcd1=false;
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.print(F("EDIT>"));
		raflcd1=true;
	}
	valbouton=readButtons();
	switch(valbouton) {
		// si bouton menu
		// on revient au debut
		case B_MENU:
			reset_menu();
			return;
			break;
		// Si validation
		// On entre dans le parametre
		case B_ENC2:
			modeMenu=modeMenu+100;
			refreshMenu=1;
			return;
			break;
			
	}		
	// Lecture des encodeurs 
	// pour changement de parametre
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (modeMenu<MAXMENUREG) {
				modeMenu++;
				raflcd1=true;
			}
			break;
		case ENC1DOWN:
			if (modeMenu>MINMENUREG) {
				modeMenu--;
				raflcd1=true;
			}
			break;
	}		
	if (raflcd1) {
		raflcd1=false;
		lcd.setCursor(5,0);
		switch(modeMenu) {
			case CHEDITWAVE:
				lcd.print(F("WAVES"));
				break;
			case CHEDITAMPS:  
				lcd.print(F("AMPS"));
				break;
			case CHEDITONE:   
				lcd.print(F("SEMI"));
				break;
			case CHEDITPWM:   
				lcd.print(F("PWM"));
				break;
			case CHEDITADSR:  
				lcd.print(F("ADSR"));
				break;
			case CHEDITUNISON:
				lcd.print(F("UNISON"));
				break;
			case CHEDITLFO1:  
				lcd.print(F("LFO1"));
				break;
			case CHEDITLFO2:
				lcd.print(F("LFO2"));
				break;
			case CHEDITDEBUG:
				lcd.print(F("DEBUG"));
				break;
		}
		lcd.print(F("   "));
	}
}


//--------------------------------------------------
// Gestion du menu racine
// menu principal du synthe
// permet de selectionner le mode reglage ou presets
//--------------------------------------------------
void gestion_menu_racine(void) {
	raflcd1=false;
	if (refreshMenu) {
		refreshMenu=0;
		lcd.clear();
		lcd.print(F(">EDIT |  LOAD"));
		lcd.setCursor(6,1);
		lcd.print(F("|  SAVE "));
		MIDI.read();
		return;
	}
	// Lecture des encodeurs 
	// pour changement de menu eventuel
	valencodeurs=readEncodeurs();
	switch(valencodeurs) {
		case ENC1UP:
			if (modeMenu<MAXMENURAC) {
				modeMenu++;
				raflcd1=true;
			}
			break;
		case ENC1DOWN:
			if (modeMenu>0) {
				modeMenu--;
				raflcd1=true;
			}
			break;
	}	
	// rafraichissement menu racine
	if (raflcd1) {
		// Effacer l'ancien curseur
		lcd.setCursor(0,0);	lcd.print(F(" "));
		lcd.setCursor(8,0);	lcd.print(F(" "));
		lcd.setCursor(8,1);	lcd.print(F(" "));
		switch(modeMenu) {
			case MENURACEDIT:
				lcd.setCursor(0,0);
				break;
			case MENURACLOAD:
				lcd.setCursor(8,0);
				break;
			case MENURACSAVE:
				lcd.setCursor(8,1);
				break;
		}
		// Positionner le nouveau curseur
		MIDI.read();
		lcd.print(F(">"));
		return;
	}	
	// Lecture de bouton
	// pour validation de menu eventuel
	valbouton=readButtons();
	if (valbouton==B_ENC2) {
		switch(modeMenu) {
			case MENURACEDIT:
				// Passer en mode reglages
				modeMenu=MINMENUREG;
				refreshMenu=1;
				return;
				break;
			case MENURACLOAD:
				// Passer en mode chargement de preset
				modeMenu=MENUPRESETSLOAD;
				refreshMenu=1;
				return;
				break;
			case MENURACSAVE:
				// Passer en mode sauvegarde preset
				modeMenu=MENUPRESETSSAVE;
				refreshMenu=1;
				return;
				break;
		}
	}	
}

//--------------------------------------
// Distribution des menus du sythetiseur
//--------------------------------------
void distrib_menu(void) {
	// Si menu racine
	if (modeMenu <10) {
		gestion_menu_racine();
		return;
	}
	// Si menu choix reglage du synthe
	if (modeMenu>=10 && modeMenu<20) {
		gestion_menu_synthe();
		return;
	}
	// Sinon est directement dans une fonction edition ou preset
	switch(modeMenu) {
		// Chargement de presets
		case MENUPRESETSLOAD:
			gestion_menu_preset_load();
			break;
		// Sauvegarde de presets
		case MENUPRESETSSAVE:
			gestion_menu_preset_save();
			break;
		// Edition nom de presets
		case MENUPRESETSEDIT:
			gestion_menu_preset_edit();
			break;
		// Edition forme onde
		case EDITWAVE:
			gestion_menu_wave();
			break; 
		// Edition amplitude OSC
		case EDITAMPS:
			gestion_menu_amps();
			break; 
		// Edition semi-tone
		case EDITONE:
			gestion_menu_semitone();
			break;
		// edition  PWM
		case EDITPWM:
			gestion_menu_pwm();
			break; 
		// Edition ADSR
		case EDITADSR:
			gestion_menu_adsr();
			break; 
		// Edition unison
		case EDITUNISON:
			gestion_menu_unison();
			break; 
		// Edition lfo1
		case EDITLFO1:
			gestion_menu_lfo1();
			break; 
		// Edition  lfo2
		case EDITLFO2:
			gestion_menu_lfo2();
			break; 
		// Ecran debug
		case EDITDEBUG:
			gestion_menu_debug();
			break; 			
	}
	// Si nous sommes en edition
	if (modeMenu>100) {
		valbouton=readButtons();
		switch(valbouton) {
			// si bouton menu
			case B_MENU:
				modeMenu-=100;
				refreshMenu=1;
				return;
				break;
		}			
	}
}

