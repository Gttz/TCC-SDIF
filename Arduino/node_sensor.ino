/*
 *  Bibliotecas e outras importações
 */
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <MQUnifiedsensor.h>
#include <EBYTE.h                                                                                                                                                                       >

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
static const PROGMEM uint32_t DEV_EUI = 0x03A6EEF9;
static const PROGMEM uint32_t GAT_EUI = 0x98657D12;
static const PROGMEM uint8_t  CH_CTRL = 0x00;
static const PROGMEM uint32_t WAIT    = 30;
static const PROGMEM uint32_t BATCH   = 40;

/*
 *  Pinos
 */
static const int PINO_E32_RX = 2;
static const int PINO_E32_TX = 3;
static const int PINO_E32_M0 = 4;
static const int PINO_E32_M1 = 5;
static const int PINO_E32_AX = 6;               
static const int PINO_GPS_RX = 7;
static const int PINO_GPS_TX = 8;
static const int PINO_LM35   = A0;

/*
 *  Constantes e variaveis para uso do sensor MQ135
 */
static const char  PLACA [12]     = "Arduino Mega";
static const char  TIPO  [7]      = "MQ-135";
static const int   RES_VOLTAGEM   = 5;
static const int   PINO_MQ135     = A1;
static const int   ADC_RES_BIT    = 10;
static const float RAZAO_AR       = 3.6;


/*
 *  Constantes e variáveis auxiliares para o ciclo de captura
 */
static const int CICLO_INTERVALO = 30;
static const int QNTD_LEITURAS   = 10;

static double leituras[QNTD_LEITURAS];

/*
 *  Contadores e auxiliares
 */
static int      i, j, k;
static float    calc_rzero;
static uint16_t contagem;
unsigned long   ultima;

/*
 *  Variáveis para armazenamento da concentração de gás
 */
static double concentracao;

/*
 *  Variáveis para armazenamento da temperatura
 */
static double temperatura;

static uint8_t canal_atual;

/*
 *  Declarações relativas ao GPS
 */
SoftwareSerial GYNSerial(PINO_GPS_RX, PINO_GPS_TX);
TinyGPSPlus    gps;

/*
 *  Declarações relativas ao transmissor-receptor
 */
SoftwareSerial E32Serial(PINO_E32_RX, PINO_E32_TX);
EBYTE          transceptor(&E32Serial, PINO_E32_M0, PINO_E32_M1, PINO_E32_AX);

/*
 *  Instanciação do sensor de gases
 */
MQUnifiedsensor MQ135(PLACA, RES_VOLTAGEM, ADC_RES_BIT, PINO_MQ135, TIPO);

/*
 *  Variáveis para armazenamento da localização 
 */
static const int MAX_LAT_LONG = 12;
static const int MAX_DATA     = 15; 
static char      latitude [MAX_LAT_LONG];
static char      longitude[MAX_LAT_LONG];
static char      data     [MAX_DATA];

