# Experience 1 - Configure an IP Network


# Steps

1. Conectar E1 do Tux23 e do Tux24 numa das entradas do switch;
2. Configurar o ip dos dois computadores;
```
$ ifconfig eth1 172.16.20.1/24 (para Tux23)
$ ifconfig eth1 172.16.20.254/24 (para Tux24)
```
3. Verificar MAC address de cada computador e consultar o campo  `ether`  no comando  `ifconfig`;
tux23 adress: 172.16.20.1 / macadress ether -> 00:50:fc:ee:0e:93
tux24 adress: 172.16.20.254 / macadress ether -< 00:08:54:50:3f:2c
4. Usar `ping` para verificar a conexão entre computadores
```
$ ping 172.16.50.254 -c 20 (para Tux23)
$ ping 172.16.50.1 -c 20 (para Tux24)
```
5. Avaliar tabela ARP do Tux23;
```
$ arp -a # ?(172.16.20.254) at 00:08:54:50:3f:2c [ether] on eth1
```
6. Apagar table ARP do Tux23;
```
$ arp -d 172.16.20.254/24
$ arp -a # empty
```
7.  Abrir o WireShark no Tux53 e começar a capturar pacotes de rede;
8. Usar `ping` no Tux23 para o Tux24 durante uns segundos;
```
$ ping 172.16.20.254/24 -c 10
```
9.  Parar de capturar pacotes de rede no WireShark;
10. Salvar o log do Wireshark obtido e analisar;
**print wireshark**
