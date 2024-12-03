# Experience 3 - Configure a Router in Linux


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

1. Transformar o Tux94 (Linux) num router:
- Configurar também eth2 interface do Tux94 e adicionar à bridge91:
```
$ ifconfig eth2 172.16.91.253/24
```
No GtkTerm adicionar à bridge91:
```
> /interface bridge port add bridge=bridge21 interface=ether1
```
Neste caso eth2 de Tux24 está ligado à porta1/ether1

2. Ativar ip forwarding e desativar ICMP echo-ignore-broadcast.
```
 Ip forwarding Tux94
 $ echo 1 > /proc/sys/net/ipv4/ip_forward

 Disable ICMP echo ignore broadcast Tux94
 $ echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
```
3. Reconfigurar Tux92 e Tux93 para que possam comunicar um com o outro.
```
 route add -net  172.16.90.0/24 gw 172.16.91.253 # no Tux52
 route add -net  172.16.91.0/24 gw 172.16.90.254 # no Tux53
```
4. Verificar as rotas em todos os Tux com o comando:
```
route -n
```
5. Começar a capturar eth1 no Tux93 com Wireshark.
6. Do Tux93 fazer ping para as outras networks.
```
ping 172.16.90.254
ping 172.16.91.253
ping 172.16.91.1
##Network is correct
```
7. Parar as capturas e salvar os logs.
8. Começar a capturar eth1 e eth2 no Tux94 (usar 2 instâncias de wireshark, uma para cada network).
9. Limpar ARP tables nos 3 Tux.
```
arp -d 172.16.90.1 #Tux54
arp -d 172.16.91.1 #Tux54
arp -d 172.16.90.254 #Tux53
arp -d 172.16.91.253 #Tux52
```
10. No Tux93, fazer ping para Tux92.
11. Parar captura no Tux94 e salvar logs.