void setup(){

  Serial.begin(9600);
  while(!Serial);
  
  Serial.println("[*] Inicializando nó sensor... ");

  Serial.println("\t[*] Inicializando interface de comunicação serial com o módulo GPS... ");
  
  GYNSerial.begin(9600);
  while(!GYNSerial);

  Serial.println("\t[*] Interface de comunicação serial com o módulo GPS inicializada ");

  Serial.println("\t[*] Inicializando interface de comunicação serial com o Transmissor-Receptor... ");
  
  E32Serial.begin(9600);
  while(!E32Serial);

  Serial.println("\t[*] Interface de comunicação serial com o Transmissor-Receptor inicializada ");

  Serial.println("\t[*] MQ135 -> Configurando o método de regressão... ");
  
  MQ135.setRegressionMethod(1);

  if(MQ135.getRegressionMethod().equalsIgnoreCase("exponential")){
    
    Serial.println("\t[*] MQ135 -> Configurado ");
    
  }else{
    
    Serial.println("[!] Houve algum problema, encerrando... ");
    return;
  
  }

  Serial.println("\t[*] MQ135 -> Inicializando módulo... ");
  
  MQ135.init();

  Serial.println("\t[*] MQ135 -> Módulo inicializado ");
  
  Serial.println("\t[*] MQ135 -> Configurando o resistor de carga (RL)... ");
  
  MQ135.setRL(1.0);

  if(MQ135.getRL() == 1.0){
  
    Serial.println("\t[*] MQ135 -> Configurado ");
  
  }else{
  
    Serial.println("[!] Houve algum problema, encerrando...");
    return;
  
  }

  Serial.println("\t[*] MQ135 -> Calibrando sensor de gases - pré-aquecimento... ");
  
  calc_rzero = 0;
  
  for(i=0; i<QNTD_LEITURAS; i++){
    
    MQ135.update();
    calc_rzero += MQ135.calibrate(RAZAO_AR);
  
  }

  MQ135.setR0(calc_rzero/QNTD_LEITURAS);

  Serial.println("\t[*] MQ135 -> Calibragem finalizada ");

  Serial.println("\t[*] E32 -> Inicializando módulo... ");

  transceptor.init();
  transceptor.SetChannel(CH_CTRL);
  transceptor.SetAirDataRate(ADR_19200);
  transceptor.SaveParameters(TEMPORARY);

  Serial.println("\t[*] E32 -> Módulo inicializado ");
  Serial.println("\t[*] E32 -> Parâmetros ");
  Serial.print("\t[*] E32 -> Canal atual -> ");
  Serial.println(transceptor.GetChannel());
  Serial.print("\t[*] E32 -> Taxa de transmissão -> ");
  Serial.println(transceptor.GetAirDataRate());

  canal_atual = CH_CTRL;

  Serial.println("\t[*] Inicializando pinos e valores... ");

  pinMode(PINO_LM35,  INPUT);

  concentracao = 0.0;
  temperatura  = 0.0;

  strncpy(latitude ,"Indefinido",  MAX_LAT_LONG);
  strncpy(longitude,"Indefinido",  MAX_LAT_LONG);
  strncpy(data     ,"Indefinido",  MAX_DATA    );

  Serial.println("\t[*] Valores e pinos inicializados ");  

  Serial.println("[*] Nó sensor inicializado ");

  Serial.println("[*] Valores iniciais ");
  exibir();
  
}

void loop(){

  Serial.println("[*] Iniciando conexao...");
  
  while(!conectar()){
    
    Serial.println("[*] Tentativa falha, tentando novamente...");
    delay(1000);

  }

  Serial.println("[*] Pedido de conexão aceita");
  Serial.println("[*] Trocando canal...");

  transceptor.SetChannel(canal_atual);
  transceptor.SaveParameters(TEMPORARY);

  Serial.println("[*] Enviando confirmação de troca de canal...");

  while(!confirmar()){
    
    Serial.println("[*] Tentativa falha, tentando novamente...");
    delay(1000);
    
  }

  Serial.println("[*] Confirmação aceita");
  Serial.println("[*] Pronto para enviar");
  Serial.println("[*] Iniciando envio...");
  
  while(!enviarDadosSensores()){

    Serial.println("[*] Tentativa falha, tentando novamente...");
    delay(1000);
    
  }
  
  Serial.println("[*] Dados aceitos");

  Serial.println("[*] Encerrando ciclo...");

  concentracao = 0.0;
  temperatura  = 0.0;

  transceptor.SetChannel(CH_CTRL);
  transceptor.SaveParameters(TEMPORARY);

  canal_atual = CH_CTRL;

  Serial.println("[*] Aguardando para reiniciar ciclo...");

  delay(30000);

  Serial.println("[*] Reiniciando ciclo...");
  
}

void capturar(){
  
  obterConcentracao();
  obterTemperatura();
  obterLocalizacao();
  
  exibir();
  
}

void obterConcentracao(){

  Serial.println("[*] Inicializando ciclo de leituras do sensor de concentração de gases... ");

  for(i=0; i<QNTD_LEITURAS; i++){

    MQ135.update();

    MQ135.setA(110.47);
    MQ135.setB(-2.862);

    leituras[i] = MQ135.readSensor();
    
  }

  Serial.println("[*] Ciclo de leituras encerrado ");

  Serial.println("[*] Iniciando remoção de outliers... ");

  removerOutliers(leituras);

  Serial.println("[*] Outliers removidos ");

  Serial.println("[*] Obtendo valor médio a partir das amostras obtidas... ");

  for(i=0, j=0; i<QNTD_LEITURAS; i++){
    if(leituras[i] != 0){
      concentracao += leituras[i];
      j++;
    }
  }

  concentracao = concentracao/j;

  Serial.print("[*] Valor obtido -> ");
  Serial.println(concentracao);
  
}

