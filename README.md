<h1 align="center">                            Sistema de Detecção de Incêndios Florestais (SDIF)                                                 </h1>
<p align="center">
  <img width="300px" heigth="300px" src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/logo.png">
</p>

## :link: Links

- Slides - [SDIF.pdf](https://github.com/Gttz/TCC-SDIF/blob/main/Documentos/SDIF.pdf)
- Documento - [SDIF.doc](https://github.com/Gttz/TCC-SDIF/blob/main/Documentos/SDIF.doc)
- Jupyter Notebook - [SDIF.ipynb](https://nbviewer.jupyter.org/github/Gttz/TCC-SDIF/blob/main/Notebooks/SDIF.ipynb)

## :bookmark_tabs: Introdução sobre o projeto

Este projeto refere-se a um sistema de detecção de incêndios florestais que abrange diferentes conceitos de diferentes áreas da ciência da computação, sendo estas áreas: inteligência artificial, visão computacional, sistemas embarcados e computação em nuvem. O projeto pode ser dividido em três etapas fundamentais, sendo elas: etapa de coleta de dados, etapa de tranmissão de dados e etapa de processamento de dados. A primeira etapa envolve o uso de sistemas embarcados e sensores para coletar dados do ambiente, os dispositivos que fazem parte desta etapa são chamados de nós sensores ou dispositivos finais. A segunda etapa é caracterizada pela utilização de módulos tranmissores-receptores acoplados nos dispositivos finais e nos gateways (dispositivos que interceptam os dados e retransmistem estes para o serviço de computação em nuvem). A terceira etapa engloba o uso de um serviço de banco de dados em nuvem, um serviço de armazenamento de dados em nuvem, modelos de inteligência artificial pré-treinados e um software para exibição dos dados.

## :framed_picture: Conjunto de dados de imagens

Os dados foram coletados para treinar modelos para distinguir entre as imagens que contêm fogo (imagens de fogo) e imagens regulares (imagens que não são de fogo), então todo o problema é de classificação binária.

###### Conjunto de dados 1:

- Os dados são divididos em 2 pastas: ``training_set`` e ``test_set``. Ambas as pastas contém uma pasta ``fire`` que contém imagens de fogo ao ar livre, já a pasta ``nofire`` contém imagens da natureza (por exemplo: florestas, árvores, gramas, rios, lagos, animais, estradas e cachoeiras). Ambas as pastas possuem 1520 imagens para treinamento e 380 imagens para teste. O conjunto de dados 1 foi dividido em 60:20:20, então ficamos com estas quantidades de imagens: 1216 para treinamento, 304 para validação, 380 para o primeiro teste. Segue abaixo um exemplo de algumas imagens:

<p align="center">
<img img width="450px" heigth="450px" src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/dataset1.png"></p>

###### Conjunto de dados 2:

- Os dados são divididos em 2 pastas, a pasta ``fire`` contém 755 imagens de fogo ao ar livre, algumas delas contêm fumaça, já a pasta ``nofire`` não são imagens de fogo que contém 244 imagens da natureza (por exemplo: florestas, árvores, gramas, rios, lagos, animais, estradas e cachoeiras). Ambas as pastas possuem 998 imagens para teste. Vale deixar claro que o conjunto de dados 2 está desbalanceado e por este motivo decidimos utiliza-lo apenas para o teste dos modelos. Segue abaixo um exemplo de algumas imagens:

<p align="center">
<img width="450px" heigth="450px" src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/dataset2.png"></p>

## :memo: Arquiteturas Utilizadas

- ConvNet. Proposto por Yann LeCun et al. (1998), são um tipo específico de rede neural artificial bastante eficaz para classificação de imagens pois são capazes de levar em consideração a coerência espacial da imagem, ou seja, que pixels próximos uns dos outros estão frequentemente relacionados.

<p align="center">
<img src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/convnet.png"></p>

- InceptionV3. Proposto por Szegedy et al. (2015), é uma arquitetura de CNN que busca a resolução de diversos problemas de reconhecimento de imagens em grande escala e também podem ser utilizados em problemas de transfer learning (descrito anteriormente). O seu diferencial é a presença de módulos extratores de características convolucionais. Estes módulos tem como funcionalidade aprender com  menos parâmetros que contém uma maior gama de informação

<p align="center">
<img src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/Inceptionv3.png"></p>

- Xception. Proposto por Chollet et al.(2016), é uma arquitetura de CNN similar ao Inception descrito anteriormente e, tem como diferencial que os módulos de iniciação foram substituidos por convoluções separáveis em profundidade. A Xception possui a mesma quantidade de parâmetros que o InceptionV3 com o total de 36 camadas convolucionais. Assim, tendo um uso mais eficiente dos parâmetros.

<p align="center">
<img src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/Xception.png"></p>

- VGG16. Proposto por K. Simonyan et al. (2014), é uma arquitetura de CNN que apresenta uma menor quantidade de camadas e uma distribuição de pesos que é capaz de extrair diversas características. Também são utilizadas com técnicas de transfer learning.

<p align="center">
<img src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/VGG16.png"></p>

- MobileNet. Proposto por Howard et al. (2017), é uma arquitetura de CNN que foram criados para a executação de tarefas de visão computacional em dispositivos móveis e sistemas embarcados. Eles são baseados em operações de convolução separáveis em profundidade, que diminui o ônus das operações nas primeiras camadas.

<p align="center">
<img src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/MobileNet.png"></p>

## :bar_chart: Resultado dos modelos
<p align="center">
Performance no treinamento e validação</p>

<p align="center">
<img src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/results_train.PNG">

<p align="center">
Performance no Testset1</p>
  
<p align="center">
<img src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/results_test1.PNG"></p>

<p align="center">
Performance no Testset2</p>

<p align="center">
<img src="https://github.com/Gttz/TCC-SDIF/blob/main/Images/results_test2.PNG"></p>


## :bookmark_tabs: Conclusão

- Neste estudo investigamos o desempenho de algoritmos de Visão Computacional tais sendo: ConvNet, Inception, Xception, VGG16 e MobileNet.  Cada algoritmo foi avaliado com base
nos valores das métricas de classificação das tabelas anteriores. Fizemos experiências com diferentes otimizações e técnicas que aumentaram consideravelmente o desempenho dos modelos, produzindo precisão de previsão em AUC de [0.93, 0.91], [0.97, 0.88], [0.97, 0.88], [0.93, 0.79] e [0.98, 0.90] para os modelos ConvNet, Inception, Xception, VGG16 e MobileNet no conjunto de dados testset1 e testset2 respectivamente. Com isso, concluimos que o modelo que obteve o melhor resultado foi o MobileNet dentre todas as métricas utilizadas. Também foi levado em consideração que, a criação do algoritmo já foi pensada para sistemas de pequeno porte, sendo sistemas embarcados.

- Também foram analisados aspectos referentes a utilização de sistemas embarcados em uma aplicação voltada a detecção de incêndios florestais. Os resultado obtidos a partir dos cenários de testes puderam confirmar a possibilidade de utilização destes sistemas em conjunto com modelos treinados e um sistema baseado em nuvem.

## :construction: Trabalhos Futuros

- Teste dos métodos de conversão e armazenamento de energia alternativos em cenários reais e determinação da autonomia do sistema
- Teste com uma rede maior de sensores, isto é, realizando a checagem das melhores distâncias entre os nós para melhoria da qualidade de transmissão
- Implementação de criptografia ponta-a-ponta entre nó sensor e gateway
- Implantação de mecanismos de troca de canais para evitar colisões de pacotes

## :bookmark_tabs: Referências

- Khan, Ali; Hassan, Bilal (2020), “Dataset for Forest Fire Detection”, [Mendeley Data](https://data.mendeley.com/datasets/gjmr63rz2r/1)

- Saied, Ahmed; (2020), “Outdoor-fire images and non-fire images for computer vision tasks”, [Kaggle](https://www.kaggle.com/phylake1337/fire-dataset)

- LeCun, Y., Bottou, L., Bengio, Y., and Haffner, P., (1998), “Gradient Based Learning Applied to Document Recognition”, In Proceedings of the IEEE Conference. Publishing Press.

- Szegedy, C., Vanhoucke, V., Ioffe,S., Shlens,J.,&Wojna,Z.(2016) “Rethinking the inception architecture for computer vision” In Proceedings of the IEEE conference on computer vision and pattern recognition.

- François  Chollet (2016), “Xception:  Deep  learning  with  depthwise  separable convolutions”, In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition (CVPR) pages 1251-1258. Publishing Press.

- Simonyan, K. and Zisserman, A. (2014). “Very deep convolutional networks for large-scale image recognition”, In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition (CVPR) pages 1409-1556. Publishing Press.

- N. S. Sanjay and A. Ahmadinia (2019), “MobileNet-Tiny: A Deep Neural Network-Based Real-Time Object Detection for Rasberry Pi”, In 18th IEEE International Conference On Machine Learning And Applications (ICMLA) pages 647-652. Publishing Press.

## :computer: Desenvolvedores

- Lucas Araújo - [Gttz](<https://github.com/Gttz/>)
- Erick Borges - [ErickB51](<https://github.com/ErickB51>)
- Felipe Rodrigues - [Felipe-Benatto](<https://github.com/Felipe-Benatto>)

## :memo: Licença

Esse projeto está utilizando a licença MIT. clique em [LICENSE](https://github.com/Gttz/TCC-SDIF/blob/add-license-1/LICENSE) para visualizar mais detalhes.

---
