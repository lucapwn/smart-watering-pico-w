# Smart Watering

Plataforma IoT para monitoramento e controle remoto de um sistema de irrigação sustentável desenvolvido com o Raspberry Pi Pico W.

![Badge](https://img.shields.io/static/v1?label=license&message=MIT&color=1E90FF)
![Badge](https://img.shields.io/static/v1?label=build&message=passing&color=00d110)

## Conteúdo

- [Sobre](#sobre)
  - [Funcionalidades](#funcionalidades)
- [Suporte](#suporte)
- [Materiais](#materiais)
  - [Conectando os componentes](#conectando-os-componentes)
  - [Exemplo do sistema](#exemplo-do-sistema)
- [Executando a aplicação](#executando-a-aplicação)
  - [Plataforma IoT](#plataforma-iot)
  - [Sistema de irrigação](#sistema-de-irrigação)
- [Autor](#autor)
- [Licença](#licença)

## Sobre

O Smart Watering é uma plataforma IoT desenvolvida com o Raspberry Pi Pico W para monitoramento e controle remoto de um sistema de irrigação sustentável. Este sistema inclui uma API REST para realizar a comunicação entre a plataforma IoT e o Pico W.

Assista este vídeo de exemplo no YouTube em [```https://www.youtube.com/watch?v=AMdGk-LIUu8```](https://www.youtube.com/watch?v=AMdGk-LIUu8).

![smart-watering](https://github.com/lucapwn/smart-watering/assets/58787069/dc8393b1-fac1-4286-b3ee-099fef58f985)

### Funcionalidades

Os usuários podem agendar a irrigação de várias formas, por exemplo, por dia, horário, umidade do solo ou fluxo de água. Esta flexibilidade permite valores dinâmicos e específicos, tornando a plataforma adequada para a gestão de diversos tipos de plantas. A rega automática pode ser definida para períodos únicos ou diários.

Em termos de gestão, os usuários podem visualizar, adicionar, editar e remover as regas agendadas. Além disso, podem pesquisar por atributos específicos, aplicar filtros e ordenar informações em colunas de tabelas.

O sistema permite a realização de testes rápidos de rega para garantir o funcionamento correto. Os dados dos sensores são apresentados em tempo real através de gráficos, permitindo aos usuários monitorizar a rega e tomar decisões com base nas condições meteorológicas apresentadas. Os dados meteorológicos incluem temperatura, qualidade do ar, umidade, pressão atmosférica, índice de calor, precipitação, intensidade da luz e radiação ultravioleta. Além disso, é exibido para o usuário a capacidade do reservatório e o consumo de água em tempo real.

Estão disponíveis opções de configuração remota, que permitem aos usuários personalizar as unidades de temperatura, os cálculos da capacidade do reservatório de água, as notificações do sistema, os esquemas de cores e o menu da plataforma.

Os dados dos sensores podem ser exportados em formato CSV, oferecendo uma visão abrangente. Os relatórios fornecem dados numéricos para análise, eliminando a necessidade de aceder diretamente à base de dados ou à página do administrador. Os usuários podem selecionar os dados e períodos de tempo pretendidos, compartilhar facilmente os relatórios e arquivá-los para referência futura.

Para melhorar a experiência de usuário e a localização dos recursos, foi implementado um campo de pesquisa para elementos e funcionalidades específicas, reduzindo o tempo de pesquisa.

A plataforma foi desenvolvida utilizando o framework Django, proporcionando uma gestão eficiente das informações da base de dados através de uma página de administração integrada. Esta funcionalidade facilita a gestão de dados dos usuários, sensores, permissões, autenticações e configurações, garantindo uma plataforma organizada e acessível aos administradores.

Além disso, o sistema fornece uma API REST para realizar a comunicação com a plataforma e o sistema de irrigação. Esta interface permite a coleta de dados em tempo real, o envio de comandos para o sistema de irrigação e configuração da plataforma. A API é protegida por autenticação, concedendo acesso apenas a usuários autorizados. Os usuários podem obter acesso instantâneo a informações críticas, incluindo dados de sensores, calendários de rega, notificações, consumo de água e configurações da plataforma.

## Suporte

A plataforma é compatível e responsiva em dispositivos móveis, tablets e computadores, garantindo a sua adaptação a diferentes tamanhos de tela e sistemas operacionais.

## Materiais

Para montar o sistema de irrigação, são necessários os seguintes materiais:

Nome                       | Tipo 
-------------------------- | ---------------
FC-28                      | Sensor
YL-83                      | Sensor
DHT11                      | Sensor
TEMT6000                   | Sensor
HCSR-04                    | Sensor
YF-S201                    | Sensor 
Relay                      | Atuador
RTC DS3231                 | Módulo
Raspberry Pi Pico W        | Microcontrolador

### Conectando os componentes

Os GPIOs dos sensores e atuadores devem ser ligados ao Pico W da seguinte forma:

Pico W | DHT11 | HCSR-04 | YL-83 | Relay | YF-S201 | DS3231 | TEMT6000 | FC-28 
------ | ----- | ------- | ----- | ----- | ------- | ------ | -------- | ----- 
0      | x     |         |       |       |         |        |          |       
0      |       | x       |       |       |         |        |          |       
0      |       | x       |       |       |         |        |          |       
0      |       |         | x     |       |         |        |          |       
0      |       |         |       |       |         |        |          |      
0      |       |         |       |       |         |        | x        |       
0      |       |         |       |       |         | x      |          |       
0      |       |         |       |       |         | x      |          |       
0      |       |         |       |       |         |        |          | x    
0      |       |         |       |       |         |        |          |       
0      |       |         |       |       |         |        |          |       
0      |       |         |       |       |         |        |          |       
GND    | GND   | GND     | GND   | GND   | GND     | GND    | GND      | GND   
5V     | VCC   | VCC     | VCC   | VCC   | VCC     | VCC    | VCC      | VCC   
3V3    |       |         |       |       |         |        |          |       

### Exemplo do sistema

Após conectar todos os componentes, o sistema deve ser semelhante à imagem ilustrada abaixo.

## Executando a aplicação

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
foo@bar:~$ python manage.py migrate
~~~

Crie um usuário com permissões de administrador:

~~~console
foo@bar:~$ python manage.py createsuperuser
~~~

Por fim, execute o servidor da aplicação:

~~~console
foo@bar:~$ python manage.py runserver 0.0.0.0:80
~~~

Você poderá acessar a aplicação agora em seu computador em [```http://localhost```](http://localhost). Para acessar a plataforma em outro dispositivo, será necessário obter o endereço IP do computador que está executando esta aplicação.

## Autor

Desenvolvido por [Lucas Araújo](https://github.com/lucapwn).

## Licença

Esse software é licenciado pelo [MIT](https://choosealicense.com/licenses/mit/).
