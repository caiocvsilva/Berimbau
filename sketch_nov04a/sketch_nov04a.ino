const int DEDO_1=50; //p48
const int DEDO_2=48;//p46
const int DEDO_3=46;//p44
const int DEDO_4=44; //p50
//const int DEDO_5=52; //p40

int dedosAtivos [5] = {9,9,9,9,9};


void initialize_pin(){
  pinMode(DEDO_1, INPUT); 
  pinMode(DEDO_2, INPUT); 
  pinMode(DEDO_3, INPUT); 
  pinMode(DEDO_4, INPUT); 
//  pinMode(DEDO_5, INPUT); 
} 


void readLuvas(){
  dedosAtivos[0] = digitalRead(DEDO_1);
  dedosAtivos[1] = digitalRead(DEDO_2);
  dedosAtivos[2] = digitalRead(DEDO_3);
  dedosAtivos[3] = digitalRead(DEDO_4);
//  dedosAtivos[4] = digitalRead(DEDO_5);
}

void setup() {
  Serial.begin(9600);
  initialize_pin();
}




void loop() {
  
  readLuvas();
  for(int i=0;i<4;i++){
//    if(dedosAtivos[i] ==1){
      Serial.print("Dedo ");
      Serial.print(i);
      Serial.print(" Vai: ");
      Serial.print(dedosAtivos[i]);
      Serial.println();
      
//   }
  }//Serial.println("-----");
  delay(500);
}

