
asdrive_MAIN="../../../../"
dest="./examples/demo_asdrive/asdrive"


rsync -rv               					\
--exclude 'libev'       					\
--exclude 'Makefile'    					\
--exclude 'SocketToRelay/wolfssl' 				\
--exclude '.git'                                        	\
--exclude 'SocketToRelay/soc/esp8266/ESP8266_RTOS_SDK' 		\
--exclude 'SocketToRelay/soc/esp8266/esp-open-rtos' 		\
--exclude 'SocketToRelay/linkd/linkd*'             	 	\
--exclude 'esp-open-sdk' --prune-empty-dirs 			\
--exclude '*.c' 						\
--exclude 'CMakeFiles'						\
../../../../ ./examples/demo_asdrive/asdrive



cp $asdrive_MAIN/asdrive.c $dest
cp $asdrive_MAIN/asdJson.c $dest
cp $asdrive_MAIN/asdUART.c $dest
cp $asdrive_MAIN/asdGPIO.c $dest
cp $asdrive_MAIN/asdResponse.c $dest
cp $asdrive_MAIN/fsm/src/fsm.c $dest
cp $asdrive_MAIN/jsmn/jsmn.c $dest
cp $asdrive_MAIN/SocketToRelay/linkd/build_request.c $dest
cp $asdrive_MAIN/SocketToRelay/log_control/log_control.c $dest
cp $asdrive_MAIN/SocketToRelay/linkd/fetch_response.c $dest
cp $asdrive_MAIN/SocketToRelay/md5encrypt/hmac-md5.c $dest
cp $asdrive_MAIN/SocketToRelay/md5encrypt/base64.c $dest
cp $asdrive_MAIN/SocketToRelay/md5encrypt/md5c.c $dest
cp $asdrive_MAIN/SocketToRelay/string_utility/string_utility.c $dest
cp $asdrive_MAIN/SocketToRelay/aes_encrypt/aes_encrypt.c $dest
cp $asdrive_MAIN/SocketToRelay/soc/esp8266/syslog.c $dest
cp $asdrive_MAIN/SocketToRelay/config/config.c $dest

cd ./examples/demo_asdrive/
rm -rf .output
rm -rf eagle*
cp Makefile.asdrive ./asdrive/Makefile

export SDK_PATH=/home/hawk/asdrive/SocketToRelay/soc/esp8266/ESP8266_RTOS_SDK/
export BIN_PATH=/home/hawk/asdrive/SocketToRelay/soc/esp8266/ESP8266_RTOS_SDK/examples/demo_asdrive/

make clean;make BOOT=none APP=0 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=4
