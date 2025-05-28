# Device drivers - review

Utilizamos LKM - Loadable Kernel Modules para acrescentar funcionalidades ao kernel do linux e evitar editar o seu código.
Para compilar estes LKMs usamos um makefile específico. Este código corre no `kernel space` que está separado da região de onde correm os programas normalmente, o `user space`.

Utilizando o exercício pipe como exemplo podemos correr:
```
make module=pipe
```
Assim obtemos vários ficheiros de output, um deles terá a extensão `.ko`. Este será o ficheiro que será carregado no kernel. Podemos fazer isso com:
```
sudo insmod pipe.ko
```
Para verificar se esta operação ocorreu sem erros podemos ver o log do kernel com
```
sudo dmesg -w
```
A flag de `-w` é opcional e apenas faz com que o log seja mostrado em tempo real.

Para verificar que modules estão carregados no kernel podemos executar:
```
cat /proc/devices
```

No moodle podemos encontrar os ficheiros bash `load_driver.sh` e `unload_driver.sh` para facilitar carregar e descarregar modulos do kernel em vez de utilizar o `insmod`.

sudo lsmod | grep pipe

!TODO: Explicar os ficheiros bash

### Tipos de device drivers


### Estrutura do código
Os módulos contém duas funções cruciais: `init()` e `exit()`. Estas funções são executadas quando o módulo é carregado e descarregado respetivamente. Estas funções terão depois que ser atribuidas à respetiva funcionalidade através do `module_init()`e `module_exit()`.
Exemplo:
```C
static int pipe_init(void){
    // Init code goes here
}

static int pipe_exit(void){
    // Exit code goes here
}

// Assign functionality
module_init(pipe_init);
module_exit(pipe_exit);
```


