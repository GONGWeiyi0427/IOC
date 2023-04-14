/*
   L'ESP32 joue une mélodie sur sa broche 17
   on utilise un signal PWM dont on varie la fréquence.
*/

const int brocheSortie = 17;
// fréquence associée à chaque note
// do, do#, ré, ré#, mi, fa, fa#, sol, sol#, la, la#, si
const float note[12] = {65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47 };

const int nombreDeNotes = 32;
const int tempo = 150; // plus c'est petit, plus c'est rapide

const int melodie[][3] = { {4, 2, 2}, {5, 2, 1}, {7, 2, 3}, {0, 3, 6}, 
{2, 2, 2}, {4, 2, 1},{5, 2, 8}, 
{7, 2, 2},  {9, 2, 1},  {11, 2, 3},  {5, 3, 6},
{9, 2, 2}, {11, 2, 1}, {0, 3, 3}, {2, 3, 3}, {4, 3, 3},
{4, 2, 2}, {5, 2, 1}, {7, 2, 3}, {0, 3, 6},
{2, 3, 2}, {4, 3, 1},{5, 3, 8}, 
{7, 2, 2}, {7, 2, 1}, {4, 3, 3}, {2, 3, 2},
{7, 2, 1}, {5, 3, 3}, {4, 3, 2}, {2, 3, 1},{0, 3, 8}
};

//mario does not seem to work yet !
const int mario[][3] = {
  {9, 4, 4}, {9, 4, 4}, {0, 4, 4}, {9, 4, 4}, {0, 4, 4}, {7, 4, 4}, {9, 4, 4},
  {0, 4, 2}, {0, 4, 2}, {0, 4, 2}, {0, 4, 2},
  {9, 3, 4}, {0, 4, 4}, {12, 4, 4}, {14, 4, 4}, {0, 4, 4}, {11, 4, 4}, {12, 4, 4},
  {0, 4, 2}, {0, 4, 2}, {0, 4, 2}, {0, 4, 2},
  {9, 3, 4}, {0, 4, 4}, {12, 4, 4}, {14, 4, 4}, {0, 4, 4}, {11, 4, 4}, {12, 4, 4},
  {0, 4, 2}, {0, 4, 2}, {0, 4, 2}, {0, 4, 2}
};


void setup() {
  ledcAttachPin(brocheSortie, 0); //broche 17 associée au canal PWM 0
}

void loop() {

  int frequence;

  for ( int i = 0; i < nombreDeNotes ; i++ ) {
    frequence = round(note[melodie[i][0]] * 2.0 * (melodie[i][1] - 1));
    ledcSetup(0, frequence, 12);   
    ledcWrite(0, 2048);  // rapport cyclique 50%
    delay(tempo * melodie[i][2] - 50);
    ledcWrite(0, 0); // rapport cyclique 0% (silence, pour séparer les notes adjacentes)
    delay(50);
  }

delay(2000);

}