/*
 *  Bibliotecas e outras importações
 */
#include <SoftwareSerial.h>
#include <EBYTE.h>

#define MAX_PAYLOAD 239

/*
 *  Estruturas para mensagem
 */
struct Mensagem{
    
  uint32_t destino_eui;
  uint32_t origem_eui;
  uint8_t  servico;
  uint16_t sequencia;
  uint8_t  flag;
  uint8_t  tamanho_payload;
  uint8_t  tamanho_lote;
  uint16_t checksum;
  char  payload[MAX_PAYLOAD];
  
};

struct Lote{

  Mensagem* mensagens;

};

/*
 *  Sinalizações do protocolo / SYN, SYN-ACK, DATA, BVACK, FIN, ACK e READY
 */
static const PROGMEM uint8_t SYN    = 0x01;
static const PROGMEM uint8_t SYNACK = 0x02;
static const PROGMEM uint8_t DATA   = 0x03;
static const PROGMEM uint8_t BVACK  = 0x04;
static const PROGMEM uint8_t FIN    = 0x05;
static const PROGMEM uint8_t ACK    = 0x06;
static const PROGMEM uint8_t READY  = 0x07;

/*
 *  Possiveis servicos
 */
static const PROGMEM uint8_t SINAL          = 0x00;
static const PROGMEM uint8_t DADOS_SENSORES = 0x01;
static const PROGMEM uint8_t DADOS_CAMERA   = 0x02;

/*
 *  Informações da rede
 */
static const PROGMEM uint32_t DEV_EUI            = 0x98657D12;
static const PROGMEM uint8_t  CH_CTRL            = 0x00;
static const PROGMEM uint32_t WAIT               = 30;
static const PROGMEM uint8_t  NUM_CANAIS         = 31;
static const PROGMEM uint8_t  CANAIS[NUM_CANAIS] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F }; 
static uint32_t DEV_CON;

/*
 *  Pinos
 */
static const int PINO_E32_RX = 2;
static const int PINO_E32_TX = 3;
static const int PINO_E32_M0 = 4;
static const int PINO_E32_M1 = 5;
static const int PINO_E32_AX = 6;

/*
 *  Contadores
 */
static int      i, j, k;
static uint16_t contagem;
unsigned long   ultima;

/*
 *  Declarações relativas ao transmissor-receptor
 */
SoftwareSerial E32Serial(PINO_E32_RX,PINO_E32_TX); //RX , TX
EBYTE          transceptor(&E32Serial, PINO_E32_M0, PINO_E32_M1, PINO_E32_AX);

/*
 *  Contadores e auxiliare s
 */
static uint8_t canal_atual;

void setup() {
  
  Serial.begin(9600);

  while(!Serial);

  Serial.println("[*] Inicializando gateway...");
  
  E32Serial.begin(9600);
  
  transceptor.init();
  transceptor.SetChannel(CH_CTRL);
  transceptor.SetAirDataRate(ADR_19200);
  transceptor.SaveParameters(TEMPORARY);

  canal_atual = CH_CTRL;

  Serial.println("[*] Gateway inicializado");
  
  Serial.println("[*] Parametros do transceptor");
  transceptor.PrintParameters();

}

void loop() {

  
  while(!ouvir());

  while(!conectar());
  
  while(!esperar());

  while(!receberDadosSensores());

  setup();
  
}

bool ouvir(){

  int tamanho_cabecalho = 14;

  Mensagem estaDisponivel = receberConexao();

  if(estaDisponivel.destino_eui == DEV_EUI){
    if(estaDisponivel.flag == SYN){
      uint8_t *cabecalho = (uint8_t*)&estaDisponivel;
      if(verificarChecksum(cabecalho, &tamanho_cabecalho, &estaDisponivel.checksum)){

        DEV_CON = estaDisponivel.origem_eui;

        return true;
        
      }
    }
  }

  return false;
}

bool conectar(){

  int tamanho_cabecalho = 14;

  Mensagem sdisponivel = disponivel();
  transceptor.SendStruct(&sdisponivel,sizeof(sdisponivel));

  transceptor.SetChannel(canal_atual);
  transceptor.SaveParameters(TEMPORARY); 

  Mensagem confirmado = receber();

  if(confirmado.destino_eui == DEV_EUI){
    if(confirmado.origem_eui == DEV_CON){
      if(confirmado.flag == ACK){
        uint8_t *cabecalho = (uint8_t*)&confirmado;
        if(verificarChecksum(cabecalho, &tamanho_cabecalho, &confirmado.checksum)){
          
          return true;
          
        }
      }
    }
  }

  canal_atual = CH_CTRL;
  transceptor.SetChannel(canal_atual);
  transceptor.SaveParameters(TEMPORARY);

  return false;
  
}

bool esperar(){

  Mensagem spronto = pronto();
  transceptor.SendStruct(&spronto,sizeof(spronto));
  return true;
  
}

