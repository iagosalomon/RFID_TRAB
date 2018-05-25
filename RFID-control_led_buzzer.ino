#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

//Pinos
#define LED_VERDE A4
#define LED_VERMELHO A5
#define BUZZER 8
#define SS_PIN 10
#define RST_PIN 9
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

String IDtag = ""; //Variável que armazenará o ID da Tag
bool Permitido = false; //Variável que verifica a permissão 

//Vetor responsável por armazenar os ID's das Tag's cadastradas
String TagsCadastradas[] = {"b5da2b77"};

MFRC522 LeitorRFID(SS_PIN, RST_PIN);    // Cria uma nova instância para o leitor e passa os pinos como parâmetro

int quantidadeNegacoes = 0;

void setup() {
        Serial.begin(9600);             // Inicializa a comunicação Serial
        SPI.begin();                    // Inicializa comunicacao SPI 
        LeitorRFID.PCD_Init();          // Inicializa o leitor RFID
        pinMode(LED_VERDE, OUTPUT);     // Declara o pino do led verde como saída
        pinMode(LED_VERMELHO, OUTPUT);  // Declara o pino do led vermelho como saída
        pinMode(BUZZER, OUTPUT);        // Declara o pino do buzzer como saída
        lcd.begin(16, 2);
}

void loop() {  
  if(quantidadeNegacoes < 3)
    Leitura();  //Chama a função responsável por fazer a leitura das Tag's
}

void Leitura(){

  lcd.clear();
  lcd.print("APROXIME SEU");
  segundaLinha(); 
  lcd.print("CARTÃO DO LEITOR");
  primeiraLinha();
        IDtag = ""; //Inicialmente IDtag deve estar vazia.
        
        // Verifica se existe uma Tag presente
        if ( !LeitorRFID.PICC_IsNewCardPresent() || !LeitorRFID.PICC_ReadCardSerial() ) {
            delay(50);
            return;
        }
        
        // Pega o ID da Tag através da função LeitorRFID.uid e Armazena o ID na variável IDtag        
        for (byte i = 0; i < LeitorRFID.uid.size; i++) {        
            IDtag.concat(String(LeitorRFID.uid.uidByte[i], HEX));
        }        
        
        //Compara o valor do ID lido com os IDs armazenados no vetor TagsCadastradas[]
        for (int i = 0; i < (sizeof(TagsCadastradas)/sizeof(String)); i++) {
          if(  IDtag.equalsIgnoreCase(TagsCadastradas[i])  ){
              Permitido = true; //Variável Permitido assume valor verdadeiro caso o ID Lido esteja cadastrado
          }
        }       

        if(Permitido == true) 
          acessoLiberado(); //Se a variável Permitido for verdadeira será chamada a função acessoLiberado()        
        else 
          acessoNegado(); //Se não será chamada a função acessoNegado()

        delay(2000); //aguarda 2 segundos para efetuar uma nova leitura
}

void acessoLiberado(){
  
  Serial.println("Tag Cadastrada: " + IDtag); //Exibe a mensagem "Tag Cadastrada" e o ID da tag não cadastrada
    efeitoPermitido();  //Chama a função efeitoPermitido()
    Permitido = false;  //Seta a variável Permitido como false novamente
}

void acessoNegado(){
  Serial.println("Tag NAO Cadastrada: " + IDtag); //Exibe a mensagem "Tag NAO Cadastrada" e o ID da tag cadastrada
  efeitoNegado(); //Chama a função efeitoNegado()
}

void efeitoPermitido(){  
    //Ligando o buzzer com uma frequência de 1500 hz e ligando o led verde.
    lcd.clear();
    digitalWrite(LED_VERDE, HIGH);   
    lcd.print("BEM VINDO IAGO!");
    delay(3000);   
    

    //Desligando o buzzer e led verde.      
    digitalWrite(LED_VERDE, LOW);
    lcd.clear();
    delay(100);
  
}

void efeitoNegado(){

    quantidadeNegacoes++;
    
    lcd.clear();
    digitalWrite(LED_VERMELHO, HIGH);
    lcd.print("CARTÃO RECUSADO");   
    delay(3000); 

    //Desligando o buzzer e o led vermelho.
    digitalWrite(LED_VERMELHO, LOW);
    lcd.clear();

    if(quantidadeNegacoes >= 3) {
      
      tone(BUZZER,500);
      digitalWrite(LED_VERMELHO, HIGH);
      primeiraLinha(); 
      lcd.print("Sistema bloquead");
      segundaLinha();
      lcd.print("o.");
      
      delay(3000);

      noTone(BUZZER);
    }
    
}

void primeiraLinha () {
  lcd.setCursor(0,0);
}

void segundaLinha () {
  lcd.setCursor(0,1);
}




