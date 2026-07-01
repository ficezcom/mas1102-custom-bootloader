ENTRY(_start)

PHDRS
{
    vectors PT_LOAD FLAGS(5); /* RX */
    text    PT_LOAD FLAGS(5); /* RX */
    data    PT_LOAD FLAGS(6); /* RW */
}

SECTIONS
{
    . = 0x0000;

    .vectors :
    {
        KEEP(*(.vectors))   
    } :vectors


   # For after serial payload, Safe region
    . = 0x32A00;   

    .text :
    {
        . = ALIGN(4);      
        *(.text*)          
        . = ALIGN(4);      
        *(.rodata*)        
        . = ALIGN(4);
    } :text
    
    .data :
    {
        *(.data)
    } :text

    .bss : {
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    } :data
}

