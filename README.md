# HW-290-Arduino-Test

O magnetometro HMC8553 deixou de ser fabricado pela HoneyWell ha muitos anos , mas um clone "QMC8553"  eh comum aparecer nas placas de modulos de arduino hoje.
Esses dois chips sao identicos com relacao ao pinos, mas o mapa de registros eh diferente, entao requer alguma atencao na hora de programar um firmware que possa realmente utilizar o magnetometro do HW-290.

Alem disso , o Endereco I2C do HMC8553 eh 0X1E e o endereco do QMC8553 eh 0X0D.

O Codigo (hw-290test.ino) serve para testar os 3 sensores do HW-290 com arduino retornando os enderecos hexadecimal de cada um dos 3 sensores.
E importante salientar q alguma peparacao nas settings do MPU-6050 eh necessaria para poder se comunicar com o QMC8553 (ja presente no arquivo hw-290test.ino nas linas 39 a 58).

Abaixo a saida do Monitor Serial q vc deve ter.

![HW290_IMG1](https://user-images.githubusercontent.com/100097972/154868323-a436760e-c03e-4178-8887-708b25bb5c8b.JPG)


Abaixo a localizacao e os dispositivos principais do modulo

![HW290_IMG2](https://user-images.githubusercontent.com/100097972/154869127-06cf0ce4-8803-4cae-a18a-b4a71b3343b2.JPG)
