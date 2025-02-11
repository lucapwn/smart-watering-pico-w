# Smart Watering

Plataforma IoT para monitoramento e controle remoto de um sistema de irrigação sustentável desenvolvido com o Raspberry Pi Pico W.

![Badge](https://img.shields.io/static/v1?label=license&message=MIT&color=1E90FF)
![Badge](https://img.shields.io/static/v1?label=build&message=passing&color=00d110)

## Conteúdo

- [Sobre](#sobre)
- [Funcionalidades](#funcionalidades)
- [Suporte](#suporte)
- [Materiais e Tecnologias](#materiais-e-tecnologias)
  - [Conectando os componentes](#conectando-os-componentes)
  - [Diagrama Esquemático do Protótipo](#diagrama-esquemático-do-protótipo)
  - [Fluxograma do _Firmware_](#fluxograma-do-firmware)
- [Executando a aplicação](#executando-a-aplicação)
  - [Raspberry Pi Pico W](#raspberry-pi-pico-w)
  - [Plataforma IoT](#plataforma-iot)
- [Autor](#autor)
- [Licença](#licença)

## Sobre

O Smart Watering é um sistema de irrigação automatizado, sustentável e de baixo custo, composto por uma plataforma IoT (_SaaS_) com funcionalidades inovadoras, disponível para _smartphones_, _tablets_ e computadores. Este sistema também inclui uma API REST para realizar a comunicação entre a plataforma IoT e o Raspberry Pi Pico W.

Este projeto foi tema do meu Trabalho de Conclusão de Curso (TCC), com o título _“Smart Watering: Um Sistema de Monitoramento Remoto e Controle de Irrigação Sustentável Baseado em Plataforma IoT”_. Além disso, fui bolsista no projeto por um ano, que também foi aprovado e fomentado pelo [Programa Centelha](https://programacentelha.com.br/ce/) - Ceará (2022).

Veja este vídeo de exemplo no YouTube em [```https://www.youtube.com/watch?v=AMdGk-LIUu8```](https://www.youtube.com/watch?v=AMdGk-LIUu8).

![smart-watering](https://github.com/lucapwn/smart-watering-pico-w/blob/main/images/Login%20Page.png)

![smart-watering](https://github.com/lucapwn/smart-watering-pico-w/blob/main/images/Dashboard%20Page.png)

## Funcionalidades

O Smart Watering permite, ao agricultor, diversos recursos e funcionalidades, como:

- Painel de controle **interativo** com gráficos e _widgets_ para visualização, em **tempo real**, dos dados meteorológicos e de irrigação, como **umidade de ar** e **solo**, **temperatura** e **luminosidade** do ambiente, **ponto de orvalho**, **nível de água** do reservatório, **nível de chuva** e **consumo de água** referente aos meses do ano. Essa funcionalidade permite, aos usuários, monitorar a plantação e tomar decisões com base nas condições meteorológicas apresentadas.
- Os usuários podem **agendar a irrigação remotamente** (sem a necessidade de se deslocar até o local da plantação) de **várias formas**, por exemplo: **por dia**, **horário**, **umidade do solo** ou **fluxo de água**. Esta flexibilidade permite, ao agricultor, definir valores dinâmicos e específicos em cada uma das regas, tornando a plataforma adequada para a gestão de diversos tipos de plantas. A rega automática pode ser definida para períodos únicos ou diários.
- Em termos de gestão, os usuários podem **visualizar**, **adicionar**, **editar** e **remover** as regas agendadas. Além disso, podem pesquisar por atributos específicos, aplicar filtros e ordenar informações em colunas de tabelas, personalizar as unidades de temperatura, as medidas do reservatório de água, as notificações do sistema, os temas de cores e o menu da plataforma.
- Gerar relatórios **inteligentes** dos dados coletados dos sensores, presentes na plantação, para uma **análise** posterior e **aprimoramento** de recursos hídricos. Os usuários podem selecionar os dados e períodos de tempo pretendidos e arquivá-los para referência futura.
- Acesso a **página de administração** integrada, a qual facilita a gestão de dados dos usuários, sensores, permissões, autenticações e configurações, garantindo uma plataforma organizada e acessível aos administradores.
- Além disso, o Smart Watering permite acesso a uma **API REST** para usuários obter recursos da plataforma e do sistema de irrigação. Esta interface permite a coleta de dados, em **tempo real**, o envio de comandos para o sistema de irrigação e a configuração da plataforma. A API REST é protegida por autenticação, a qual concede acesso apenas aos usuários autorizados. Através deste recurso, os usuários podem obter acesso instantâneo a informações críticas, incluindo dados de sensores, agendas de irrigação, notificações, consumo de água e configurações da plataforma.

## Suporte

A plataforma é compatível e responsiva em _smartphones_, _tablets_ e computadores, a qual permite sua adaptação em diferentes tamanhos de telas e sistemas operacionais.

## Materiais e Tecnologias

Para montar e desenvolver o sistema de irrigação, são necessários os seguintes materiais e tecnologias:

Material               | Tipo             | Tecnologia                               | Descrição
---------------------- | ---------------- | ---------------------------------------- | --------------------------------------------------
FC-28                  | Sensor           | Wi-Fi (802.11)                           | Protocolo de comunicação sem fio.                 
YL-83                  | Sensor           | REST                                     | Arquitetura de comunicação.                       
DHT11                  | Sensor           | JSON                                     | Formato de dados da API.                          
TEMT6000               | Sensor           | SQLite                                   | Banco de dados relacional.                        
HCSR-04                | Sensor           | C                                        | Linguagem de programação do _firmware_.           
YF-S201                | Sensor           | Django                                   | Framework de desenvolvimento _web_ em Python.     
Bomba de Água          | Atuador          | HTML, CSS e JavaScript                   | Linguagens de marcação, formatação e programação. 
Relé                   | Atuador          | Bootstrap 5                              | _Framework front-end_.                            
RTC DS3231             | Módulo           | jQuery                                   | Biblioteca de manipulação do DOM em JavaScript.   
Raspberry Pi Pico W    | Microcontrolador | ApexCharts, DataTables e Ion.RangeSlider | Biblioteca de gráficos, tabelas e _inputs_.       

### Conectando os componentes

Os GPIOs dos sensores e atuadores devem ser conectados ao Raspberry Pi Pico W da seguinte forma:

Pico W | DS3231 | DHT11 | Relé | YF-S201 | HC-SR04 | TEMT6000 | YL-83 | FC-28 
------ | ------ | ----- | ---- | ------- | ------- | -------- | ----- | ----- 
GP0    | SDA    |       |      |         |         |          |       |       
GP1    | SCL    |       |      |         |         |          |       |       
GP16   |        | OUT   |      |         |         |          |       |       
GP17   |        |       | IN   |         |         |          |       |       
GP18   |        |       |      | IN      |         |          |       |      
GP19   |        |       |      |         | ECHO    |          |       |       
GP20   |        |       |      |         | TRIG    |          |       |       
GP26   |        |       |      |         |         | OUT      |       |       
GP27   |        |       |      |         |         |          | A0    |     
GP28   |        |       |      |         |         |          |       | A0            
3V3    |        |       |      |         |         | VCC      | VCC   | VCC     
5V     | VCC    | VCC   | VCC  | VCC     | VCC     |          |       | 
GND    | GND    | GND   | GND  | GND     | GND     | GND      | GND   | GND 

### Diagrama Esquemático do Protótipo

Após conectar todos os componentes, o sistema deve ser semelhante à imagem ilustrada abaixo.

<img src="https://github.com/lucapwn/smart-watering-pico-w/blob/main/images/Prototype%20Schematic%20Diagram.png" width="700">

### Fluxograma do _Firmware_

O fluxograma abaixo ilustra as interações do _firmware_ do Raspberry Pi Pico W com a plataforma IoT e outros recursos do sistema.

<img src="https://github.com/lucapwn/smart-watering-pico-w/blob/main/images/Firmware%20Flowchart.svg" width="1000">

## Executando a aplicação

### Raspberry Pi Pico W

Instale o GCC, o Visual Studio Code e a extensão Raspberry Pi Pico. Em seguida, importe o projeto ```pico_firmware``` e selecione o SDK na versão ```v1.5.1```.

Edite o arquivo ```main.c```, adicionando suas credenciais de Wi-Fi e endereço IP (ou servidor). As demais configurações são opcionais.

Por fim, configure o CMake, compile o projeto e faça o _upload_ do _firmware_ (```build/main.uf2```) para o Raspberry Pi Pico W.

### Plataforma IoT

Com o Git instalado, clone o repositório do projeto:

~~~console
foo@bar:~$ git clone https://github.com/lucapwn/smart-watering.git
~~~

Com o Python instalado, navegue até a pasta principal do projeto e instale as dependências da aplicação:

~~~console
foo@bar:~$ pip install -r requirements.txt
~~~

Aplique as alterações no banco de dados da aplicação:

~~~console
foo@bar:~$ python manage.py migrate --run-syncdb
~~~

Crie um usuário com permissões de administrador:

~~~console
foo@bar:~$ python manage.py createsuperuser
~~~

Por fim, execute o servidor da aplicação:

~~~console
foo@bar:~$ python manage.py runserver 0.0.0.0:80
~~~

Agora, você pode acessar a aplicação no seu computador através de [http://localhost](http://localhost).

Para usá-la em outro dispositivo, obtenha o endereço IP do computador que está executando a aplicação.

Se tudo estiver configurado corretamente, os dados dos sensores serão exibidos, em tempo real, na plataforma. Obrigado por chegar até aqui!

## Autor

Desenvolvido por [Lucas Araújo](https://github.com/lucapwn).

## Licença

Esse software é licenciado pelo [MIT](https://choosealicense.com/licenses/mit/).
