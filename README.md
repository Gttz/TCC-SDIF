<h1 align="center">                            Sistema de Detecção de Incêndios Florestais (SDIF)                                                 </h1>
<p align="center">
  <img width="300px" heigth="300px" src="https://github.com/Gttz/TCC-SDIF/blob/main/images/logo.png">
</p>

## :link: Links

- Slides - [SDIF.pdf](<https://github.com/Gttz/>)
- Documento - [SDIF.doc](<https://github.com/Gttz/>)

## :bookmark_tabs: Introdução sobre o projeto

Este projeto refere-se a um sistema de detecção de incêndios florestais que abrange diferentes conceitos de diferentes áreas da ciência da computação, sendo estas áreas: inteligência artificial, visão computacional, sistemas embarcados e computação em nuvem. O projeto pode ser dividido em três etapas fundamentais, sendo elas: etapa de coleta de dados, etapa de tranmissão de dados e etapa de processamento de dados. A primeira etapa envolve o uso de sistemas embarcados e sensores para coletar dados do ambiente, os dispositivos que fazem parte desta etapa são chamados de nós sensores ou dispositivos finais. A segunda etapa é caracterizada pela utilização de módulos tranmissores-receptores acoplados nos dispositivos finais e nos gateways (dispositivos que interceptam os dados e retransmistem estes para o serviço de computação em nuvem). A terceira etapa engloba o uso de um serviço de banco de dados em nuvem, um serviço de armazenamento de dados em nuvem, modelos de inteligência artificial pré-treinados e um software para exibição dos dados.

## :framed_picture: Conjunto de dados de imagens

Para avaliar os modelos de classificação...

## :memo: Arquiteturas Utilizadas

- ConvNet. Proposto por Yann LeCun et al. (1998), são um tipo específico de rede neural artificial bastante eficaz para classificação de imagens pois são capazes de levar em consideração a coerência espacial da imagem, ou seja, que pixels próximos uns dos outros estão frequentemente relacionados.

<p align="center">
<img src="https://github.com/ErickB51/tcc-sdi/blob/main/Imagens/convnet.png"></p>

- InceptionV3. Proposto por Szegedy et al. (2015), é uma arquitetura de CNN que busca a resolução de diversos problemas de reconhecimento de imagens em grande escala e também podem ser utilizados em problemas de transfer learning (descrito anteriormente). O seu diferencial é a presença de módulos extratores de características convolucionais. Estes módulos tem como funcionalidade aprender com  menos parâmetros que contém uma maior gama de informação

<p align="center">
<img src="https://github.com/ErickB51/tcc-sdi/blob/main/Imagens/Inceptionv3.png"></p>

- Xception. Proposto por Chollet et al.(2016), é uma arquitetura de CNN similar ao Inception descrito anteriormente e, tem como diferencial que os módulos de iniciação foram substituidos por convoluções separáveis em profundidade. A Xception possui a mesma quantidade de parâmetros que o InceptionV3 com o total de 36 camadas convolucionais. Assim, tendo um uso mais eficiente dos parâmetros.

<p align="center">
<img src="https://github.com/ErickB51/tcc-sdi/blob/main/Imagens/Xception.png"></p>

- VGG16. Proposto por K. Simonyan et al. (2014), é uma arquitetura de CNN que apresenta uma menor quantidade de camadas e uma distribuição de pesos que é capaz de extrair diversas características. Também são utilizadas com técnicas de transfer learning.

<p align="center">
<img src="https://github.com/ErickB51/tcc-sdi/blob/main/Imagens/VGG16.png"></p>

- MobileNet. Proposto por Howard et al. (2017), é uma arquitetura de CNN que foram criados para a executação de tarefas de visão computacional em dispositivos móveis e sistemas embarcados. Eles são baseados em operações de convolução separáveis em profundidade, que diminui o ônus das operações nas primeiras camadas.

<p align="center">
<img src="https://github.com/ErickB51/tcc-sdi/blob/main/Imagens/MobileNet.png"></p>

## :bookmark_tabs: Resultado dos modelos

- Performance no treinamento e validação: 
O conjunto de dados de treinamento está balanceado e consiste de 1520 imagens no total, onde 760 imagens pertecem a cada classe com resolução espacial de 250 x 250. Este conjunto de dados foi dividido em 80:20:20 (1216 imagens ficaram para o treinamento, 304 para validação e 380 para o teste do modelo). Os resultados da avaliação se encontram na tabela abaixo.

<p align="center">
<img src="https://github.com/ErickB51/tcc-sdi/blob/main/Imagens/results_train.PNG">

- Performance no Testset1:
O conjunto de dados Testset1 está balanceado e consiste de 380 imagens no total, onde 190 imagens pertecem a cada classe com resolução espacial de 250 x 250. Este conjunto de dados foi dividido em 80:20 como descrito anteriormente e, por esta razão possuí menos imagens para teste. Os resultados da avaliação se encontram na tabela abaixo.

<p align="center">
<img src="https://github.com/ErickB51/tcc-sdi/blob/main/Imagens/results_test1.PNG"></p>

- Performance no Testset2:
O conjunto de dados Testset2 está desbalanceado e consiste de 998 imagens no total, onde 755 imagens pertecem a classe fogo e 243 a classe não fogo e as imagens possuem diferentes resoluções. Portanto, este conjunto de dados é mais adequado para realizar a avaliação do desempenho do modelo do que para treinâ-lo por estar desbalanceado. Os resultados da avaliação se encontram na tabela abaixo.

<p align="center">
<img src="https://github.com/ErickB51/tcc-sdi/blob/main/Imagens/results_test2.PNG"></p>

## :bookmark_tabs: Trabalhos Futuros

...

## :bookmark_tabs: Referências

...

## :computer: Desenvolvedores

- Lucas Araújo - [Gttz](<https://github.com/Gttz/>)
- Erick Borges - [ErickB51](<https://github.com/ErickB51>)
- Felipe Rodrigues - [Felipe-Benatto](<https://github.com/Felipe-Benatto>)

## :memo: Licença

Esse projeto está utilizando a licença MIT. clique em [LICENSE]() para visualizar mais detalhes.

---
