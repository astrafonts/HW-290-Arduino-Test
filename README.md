# HW-290-Arduino-Test

O magnetometro HMC8553 deixou de ser fabricado pela HoneyWell ha muitos anos , mas um clone "QMC8553"  eh comum aparecer nas placas de modulos de arduino hoje.
Esses dois chips sao identicos com relacao ao pinos, mas o mapa de registros eh diferente, entao requer alguma atencao na hora de programar um firmware que possa realmente utilizar o magnetometro do HW-290.

Alem disso , o Endereco I2C do HMC8553 eh 0X1E e o endereco do QMC8553 eh 0X0D.

O Codigo (hw-290test.ino) serve para testar os 3 sensores do HW-290 com arduino retornando os enderecos hexadecimal de cada um dos 3 sensores.
E importante salientar que o QMC8553 esta conectado aos Pinos do MPU6050 (xcl e xda) e nao nos pinos da placa SDL e SDA (veja schematico abaixo), portanto requer q a comunicacao com o magnetometro seja feita via MPU-6050. no arquivo hw-290test.ino esse set up  esta sendo feito nas linhas 39 a 58.

Abaixo a saida do Monitor Serial q vc deve ter.

![HW290_IMG1](https://user-images.githubusercontent.com/100097972/154868323-a436760e-c03e-4178-8887-708b25bb5c8b.JPG)


Abaixo a localizacao e os dispositivos principais do modulo

![HW290_IMG2](https://user-images.githubusercontent.com/100097972/154869127-06cf0ce4-8803-4cae-a18a-b4a71b3343b2.JPG)

Um schematic do GY-87/HW-290 esta abaixo :

![01](https://user-images.githubusercontent.com/100097972/154870822-f59a06d7-6a9e-4eaf-a94f-c436df253556.jpg)