void obterTemperatura(){

  Serial.println("[*] Inicializando ciclo de leituras do sensor de gases... ");

  Serial.println("[*] Alterando a referência analógica para 1.1v... ");
  
  analogReference(INTERNAL1V1);
  
  Serial.println("[*] Alterado ");
  
  for(i=0; i<QNTD_LEITURAS; i++){
    leituras[i] = analogRead(PINO_LM35)/9.31;
  }

  Serial.println("[*] Ciclo de leituras encerrado ");

  Serial.println("[*] Iniciando remoção de outliers... ");

  removerOutliers(leituras);

  Serial.println("[*] Ouliers removidos ");

  Serial.println("[*] Obtendo valor médio a partir das amostras obtidas... ");

  for(i=0, j=0; i<QNTD_LEITURAS; i++){
    if(leituras[i] != 0){
      temperatura += leituras[i];
      j++;
    }
  }

  temperatura = temperatura/j;

  Serial.print("[*] Valor obtido -> ");
  Serial.println(temperatura);

  Serial.println("[*] Alterando a referência analógica para padrão... ");
  
  analogReference(DEFAULT);

  Serial.println("[*] Alterado");
  
}

void obterLocalizacao(){

  Serial.println("[*] Inicializando tentativa de obtenção de localização... ");

  while(GYNSerial.available()){
    
    gps.encode(GYNSerial.read());
    
  }

  if(gps.location.isUpdated()){

    Serial.println("[*] Nova localização obtida");
    
    dtostrf(gps.location.lat(),9,7,latitude);
    dtostrf(gps.location.lng(),9,7,longitude);
    sprintf(data,"%lu%02d%02d%02d", gps.date.value(),(uint32_t*)(gps.time.hour()-3),(uint32_t*)gps.time.minute(),(uint32_t*)gps.time.second());
  
    Serial.println("[*] Localizacao");
    Serial.print("\tData -> ");
    Serial.println(data);
    Serial.print("\tLatitude -> ");
    Serial.println(latitude);
    Serial.print("\tLongitude -> ");
    Serial.println(longitude);
    
  }
  
}

void exibir(){
  
  Serial.print("[*] Temperatura -> ");
  Serial.println(temperatura);
  Serial.print("[*] Concetracao -> ");
  Serial.println(concentracao);
  Serial.print("[*] Data -> ");
  Serial.println(data);
  Serial.print("[*] Latitude -> ");
  Serial.println(latitude);
  Serial.print("[*] Longitude -> ");
  Serial.println(longitude);
  
}

/*
 *  Função utilizada para conexão com gateway
 */
bool conectar(){

  Serial.println("[*] Inicializando tentativa de conexão com gateway... ");

  int tamanho_cabecalho = 14;

  Serial.println("[*] Iniciando contador de mensagens em 1... ");
  
  contagem = 0x0001;

  Serial.println("[*] Iniciado ");

  Serial.println("[*] Criação da mensagem de requisicao... ");
  
  Mensagem estaDisponivel = requisicaoConexao();

  Serial.println("[*] Mensagem criada ");
  Serial.print("[*] Tamanho Cabec. -> ");
  Serial.println(tamanho_cabecalho);
  Serial.print("[*] Destino -> ");
  Serial.println(estaDisponivel.destino_eui);
  Serial.print("[*] Origem -> ");
  Serial.println(estaDisponivel.origem_eui);
  Serial.print("[*] Sequencia -> ");
  Serial.println(estaDisponivel.sequencia);
  Serial.print("[*] Flag -> ");
  Serial.println(estaDisponivel.flag);
  Serial.print("[*] Checksum -> ");
  Serial.println(estaDisponivel.checksum);
  Serial.print("[*] Payload -> ");
  Serial.println(estaDisponivel.payload);

  Serial.println("[*] Enviando mensagem... ");

  transceptor.SendStruct(&estaDisponivel, sizeof(estaDisponivel));

  Serial.println("[*] Enviado ");

  Serial.println("[*] Aguardando resposta... ");

  Mensagem disponivel = receber();

  if(disponivel.origem_eui == GAT_EUI){
    if(disponivel.destino_eui == DEV_EUI){
      if(disponivel.flag == SYNACK){
        
        Serial.println("[*] Resposta recebida ");
        Serial.println("[*] Verificando integridade do cabecalho... ");
        
        uint8_t *cabecalho = (uint8_t*)&disponivel;
        if(verificarChecksum(cabecalho, &tamanho_cabecalho, &disponivel.checksum)){
          
          Serial.println("[*] Integridade verificada e válida ");
          
          canal_atual = disponivel.payload[0];
          contagem = contagem + 0x0001;
          return true;
          
        }
      }
    }
  }

  Serial.println("[*] Resposta inválida ou não respondidada ");
  
  return false;
  
}

