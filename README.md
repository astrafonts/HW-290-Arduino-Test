# HW-290-Arduino-Test

O magnetometro HMC8553 deixou de ser fabricado pela HoneyWell ha muitos anos , mas um clone "QMC8553"  eh comum aparecer nas placas de modulos de arduino hoje.
Esses dois chips sao identicos com relacao ao pinos, mas o mapa de registros eh diferente, entao requer alguma atencao na hora de programar um firmware que possa realmente utilizar o magnetometro do HW-290. (documentacao a respeito dos registros e de operacao de cada sensor disponivel na pasta docs  https://github.com/astrafonts/HW-290-Arduino-Test/tree/main/docs)

Uma forma de saber qual chip veio na sua placa eh pelo Endereco I2C :  do HMC8553 eh 0X1E e o endereco do QMC8553 eh 0X0D. Entao ao rodar o program de teste hw-290test.ino, se aparecer o endereco "0X0D" vc ja sabe q eh o chip QMC8553 e nao o HMC8553.

outra maneira de saber qual chip vc tem eh olhar a descricao no chip mesmo : se tiver L883 eh o HMC5883, o QMC8553 geralmente aparece como DA5883 ou DB5883

O Codigo (hw-290test.ino) serve para testar os 3 sensores do HW-290 com arduino retornando os enderecos hexadecimal de cada um dos 3 sensores.
E importante salientar que o QMC8553 esta conectado aos Pinos do MPU6050 (xcl e xda) e nao direto aos pinos da placa SDL e SDA (veja schematic abaixo), portanto requer q a comunicacao com o magnetometro seja feita via MPU-6050, no arquivo hw-290test.ino e para os propositos do teste esse set up  esta sendo feito nas linhas 39 a 58.

NOTA : Boas referencias sobre os sensores *5883 nos seguintes LINKs : 
     
     - https://surtrtech.com/2018/02/01/interfacing-hmc8553l-qmc5883-digital-compass-with-arduino/
     
     - https://forum.arduino.cc/t/gy-87-clone-hw-290-addressing-issues/556702

Abaixo a saida do Monitor Serial q vc deve ter.

![HW290_IMG1](https://user-images.githubusercontent.com/100097972/154868323-a436760e-c03e-4178-8887-708b25bb5c8b.JPG)


Abaixo a localizacao e os dispositivos principais do modulo

![HW290_IMG2](https://user-images.githubusercontent.com/100097972/154869127-06cf0ce4-8803-4cae-a18a-b4a71b3343b2.JPG)

Um schematic do GY-87/HW-290 esta abaixo :

![01](https://user-images.githubusercontent.com/100097972/154870822-f59a06d7-6a9e-4eaf-a94f-c436df253556.jpg)
