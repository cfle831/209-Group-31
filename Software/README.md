

TODO:
 - [x] Confirm Adc functionality on testing board
 - [] Convert Display functions to run entirely off timer interrupts, displaying one LED per interrupt
    - Function to change cur_display_number number in display.c
    - make function above set_seg that selects LED ad uses cur_display_number
 - [] Display power
 - [] Transmit power


Notes:
 - Current display function (flagged from interrupt) takes ~5ms via _delay function (0.5ms bursts) which could cause issues for other interrupts