bool confirmar(){

  int tamanho_cabecalho = 14;

  Serial.println("[*] Criando mensagem de confirmação ");
  
  Mensagem confirmado = confirmacao();

  Serial.println("[*] Mensagem criada ");
  Serial.print("[*] Tamanho Cabec. -> ");
  Serial.println(tamanho_cabecalho);
  Serial.print("[*] Destino -> ");
  Serial.println(confirmado.destino_eui);
  Serial.print("[*] Origem -> ");
  Serial.println(confirmado.origem_eui);
  Serial.print("[*] Sequencia -> ");
  Serial.println(confirmado.sequencia);
  Serial.print("[*] Flag -> ");
  Serial.println(confirmado.flag);
  Serial.print("[*] Checksum -> ");
  Serial.println(confirmado.checksum);
  Serial.print("[*] Payload -> ");
  Serial.println(confirmado.payload);

  Serial.println("[*] Enviando mensagem... ");

  transceptor.SendStruct(&confirmado, sizeof(confirmado));

  Serial.println("[*] Mensagem enviada ");

  Serial.println("[*] Aguardando resposta... ");

  Mensagem pronto = receber();

  if(pronto.origem_eui == GAT_EUI){
    if(pronto.destino_eui == DEV_EUI){
      if(pronto.flag == READY){

        Serial.println("[*] Resposta recebida ");
        Serial.println("[*] Verificando integridade do cabecalho... ");
        
        uint8_t *cabecalho = (uint8_t*)&pronto;
        if(verificarChecksum(cabecalho, &tamanho_cabecalho, &pronto.checksum)){
          Serial.println("[*] Integridade verificada e válida ");
          contagem = contagem + 0x0001;
          return true;
        }
      }
    }
  }

  Serial.println("[*] Resposta inválida e não respondida ");
  
  return false;  
}

bool enviarDadosSensores(){

  int tamanho_cabecalho = 14;

  char aux[MAX_PAYLOAD];

  char bufCon[32];
  char bufTemp[32];

  obterConcentracao();
  obterTemperatura();
  obterLocalizacao();

  dtostrf(concentracao, 7, 2, bufCon );
  dtostrf(temperatura , 5, 2, bufTemp);
  
  snprintf(aux, sizeof(aux), "{Concentracao:%s,Temperatura:%s,Data:%s,Latitude:%s,Longitude:%s}", bufCon, bufTemp, data, latitude, longitude);

  uint8_t max_payload = MAX_PAYLOAD;

  Serial.println("[*] Criando mensagem com dados... ");
  
  Mensagem dados = sensores(aux, &max_payload);

  Serial.println("[*] Mensagem criada");
  Serial.print("[*] Tamanho Cabec. -> ");
  Serial.println(tamanho_cabecalho);
  Serial.print("[*] Destino -> ");
  Serial.println(dados.destino_eui);
  Serial.print("[*] Origem -> ");
  Serial.println(dados.origem_eui);
  Serial.print("[*] Sequencia -> ");
  Serial.println(dados.sequencia);
  Serial.print("[*] Flag -> ");
  Serial.println(dados.flag);
  Serial.print("[*] Checksum -> ");
  Serial.println(dados.checksum);
  Serial.print("[*] Payload -> ");
  Serial.println(dados.payload);

  Serial.println("[*] Enviando mensagem... ");

  transceptor.SendStruct(&dados, sizeof(dados));

  Serial.println("[*] Mensagem enviada ");
  Serial.println("[*] Aguardando resposta... ");

  Mensagem integridade = receber();

  if(integridade.origem_eui == GAT_EUI){
    if(integridade.destino_eui == DEV_EUI){
      if(integridade.flag == BVACK){

        Serial.println("[*] Resposta recebida ");
        Serial.println("[*] Verificando integridade do cabecalho... ");
        
        uint8_t *cabecalho = (uint8_t*)&integridade;
        if(verificarChecksum(cabecalho, &tamanho_cabecalho, &integridade.checksum)){
          Serial.println("[*] Integridade verificada e válida ");
          Serial.println("[*] Verificando erro de sequência... ");
          if(integridade.payload[0] == '1'){
            Serial.println("[*] Sem erros de sequência ");
            contagem = contagem + 0x0001;
            return true;
          }
        }
      }
    }
  }

  return false;
  
}

