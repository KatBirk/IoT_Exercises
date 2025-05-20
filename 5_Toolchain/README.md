# Toolchain

## Quest 3
This exercise sets up UART. The logic for UART is used both for this exercise and exercises 5 and 6. 

- ´const uart_port_t uart_num = UART_NUM_0;´ -> This line defines which UART port is being used
- ´uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // Disable flow control
      .rx_flow_ctrl_thresh = 122,
  };´ -> This configures UART params


## Quest 5
For this exercise, we create a method for printing the contents of a char*[]. To get the size of the array (i.e. the number of elements in the array), we use **sizeof(arr)/sizeof(char*)** because it divides the total memory taken by the whole array by the memory taken by one element (which is a pointer). 

UART is used to send the messages to the esp32
Problems encountered during the exercise includes receiving an empty response when monitoring UART. 
