
#include <DHTlib.h>

dht DHT;

void dht_command() {
    char* arg = sCmd.next();
    if(arg == NULL) {
        argument_error();
        return;
    }
    int dht_type = atoi(arg);
    arg = sCmd.next();
    if(arg == NULL) {
        argument_error();
        return;
    }
    int dht_pin = atoi(arg);

    int chk;
    // just the dht 11 sensor read function is different
    if (dht_type == 11) {
        chk = DHT.read11(dht_pin);
    } else {
        chk = DHT.read(dht_pin);
    }

    switch (chk) {
        case DHTLIB_OK:
            HomeClient.print("ACK ");
            HomeClient.print(DHT.temperature, 1);
            HomeClient.write(' ');
            HomeClient.print(DHT.humidity, 1);
            HomeClient.print("\r\n");
            break;
        case DHTLIB_ERROR_CHECKSUM:
            HomeClient.print("ERR checksum_error\r\n");
            break;
        case DHTLIB_ERROR_TIMEOUT:
            HomeClient.print("ERR timeout_error\r\n");
            break;
        default:
            HomeClient.print("ERR unknown_error\r\n");
            break;
    }
}
