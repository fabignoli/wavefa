//-----------------------------------------
// procedures pour gerer les affichages LCD
// programme principal wavefa.ino
//-----------------------------------------


//--------------------------------------------------------
// Affiche une donne numerique avec des 0 non significatif
// nbdigits = longueur totale du nombre  entre 1 et 3 chiffres
//--------------------------------------------------------
void digitsLcd(byte valeur, byte nbdigits) {
  MIDI.read();      
  if (nbdigits==3 && valeur <100)
      lcd.print(F("0"));
  MIDI.read();
  if (valeur<10)
      lcd.print(F("0"));
  MIDI.read();
  lcd.print(valeur,DEC);
  MIDI.read();
}

//--------------------------------------------------------
// Affiche "On" si valeur >0 sinon Off
// le tout a la position x,y
//--------------------------------------------------------
void OnOffLcd(byte valeur, byte x, byte y) {
  lcd.setCursor(x,y);      
  if (valeur)
      lcd.print(F("On "));
  else
      lcd.print(F("Off"));
}

//---------------------------------------------------
// Affiche une chaine passee en parametre
// avec lecture du midi entre chaque caractere
// Identique au fonctionnement de affChaineLcdPROGMEM
// mais utilise la RAM
//---------------------------------------------------
void affChaineLcd(char *mychaine) {
int i;
char *p;
  p=mychaine;
  for (i=0;i<strlen(mychaine);i++,p++) {
	 MIDI.read();
     lcd.printByte(*p);
  }
}

//----------------------------------------
// Affiche un octet en binaire
// Sert pour du debug
// A enlever une fois le programme termine
//----------------------------------------
void aff_binary(byte oct) {
byte i;
    lcd.print(F("B"));
    for (i=8; i>0; i--) {
        if (bitRead(oct,i-1))
            lcd.print(F("1"));
        else
            lcd.print(F("0"));
    }
}

//---------------------------------------------------------------------
// Affiche une donne numerique avec des 0 non significatif
// nbdigits = longueur totale du nombre  entre 1 et 3 chiffres
// le tout à la position x,y
// Au dela de 999 affiche au format 1.0 (resultat a multiplier par 1000
//---------------------------------------------------------------------
void digitsLcdPos(int valeur, byte x, byte y, byte nbdigits) {
int inter;
  lcd.setCursor(x,y);
  if (valeur>=1000) {
    MIDI.read();      
	// lcd.print("x");
    // MIDI.read();      
	inter=valeur/1000;
	lcd.print(inter);
	lcd.print(".");
	lcd.print((valeur-(inter*1000))/100);
	return;
  }

  MIDI.read();      
  if (nbdigits==3 && valeur <100)
      lcd.print(F("0"));
  MIDI.read();
  if (valeur<10)
      lcd.print(F("0"));
  MIDI.read();
  lcd.print(valeur,DEC);
  MIDI.read();
}
//---------------------------------------------------
// Affiche des chaines statiques sur le LCD
// on aurait pu utiliser lcd.print simplement
// mais cette procedure permet de lire le MIDI
// entre chaque caractere affiche, cela permet
// pendant un affichage important de reduire
// la latence.
// Les chaines sont en PROGMEM pour liberer de la RAM
//---------------------------------------------------
void affOndesLcdPROGMEM(byte item) {

byte pos;
  memset(BuffAff,0,sizeof(BuffAff));
  // recopie item de la flash vers une chaine en RAM
  strcpy_P(BuffAff, (char*)pgm_read_word(&(WaveName[item])));
  for (pos=0;pos<strlen(BuffAff);pos++) {
       lcd.print(BuffAff[pos]);
       MIDI.read();
  }
}