bool receberDadosSensores(){

  int tamanho_cabecalho = 14;

  Mensagem dados = receber();

  if(dados.destino_eui == DEV_EUI){
    if(dados.origem_eui == DEV_CON){
      if(dados.flag == DATA){
        uint8_t *cabecalho = (uint8_t*)&dados;
        if(verificarChecksum(cabecalho, &tamanho_cabecalho, &dados.checksum)){

          Serial.print('\t'); // Sinaliza inicio da transmissão dos dados sensores via serial 

          for(i=0; i<dados.tamanho_payload; i++){
            Serial.print(dados.payload[i]);
          }
          
          Serial.print('\r'); // Sinaliza o final da transmissão dos dados sensores via serial

          char s[1];
          s[0] = 0;

          Mensagem integridade = integro(1, s);

          transceptor.SendStruct(&integridade,sizeof(integridade));
          
          return true;
          
        }else{
          
          char s[1];
          s[0] = 1;

          Mensagem integridade = integro(1, s);

          transceptor.SendStruct(&integridade,sizeof(integridade));
          
          return false;
          
        }
      }
    }
  }
  
  return false;
}

Mensagem receberConexao(){

  bool recebeu = false;

  Mensagem recebida;

  do{
    if(E32Serial.available()){
      
      transceptor.GetStruct(&recebida, sizeof(recebida));

      recebeu = true;
      
    }
  }while(!recebeu);

  return recebida;
  
}

Mensagem receber(){

  Mensagem recebida;

  ultima = millis();

  do{
    if(E32Serial.available()){
      
      transceptor.GetStruct(&recebida, sizeof(recebida));

      return recebida;
      
    }
  }while((millis() - ultima) < WAIT);

  return recebida;
  
}

Mensagem disponivel(){

  int tamanho_cabecalho = 14;
  
  Mensagem disponivel;
  disponivel.destino_eui = DEV_CON;
  disponivel.origem_eui = DEV_EUI;
  disponivel.servico = SINAL;
  disponivel.sequencia = contagem;
  disponivel.flag = SYNACK;
  disponivel.tamanho_payload = 0x00;
  disponivel.tamanho_lote = 0x01;
  disponivel.checksum = 0x0000;

  uint8_t *cabecalho = (uint8_t*)&disponivel;
  
  disponivel.checksum = calcularChecksum(cabecalho, &tamanho_cabecalho);

  canal_atual = rand()%NUM_CANAIS;
  
  disponivel.payload[0] = CANAIS[canal_atual];

  return disponivel;
  
}

Mensagem pronto(){

  
  int tamanho_cabecalho = 14;
  
  Mensagem pronto;
  pronto.destino_eui = DEV_CON;
  pronto.origem_eui = DEV_EUI;
  pronto.servico = SINAL;
  pronto.sequencia = contagem;
  pronto.flag = READY;
  pronto.tamanho_payload = 0x00;
  pronto.tamanho_lote = 0x01;
  pronto.checksum = 0x0000;

  uint8_t *cabecalho = (uint8_t*)&pronto;
  
  pronto.checksum = calcularChecksum(cabecalho, &tamanho_cabecalho);
  
  return pronto;
  
}

Mensagem integro(int tamanho_lote, char* sucesso_lote){

  int tamanho_cabecalho = 14;
  
  Mensagem integro;
  integro.destino_eui = DEV_CON;
  integro.origem_eui = DEV_EUI;
  integro.servico = SINAL;
  integro.sequencia = contagem;
  integro.flag = BVACK;
  integro.tamanho_payload = 0x00;
  integro.tamanho_lote = 0x01;
  integro.checksum = 0x0000;

  uint8_t *cabecalho = (uint8_t*)&integro;
  
  integro.checksum = calcularChecksum(cabecalho, &tamanho_cabecalho);
  
  for(i=0; i<tamanho_lote; i++){

    integro.payload[i] = sucesso_lote[i];
    
  }

  return integro;
  
}

uint16_t calcularChecksum(uint8_t* dados, int* tamanho){
  
  int len = *tamanho;
  i=0;
  
  uint16_t checksum = 0;
  
  while(len>0){
    
    checksum = ((dados[i]<<8) + dados[i+1]) + checksum;

    len -= 2;
    i += 2;
    
  }
  
  checksum = (checksum>>16) + checksum;
  checksum = (unsigned int)~checksum;

  return checksum;
  
}

bool verificarChecksum(uint8_t* dados, int* tamanho, uint16_t* checksum){

  int len = *tamanho;
  i=0;

  uint16_t aux = 0;
  
  while(len>0){
    
    aux = ((dados[i]<<8) + dados[i+1]) + aux;

    len -= 2;
    i += 2;
    
  }
  
  aux = *checksum + aux;
  aux = (aux>>16) + aux;
  aux = (unsigned int)~aux;

  return !aux;
  
}
