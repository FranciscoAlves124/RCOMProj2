# Experience 2 - Implement two bridges in a switch


# Steps

1. Conectar e configurar E1 do Tux72 e registar o IP e MAC Address;
```
$ ifconfig eth1 172.16.71.1/24
Tux72 ip: 172.16.71.1 / MAC Address: 00:e0:7d:b5:8c:8f
```
2. Abrir GtkTerm no Tux72 e configurar a baudrate para 115200;

3. Utilizar o comando `/system reset-configuration` para resetar as configurações do switch;
4. Criar duas bridges no switch: bridge70 e bridge71
```
> /interface bridge add name=bridge70
> /interface bridge add name=bridge71
```
5. Eliminar as portas as quais o Tux73, 74 e 72 estão ligados por defeito
tux72 -> porta2
tux73 -> porta3
tux74 -> porta4
```
> /interface bridge port remove [find interface=ether2] 
> /interface bridge port remove [find interface=ether3] 
> /interface bridge port remove [find interface=ether4]
```
6. Adicionar as portas correspondentes;
```
> /interface bridge port add bridge=bridge70 interface=ether3
> /interface bridge port add bridge=bridge70 interface=ether4
> /interface bridge port add bridge=bridge71 interface=ether2
```
7. Confirmar que as portas foram adicionadas corretamente com o comando:
```
> /interface bridge port print
```
8. Configurar Tux73 e Tux74 (ou verificar que a configuração já está feita);
	Tux73 
		ip: 172.16.70.1/24
	    MAC : 00:01:02:9f:81:2e
	Tux74:
		ip: 172.16.70.254/24
		MAC: 00:c0:df:02:55:95
9. Começar a captura do eth1 no Tux73;
10. Desde o Tux73, ``ping`` Tux74 e depois ping Tux72;
```
$ ping 172.16.70.254
#Tux74 -> Tudo ok
$ ping 172.16.71.1
#Tux72 -> connect: Network is unreachable
```
Como  Tux73 está na mesma bridge que o Tux74 e numa bridge diferente que o Tux72, obtem-se este resultado.

11. Começar capturas de eth1 no Tux72, Tux73 e Tux74;
12. Do Tux73 fazer ``ping -b 172.16.70.255`` por uns segundos;
13. Capturar e observar os resultados obtidos;

15. Começar novamente capturas de eth1 no Tux72, Tux73 e Tux74;
16. Do Tux72 fazer ``ping -b 172.16.71.255`` por uns segundos;
17. Capturar e observar os resultados obtidos;

