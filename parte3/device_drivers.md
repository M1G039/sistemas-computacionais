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

Com este comando podemos ver a informação de um driver específico:
```
sudo lsmod | grep <driver_name>
```

!TODO: Explicar os ficheiros bash

### Estrutura do código
Os módulos contém duas funções cruciais: `init()` (função de inicialização) e `exit()` (função de limpeza). Estas funções são executadas quando o módulo é carregado e descarregado respetivamente. Estas funções terão depois que ser atribuidas à respetiva funcionalidade através do `module_init()`e `module_exit()`.
Exemplo:
```C
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("I'd rather not say");
MODULE_DESCRIPTION("My first LKM");
MODULE_VERSION("0:0.1");

static int pipe_init(void){
    // Init code goes here
    printk(KERN_INFO, "Hello");
    return 0;
}

static int pipe_exit(void){
    // Exit code goes here
    printk(KERN_INFO, "Module unloaded, bye.");
}

// Assign functionality
module_init(pipe_init);
module_exit(pipe_exit);
```
Este é um exemplo de um driver simples que implementa a função `init()` e `exit()`. è importante realçar que, ao contrário de outros programas que terão escrito em C anterioirmente, um LKM não inclui as bibliotecas `stdio.h` e `stdlib.h` nem contém uma função `main()`. Isto deve se ao facto de que, ao contrário desses programas que correm no `user space`, os LKM's correm no `kernel space`, que são regiões de memória separadas, daí apenas podermos chamar funções que pertençam ao API do kernel, como o `printk()`, que imprime texto no log do kernel.
Para compilar estes programas é adotado um processo diferente e para tal podemos usar o Makefile disponível no Moodle:

```makefile
# To build modules outside of the kernel tree, we run "make"
# in the kernel source tree; the Makefile these then includes this
# Makefile once again.
# This conditional selects whether we are being included from the
# kernel Makefile or not.
ifeq ($(KERNELRELEASE),)

    # Assume the source tree is where the running kernel was built
    # You should set KERNELDIR in the environment if it's elsewhere
    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    # The current directory is passed to sub-makes as argument
    PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod *.mod.c .tmp_versions Module.symvers modules.order

.PHONY: modules modules_install clean

else
    # called from kernel build system: just declare what our modules are
    obj-m := $(module).o
endif
```

### Tipos de device drivers

Existem 3 tipos de device drivers:
- Charcter drivers;
- Block drivers;
- Networks drivers.

Aqui apenas abordamos os character drivers ou character devices. De qualquer das formas, quer os char devices que os block devices são como ficheiros no filesystem do Linux. Por convenção, quando carregados, estão localizados no diretório `/dev`. Isto significa que, como podem ser acedidos como ficheiros por norma implementam pelo menos alguma das system calls `open()`, `close()`, `read()`, `write()`, entre outras.
Além disto, cada device tem associado um *major* e um *minor* number.
- O *major* identifica o driver que controla o dispositivo em questão.
- O *minor* apenas é utilizado pelo próprio device para se identificar em relação a outros que possam utilizar o mesmo driver.

### Char devices
Ao fazer um LKM para um device driver podemos escolher um `major`e `minor`predefinidos (static assignment) ou deixar que o kernel decida (dynamic assignment). De modo a obter estes números o device driver pode invocar a função `alloc_chrdev_reregion()` que pertence à biblioteca `<linux/fs.h>`. Na saída os LKM devem libertar os device numbers que estavam a utilizar com `unregister_chrdev_region()`.

Sendo assim, para registar um character device primeiro temos que inicializar uma estrutura `struct cdev` que representa o device driver e as suas funcionalidades. o `cdev`tem dois membros:
- `owner`: aponta para o LKM a que o device pertence (definido pelo macro `THIS_MODULE`);
- `ops`: aponta para uma estrutura do tipo `file_operations` que define que system calls serão suportadas pelo device e que intruções executar na ocorrência de cada uma.

