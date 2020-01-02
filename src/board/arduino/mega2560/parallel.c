// High resolution pinout can be found here:
// https://osoyoo.com/wp-content/uploads/2017/08/arduino_mega_2560_pinout.png

#include <stdint.h>
#include <stdio.h>
#include <board/cpu.h>
#include <util/delay.h>

#include <arch/gpio.h>

// Mapping of 24-pin ribbon cable to parallel pins. See schematic
#define PINS \
    /* Data (KSO0 - KSO7) - bi-directional */ \
    PIN(d0, 1) \
    PIN(d1, 2) \
    PIN(d2, 3) \
    PIN(d3, 7) \
    PIN(d4, 9) \
    PIN(d5, 10) \
    PIN(d6, 13) \
    PIN(d7, 16) \
    /* Wait# (KSO9) - input */ \
    /*  low to indicate cycle may begin, high to indicate cycle may end */ \
    PIN(wait_n, 18) \
    /* Write# (KSI0) - output */ \
    /*  low to indicate write cycle, high to indicate read cycle */ \
    PIN(write_n, 4) \
    /* DataStrobe# (KSI1) - output */ \
    /*  low indicates a data cycle */ \
    PIN(data_n, 5) \
    /* Reset# (KSI2) - output */ \
    /*  low requests device reset */ \
    PIN(reset_n, 6) \
    /* AddressStrobe# (KSI3) - output */ \
    /*  low indicates an address cycle */ \
    PIN(addr_n, 8) \
    /* Strap0 (KSI4) */ \
    /*  1K-Ohm pull-down resistor */ \
    PIN(strap_0, 11) \
    /* Strap1 (KSI5) */ \
    /*  1K-Ohm pull-down resistor */ \
    PIN(strap_1, 12)

#define DATA_BITS \
    DATA_BIT(0) \
    DATA_BIT(1) \
    DATA_BIT(2) \
    DATA_BIT(3) \
    DATA_BIT(4) \
    DATA_BIT(5) \
    DATA_BIT(6) \
    DATA_BIT(7)

// Flip odd and even pins
//#define FLIP

#ifdef FLIP

// Mapping of 24-pin ribbon cable to GPIOs (flipped)
static struct Gpio GPIOS[24] = {
    GPIO(B, 0), GPIO(B, 1), 
    GPIO(B, 2), GPIO(B, 3), 
    GPIO(L, 0), GPIO(L, 1), 
    GPIO(L, 2), GPIO(L, 3), 
    GPIO(L, 4), GPIO(L, 5), 
    GPIO(L, 6), GPIO(L, 7), 
    GPIO(G, 0), GPIO(G, 1), 
    GPIO(G, 2), GPIO(D, 7), 
    GPIO(C, 0), GPIO(C, 1), 
    GPIO(C, 2), GPIO(C, 3), 
    GPIO(C, 4), GPIO(C, 5), 
    GPIO(C, 6), GPIO(C, 7)
};

#else // FLIP

// Mapping of 24-pin ribbon cable to GPIOs
static struct Gpio GPIOS[24] = {
    GPIO(B, 1), GPIO(B, 0),
    GPIO(B, 3), GPIO(B, 2),
    GPIO(L, 1), GPIO(L, 0),
    GPIO(L, 3), GPIO(L, 2),
    GPIO(L, 5), GPIO(L, 4),
    GPIO(L, 7), GPIO(L, 6),
    GPIO(G, 1), GPIO(G, 0),
    GPIO(D, 7), GPIO(G, 2),
    GPIO(C, 1), GPIO(C, 0),
    GPIO(C, 3), GPIO(C, 2),
    GPIO(C, 5), GPIO(C, 4),
    GPIO(C, 7), GPIO(C, 6)
};


#endif // FLIP

// Parallel struct definition
// See http://efplus.com/techref/io/parallel/1284/eppmode.htm
struct Parallel {
    #define PIN(N, P) struct Gpio * N;
    PINS
    #undef PIN
};

// Parallel struct instance
static struct Parallel PORT = {
    #define PIN(N, P) .N = &GPIOS[P - 1],
    PINS
    #undef PIN
};

// Set port to all high-impedance inputs
void parallel_hiz(struct Parallel * port) {
    #define PIN(N, P) \
        gpio_set_dir(port->N, false); \
        gpio_set(port->N, false);
    PINS
    #undef PIN
}

// Set port to initial state required before being able to perform cycles
void parallel_reset(struct Parallel * port) {
    parallel_hiz(port);

    // Set reset line low
    gpio_set_dir(port->reset_n, true);

    // Wait 5 microseconds
    _delay_us(5);
    
    // Make sure strobes are high outputs
    gpio_set(port->data_n, true);
    gpio_set(port->addr_n, true);
    gpio_set_dir(port->data_n, true);
    gpio_set_dir(port->addr_n, true);

    // Set write line high output
    gpio_set(port->write_n, true);
    gpio_set_dir(port->write_n, true);

    // Pull up wait line
    gpio_set(port->wait_n, true);

    //TODO: something with straps
    
    // Wait 5 microseconds
    _delay_us(5);

    // Set reset line high, ending reset
    gpio_set(port->reset_n, true);
}

