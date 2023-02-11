//------------------------------------------------------------------------------
// Headers that are required for printf and mmap
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

//------------------------------------------------------------------------------
// GPIO ACCES
//------------------------------------------------------------------------------

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

#define GPIO_LED0   4//Numero du port LED0
#define GPIO_LED1   17//Numero du port LED1
#define GPIO_BP     18//Numero du port button

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
};

struct gpio_s *gpio_regs_virt; 


static void 
gpio_fsel(uint32_t pin, uint32_t fun)//Fonction pour choisir fsel
{
    uint32_t reg = pin / 10;//Chaque ensemble de sel a 10 pin, diviser par 10 pour trouver les ensembles
    uint32_t bit = (pin % 10) * 3;//Chaque case a 3 bits, pour trouver les bits initialisation
    uint32_t mask = 0b111 << bit;//Intialiser un mask pour changer la valeur
    gpio_regs_virt->gpfsel[reg] = (gpio_regs_virt->gpfsel[reg] & ~mask) | ((fun << bit) & mask);//Modifier les bits pour charger GPIO corresponds
}

static void 
gpio_write (uint32_t pin, uint32_t val)
{
    uint32_t reg = pin / 32;//Chaque ensemble de gpio_write a 32 bits, diviser par 32 pour choisir le bon ensemble
    uint32_t bit = pin % 32;//Choisir le bit d'initialise
    if (val == 1) 
        gpio_regs_virt->gpset[reg] = (1 << bit);//Mettre le valeur a 1
    else
        gpio_regs_virt->gpclr[reg] = (1 << bit);//Mettre le valeur a 0
}

//------------------------------------------------------------------------------
// Access to memory-mapped I/O
//------------------------------------------------------------------------------

#define RPI_PAGE_SIZE           4096
#define RPI_BLOCK_SIZE          4096

static int mmap_fd;

static int
gpio_mmap ( void ** ptr )
{
    void * mmap_result;

    mmap_fd = open ( "/dev/mem", O_RDWR | O_SYNC );

    if ( mmap_fd < 0 ) {
        return -1;
    }

    mmap_result = mmap (
        NULL
      , RPI_BLOCK_SIZE
      , PROT_READ | PROT_WRITE
      , MAP_SHARED
      , mmap_fd
      , BCM2835_GPIO_BASE );

    if ( mmap_result == MAP_FAILED ) {
        close ( mmap_fd );
        return -1;
    }

    *ptr = mmap_result;

    return 0;
}

void
gpio_munmap ( void * ptr )
{
    munmap ( ptr, RPI_BLOCK_SIZE );
}

//------------------------------------------------------------------------------
// Main Programm
//------------------------------------------------------------------------------

void
delay ( unsigned int milisec )//Fonction delay pour realier le fonctionnement d'attente
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

void* clig0(void* freq)//Fonction pour le led0
{

    unsigned freq_un = *((unsigned int*)freq);//Pour pthread, il faut changer les parametres a forme void*, il faut translater


    // Blink led at frequency of 1Hz
    // ---------------------------------------------

    uint32_t val = 0;//La valeur a ecrire dans led0


    while (1) {
        gpio_write ( GPIO_LED0, val );
        delay ( freq_un );//Attends dans un temps
        val = 1 - val;//mettre la valeur inverse
    }
}

void* clig1(void* freq)
{

    unsigned freq_un = *((unsigned int*)freq);
    // Blink led at frequency of 1Hz
    // ---------------------------------------------

    uint32_t val = 0;//La valeur a ecrire dans led1

    while (1) {
        gpio_write ( GPIO_LED1, val );
        delay ( freq_un );
        val = 1 - val;
    }
}

int
main ( int argc, char **argv )
{
    // Get args
    // ---------------------------------------------

    int period, half_period;

    period = 1000; /* default = 1Hz */
    if ( argc > 1 ) {
        period = atoi ( argv[1] );
    }
    half_period = period / 2;
    uint32_t volatile * gpio_base = 0;

    // map GPIO registers
    // ---------------------------------------------

    if ( gpio_mmap ( (void **)&gpio_regs_virt ) < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    // Setup GPIO of LED0 to output
    // ---------------------------------------------
    
    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);//Initisalier fsel pour led0

    gpio_fsel(GPIO_LED1, GPIO_FSEL_OUTPUT);//Initisalier fsel pour led1

    // Blink led at frequency of 1Hz
    // ---------------------------------------------

    uint32_t val = 0;

    printf ( "-- info: start blinking.\n" );

    /*
    pid_t pid = fork();
    if(pid == 0)//Utiliser fork pour faire les deux process dans un meme temps
    {
        while (1) {
            gpio_write ( GPIO_LED0, val );
            delay ( half_period );
            val = 1 - val;
        }
    }
    else
    {
        while (1) {
            gpio_write ( GPIO_LED1, val );
            delay ( period );
            val = 1 - val;
        }
    }
    */
    pthread_t t1,t2;//Initialier pthread
    pthread_create(&t1,NULL,clig0,&half_period);//Creer pthread pour clig0 en frequence half_period
    pthread_create(&t2,NULL,clig1,&period);//Creer pthread pour clig1 en frequence period

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);//Securiser les deux thread
    while(1);

   

    return 0;
}