Mensagem sensores(char* dados, uint8_t* qntd_dados){

  int tamanho_cabecalho = 14;

  contagem = 0x0001;

  Mensagem pacote;
  pacote.destino_eui = GAT_EUI;
  pacote.origem_eui = DEV_EUI;
  pacote.servico = DADOS_SENSORES;
  pacote.sequencia = contagem;
  pacote.flag = DATA;
  pacote.tamanho_payload = *qntd_dados;
  pacote.tamanho_lote = 0x01;
  pacote.checksum = 0x0000;

  uint8_t *cabecalho = (uint8_t*)&pacote;
  
  pacote.checksum = calcularChecksum(cabecalho, &tamanho_cabecalho);

  strncpy(pacote.payload,dados,*qntd_dados);
  
  return pacote;
  
}

Mensagem requisicaoConexao(){

  int tamanho_cabecalho = 14;

  Mensagem estaDisponivel;
  estaDisponivel.destino_eui = GAT_EUI;
  estaDisponivel.origem_eui = DEV_EUI;
  estaDisponivel.servico = SINAL;
  estaDisponivel.sequencia = contagem;
  estaDisponivel.flag = SYN;
  estaDisponivel.tamanho_payload = 0x00;
  estaDisponivel.tamanho_lote = 0x01;
  estaDisponivel.checksum = 0x0000;

  uint8_t *cabecalho = (uint8_t*)&estaDisponivel;
  
  estaDisponivel.checksum = calcularChecksum(cabecalho, &tamanho_cabecalho);

  return estaDisponivel;
  
}

Mensagem confirmacao(){

  int tamanho_cabecalho = 14;
  
  Mensagem confirmacao;
  confirmacao.destino_eui = GAT_EUI;
  confirmacao.origem_eui = DEV_EUI;
  confirmacao.servico = SINAL;
  confirmacao.sequencia = contagem;
  confirmacao.flag = ACK;
  confirmacao.tamanho_payload = 0x00;
  confirmacao.tamanho_lote = 0x01;
  confirmacao.checksum = 0x0000;

  uint8_t *cabecalho = (uint8_t*)&confirmacao;
  
  confirmacao.checksum = calcularChecksum(cabecalho, &tamanho_cabecalho);

  return confirmacao;
  
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

/*
 * Função destinada a remoção de outliers provenientes de erros de leitura através do método de Tukey (boxplot)
 */
void removerOutliers(double* dados){
  
  ordenar(dados, 0, QNTD_LEITURAS-1);

  double q1 = obterPrimeiroQuartil(dados);
  double q3 = obterTerceiroQuartil(dados);
  double iqr = q3 - q1;
  double superior = q1 - (1.5*iqr);
  double inferior = q3 + (1.5*iqr);

  for(i=0; i<QNTD_LEITURAS; i++){
    if(dados[i] < inferior || dados[i] > superior){
      dados[i] = 0;
    }
  }
}

double obterPrimeiroQuartil(double* dados){
    return mediana(dados,0,QNTD_LEITURAS/2);
}

double obterTerceiroQuartil(double* dados){
  if(QNTD_LEITURAS%2 == 0){
    return mediana(dados, QNTD_LEITURAS/2, QNTD_LEITURAS-1);
  }else{
    return mediana(dados, (QNTD_LEITURAS/2)+1, QNTD_LEITURAS-1);
  }
}

double mediana(double* dados, int inicio, int fim){
  if((inicio+fim)%2 == 0){
    return dados[(inicio+fim)/2];
  }else{
    return (dados[(inicio+fim)/2] + dados[((inicio+fim)/2)+1])/2;
  }
}

/*
 *  Função para ordenação de um conjunto de dados a partir do algoritmo Mergesort
 */
void ordenar(double* dados, int inicio, int fim){

  if(inicio < fim){
    int meio = inicio + (fim-1) / 2;
    ordenar(dados, inicio, meio);
    ordenar(dados, meio+1, fim);
    merge(dados, inicio, meio, fim);
  }
  
}

void merge(double* dados, int inicio, int meio, int fim){
  
  int n1 = meio - inicio + 1;
  int n2 = fim - meio;
  
  int aux1[n1], aux2[n2];
  
  for (i = 0; i < n1; i++){
    aux1[i] = dados[inicio + i];
  }
  for (j = 0; j < n2; j++){
    aux2[j] = dados[meio + 1 + j];
  }
      
  i = 0;
  j = 0;
  k = inicio;
  
  while(i<n1 && j<n2){
    if(aux1[i] <= aux2[j]){
      dados[k] = aux1[i];
      i++;
    }else{
      dados[k] = aux2[j];
      j++;
    }
    k++;
  }
  
  while (i < n1) {
      dados[k] = aux1[i];
      i++;
      k++;
  }
  while (j < n2) {
      dados[k] = aux2[j];
      j++;
      k++;
  }
  
}