//TODO: timeout
int parallel_transaction(struct Parallel * port, uint8_t * data, int length, bool read, bool addr) {
    int i;
    for (i = 0; i < length; i++) {
        // Wait for wait line to be low
        while (gpio_get(port->wait_n)) {}

        // Set write line
        gpio_set(port->write_n, read);
        
        if (!read) {
            // Set data
            uint8_t byte = data[i];
            #define DATA_BIT(B) \
                gpio_set(port->d ## B, (byte & (1 << B)) > 0); \
                gpio_set_dir(port->d ## B, true);
            DATA_BITS
            #undef DATA_BIT
        }

        // Wait 5 microseconds
        _delay_us(5);

        if (addr) {
            // Set address strobe low
            gpio_set(port->addr_n, false);
        } else {
            // Set data strobe low
            gpio_set(port->data_n, false);
        }

        // Wait for wait line to be high
        while (!gpio_get(port->wait_n)) {}

        if (read) {
            uint8_t byte = 0;
            #define DATA_BIT(B) \
                if (gpio_get(port->d ## B)) byte |= (1 << B);
            DATA_BITS
            #undef DATA_BIT
            data[i] = byte;
        }

        if (addr) {
            // Set address strobe high
            gpio_set(port->addr_n, true);
        } else {
            // Set data strobe high
            gpio_set(port->data_n, true);
        }

        // Wait 5 microseconds
        _delay_us(5);

        // Reset data lines to high impedance inputs
        #define DATA_BIT(B) \
            gpio_set_dir(port->d ## B, false); \
            gpio_set(port->d ## B, false);
        DATA_BITS
        #undef DATA_BIT

        // Set write line high
        gpio_set(port->data_n, true);
    }

    return i;
}

void parallel_host(void) {
    printf("Parallel host. Reset to exit\n");

    struct Parallel * port = &PORT;
    parallel_reset(port);

    for (;;) {
        int c = getchar();
        if (c < 0) break;

        putchar(c);
        if (c == '\r') {
            putchar('\n');
        }

        uint8_t byte = (uint8_t)c;
        parallel_transaction(port, &byte, 1, false, false);
    }

    parallel_hiz(port);
}

void parallel_peripheral(void) {
    printf("Parallel peripheral (WIP, writes only). Reset to exit\n");

    struct Parallel * port = &PORT;
    parallel_hiz(port);

    // Set wait line to low output
    gpio_set_dir(port->wait_n, true);

    // Strobes are high when inactive
    bool last_data_n = true;
    bool last_addr_n = true;

    for (;;) {
        // Read data strobe and edge detect
        bool data_n = gpio_get(port->data_n);
        bool data_edge = last_data_n && !data_n;

        // Read address strobe and edge detect
        bool addr_n = gpio_get(port->addr_n);
        bool addr_edge = last_addr_n && !addr_n;

        // If not in reset
        if (gpio_get(port->reset_n)) {
            // On the falling edge of either strobe
            if (data_edge || addr_edge) {
                // Check if read or write cycle
                bool read = gpio_get(port->write_n);
        
                // Read data
                uint8_t byte = 0;
                #define DATA_BIT(B) \
                    if (gpio_get(port->d ## B)) byte |= (1 << B);
                DATA_BITS
                #undef DATA_BIT

                //TODO: Check if strobe fell while reading

                // Set wait line high
                gpio_set(port->wait_n, true);

                if (data_edge) {
                    putchar('d');
                }

                if (addr_edge) {
                    putchar('a');
                }

                if (read) {
                    putchar('<');
                } else {
                    putchar('>');
                }

                printf("%02X\n", byte);

                // Delay 5 usecs
                _delay_us(5);

                // Set wait line low
                gpio_set(port->wait_n, false);
            }
        }

        last_data_n = data_n;
        last_addr_n = addr_n;
    }
}

void parallel_spy(void) {
    printf("Parallel spy. Reset to exit\n");

    struct Parallel * port = &PORT;
    parallel_hiz(port);

    // Strobes are high when inactive
    bool last_data_n = true;
    bool last_addr_n = true;

    for (;;) {
        // Read data strobe and edge detect
        bool data_n = gpio_get(port->data_n);
        bool data_edge = last_data_n && !data_n;

        // Read address strobe and edge detect
        bool addr_n = gpio_get(port->addr_n);
        bool addr_edge = last_addr_n && !addr_n;

        // If not in reset
        if (gpio_get(port->reset_n)) {
            // On the falling edge of either strobe
            if (data_edge || addr_edge) {
                // Check if read or write cycle
                bool read = gpio_get(port->write_n);
        
                // Read data
                uint8_t byte = 0;
                #define DATA_BIT(B) \
                    if (gpio_get(port->d ## B)) byte |= (1 << B);
                DATA_BITS
                #undef DATA_BIT

                //TODO: Check if strobe fell while reading

                if (data_edge) {
                    putchar('d');
                }

                if (addr_edge) {
                    putchar('a');
                }

                if (read) {
                    putchar('<');
                } else {
                    putchar('>');
                }

                printf("%02X\n", byte);
            }
        }

        last_data_n = data_n;
        last_addr_n = addr_n;
    }
}