```C
static dev_t mydev;

// Allocate memory for the cdev structure
static struct cdev *mycdev = cdev_alloc();
static struct file_operations myfops = {
    .owner = THIS_MODULE,
    .read = echo_read,
    .write = echo_write,
    .open = echo_open,
    .release = echo_release
};

// Initialize cdev structure
mycdev->owner = THIS_MODULE;
mycdev->ops = &myfops;

// Tell the kernel about our device
cdev_add(mycdev, mydev, 1);
```

#### Registo e desregisto 
Através da secção anterior podemos então proceder ao registo e desregisto dos devices nas funções `init()`e `exit()`respetivamente.
Exemplo (hello_device):
```C
#include <linux/init.h>    // For module initialization and cleanup functions
#include <linux/module.h>  // For module initialization and cleanup functions
#include <linux/types.h>   // For dev_t, MAJOR, MINOR macros
#include <linux/fs.h>      // For file operations, file structure, etc.
#include <linux/cdev.h>    // For character device operations
#include <linux/uaccess.h> // For copy_to_user, copy_from_user
#include <linux/slab.h>    // For kmalloc, kfree
#include <linux/kernel.h>  // For printk, KERN_INFO, etc.

MODULE_LICENSE("Dual BSD/GPL"); 
MODULE_AUTHOR("SC SC24/25"); 
MODULE_DESCRIPTION("Test driver"); 
MODULE_VERSION("0.0.1"); 

// Declare module related functions
static int hello_open(struct inode *inode, struct file *file);    // Function to open the hello device
static int hello_release(struct inode *inode, struct file *file); // Function to release the hello device

ssize_t hello_read(struct file *filep, char __user *buff, size_t count, loff_t *offp); // Function to read from the hello device
ssize_t hello_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp); // Function to write to the hello device

static int hello_init(void); // Function to initialize the hello device
static void hello_exit(void); // Function to clean up the hello device

// Initialize device structures
static dev_t mydev;
static struct cdev* mycdev;
static struct file_operations myfops = {
    .owner = THIS_MODULE;
    .read = hello_read,
    .write = hello_write,
    .open = hello_open,
    .release = hello_release
};

static int hello_init(void){

    int major;
    int result = -1;

    // Ask the kernel for major and minor numbers
    result = alloc_chrdev_region(&mydev, 0, 1, "hello_device");
    if (result < 0){
        printk(KERN_INFO, "Failed to register device driver.");
        return result; // Allocation failed, return the error code.
    } 

    // Register the device to the kernel
    mycdev = cdev_alloc();
    if (mycdev == NULL){
        // Unregister device before returning;
		unregister_chrdev_region(mydev, 1);
		printk(KERN_ERR "Failed to allocate cdev structure\n");
		return -1;
	}

    // Initialize cdev structure.
    mycdev->owner = THIS_MODULE;
	mycdev->ops = &myfops;

    // Tell the kernel about our device.
    result = cdev_add(mycdev, mydev, 1);
    if (result < 0){
        // Free all memory associated with the device.
        kfree(mycdev);
        unregister_chrdev_region(mydev, 1);

        printk(KERN_ERR "Failed to register character device info\n");
        return return_of_cdev_add;
    }

    major = MAJOR(mydev);

    printk(KERN_INFO "Hello device registered with major number %d\n", major); // Print a message indicating the device was registered
    return 0;
}

static void hello_exit(){

    // Deregister character device
	if (mycdev){
		cdev_del(my_cdev);   // Unregister from kernel first
		kfree(my_cdev);      // Then free the memory
	}

    // Get the major number
    int major = MAJOR(mydev);

    // Deregister device numbers
    unregister_chrdev_region(mydev, 1); 

    printk(KERN_INFO "Hello device with major number %d unregistered\n", major);

    // NO RETURN

}

// Assign init and exit functions
module_init(hello_init);
module_exit(hello_exit);
```
Apenas falta implementar as system calls relacionadas com as operações de ficheiros.

