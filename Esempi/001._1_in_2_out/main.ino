/** 
*  Questo codice permette di comandare 2 zone o gruppi di lampade con un solo pulsante.
*  Funzionamento: Se il pulsante viene premuto e rilasciato velocemente vengono attivate o disattivate le uscite, 
*                 se il pulsante viene premuto mantenendolo premuto si attiva il flip-flop sulle uscite, fino al suo rilacio.
*/


/* Azioni sui relé 
*    0 = stanby
*    1 = click veloce
*    2 = flip-flop
*/
int azione = 0;

#define RELE_1_PIN 12
#define RELE_2_PIN 13
#define PULSANTE_1_PIN 8

class Rele {
private:
  byte pin;
public:
  Rele(byte pin) {
    this->pin = pin;
    init();
  }
  Rele() {}
  void init() {
    pinMode(pin, OUTPUT);
    off();
  }
  void on() {
    digitalWrite(pin, HIGH);
  }
  void off() {
    digitalWrite(pin, LOW);
  }
  void change() {
    digitalWrite(pin, !digitalRead(pin));
  }
  int getData() {
    return digitalRead(pin);
  }
};

class Pulsante {
private:
  byte pin;
  bool stato;
  long tempoPressione;
public:
  Pulsante(byte pin) {
    this->pin = pin;
    init();
  }
  Pulsante() {}
  void init() {
    pinMode(pin, INPUT);
  }
  int getData() {
    return digitalRead(pin);
  }

  bool getStatoPrecedente() {
    return this->stato;
  }
  void setStatoPrecedente(bool statoPrecedente) {
    this->stato = statoPrecedente;
  }
  long getTempoPressione() {
    return this->tempoPressione;
  }
  void setTempoPressione(long tempoPressione) {
    this->tempoPressione = tempoPressione;
  }
};

Rele rele1(RELE_1_PIN);
Rele rele2(RELE_2_PIN);
Pulsante pulsante1(PULSANTE_1_PIN);

void setup() {
  Serial.begin(9600);
}

void loop() {

  //premo il pulsante (pulldown)
  if (!pulsante1.getStatoPrecedente() && pulsante1.getData()) {
    // Serial.println("Pulsante premuto");
    pulsante1.setTempoPressione(millis());
    pulsante1.setStatoPrecedente(pulsante1.getData());
    azione = 1;
  }

  //lascio il pulsante (pullup)
  if (pulsante1.getStatoPrecedente() && !pulsante1.getData()) {
    // Serial.println("fine conteggio");
    // long tP = millis() - pulsante1.getTempoPressione();
    // Serial.print("Pulsante rilasciato dopo (ms): ");
    // Serial.println(tP);
    if (azione == 1) {
      if (rele1.getData() || rele2.getData()) {
        rele1.off();
        rele2.off();
      } else {
        rele1.change();
        rele2.change();
      }
    } else {
      azione = 0;
    }
    pulsante1.setStatoPrecedente(pulsante1.getData());
  }

  long durataPressione = millis() - pulsante1.getTempoPressione();
  if (pulsante1.getData() && durataPressione == 750) {
    azione = 2;
    pulsante1.setTempoPressione(millis());  // resetto tPressione per ricominciare il ciclo
  }

  if (azione == 2) {
    if (rele1.getData()) {
      rele2.on();
      rele1.off();
    } else {
      rele1.on();
      rele2.off();
    }
    azione = 0;
  }
}
