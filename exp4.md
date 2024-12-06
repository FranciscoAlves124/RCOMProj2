# Experience 4 - Configure a Commercial Router and Implement NAT


# Steps
Tux94:
eth1 IP-> 172.16.90.254 / eth2 IP-> 172.16.91.253
eth1 MAC-> 00:c0:df:08:d5:99 / eth2 MAC-> 00:c0:df:25:40:81

Tux93:
eth1 IP-> 172.16.90.1
eth1 MAC-> 00:c0:df:08:d5:9a

Tux92:
eth1 IP-> 172.16.91.1
eth1 MAC-> 00:50:fc:59:be:3a

1. Ligar ether1 do RC ao P9.12.
2. Ligar ether2 do RC a uma porta da Switch (nosso caso porta 5).
3. Eliminar as portas default do ether5 do switch e ligar o ether5 à bridge91.
```
/interface bridge port remove [find interface=ether5]
/interface bridge port add bridge=bridge91 interface=ether5
```
4. Trocar o cabo ligado à consola do Switch para Router MT.
5. No Tux52, conectar ao router desde o GTKterm com:
```
Serial Port: /dev/ttyS0
Baudrate: 115200
Username: admin
Password: <ENTER>
```
6. Resetar as configurações do Router:
```
 /system reset-configuration
```
7. Configurar ip do Router pela consola do router no GTKterm do Tux92.
```
   /ip address add address=172.16.1.31/24 interface=ether1
   /ip address add address=172.16.91.254/24 interface=ether2
```
8.  Configurar as rotas default nos Tuxs e no Router:
```
   route add default gw 172.16.91.254 # Tux92
   route add default gw 172.16.90.254 # Tux93
   route add default gw 172.16.91.254 # Tux94

   /ip route add dst-address=172.16.90.0/24 gateway=172.16.91.253  # Router console
   /ip route add dst-address=0.0.0.0/0 gateway=172.16.1.254        # Router console
```
9.  No Tux93, começar a captura do Wireshark e fazer ping de todas as interfaces. Todas deverão funcionar. Guardar o resultado obtido:
```
ping 172.16.90.254 # Figura 1
ping 172.16.91.1   # Figura 2
ping 172.16.91.254 # Figura 3
``` 

10. Desativar o accept_redirects no Tux32:
```
$ sysctl net.ipv4.conf.eth1.accept_redirects=0 
$ sysctl net.ipv4.conf.all.accept_redirects=0
```
11. Remover a route que liga Tux32 a Tux34:
```
   route del -net 172.16.30.0 gw 172.16.31.253 netmask 255.255.255.0
```
12. No Tux32, iniciar o WireShark e fazer ping ao Tux33. A ligação é estabelecida, usando o RC como router em vez do Tux34:
```
ping 172.16.30.1 # Figura 4
```
13. Fazer traceroute para verificar o percurso da ligação:
```
traceroute -n 172.16.30.1
traceroute to 172.16.30.1 (172.16.30.1), 30 hops max, 60 byte packets
1  172.16.31.254  0.249 ms  0.249 ms  0.253 ms
2  172.16.31.25  0.546 ms  0.531 ms  0.518 ms
3  172.16.30.1 1.061 ms  1.051 ms  1.045 ms
```
14. Adicionar novamente a route que liga Tux32 a Tux34
```
route add -net 172.16.30.0/24 gw 172.16.31.253
```
15. 
```
traceroute -n 172.16.30.1
traceroute to 172.16.30.1 (172.16.30.1), 30 hops max, 60 byte packets
1  172.16.31.253  0.203 ms  0.167 ms  0.154 ms
2  172.16.30.1  0.321 ms  0.319 ms  0.311 ms
```

16. Reativar o accept_redirects no Tux32:
```
$ sysctl net.ipv4.conf.eth1.accept_redirects=0
$ sysctl net.ipv4.conf.all.accept_redirects=0
```
17. No Tux33, fazer ping ao FTP server (172.16.1.10) para verificar a ligação:
```
ping 172.16.1.10 # Figura 5
```
18. Desativar NAT do Router:
```
/ip firewall nat disable 0
```
19. No Tux33, fazer de novo ping do router do lab I.321 para verificar a ligação. Verifica-se que não há ligação:
```
ping 172.16.1.10 # Figura 6
```
20.  Reativar NAT do Router:
```
/ip firewall nat enable 0
```
