import asyncio
from time import sleep

from bleak import BleakClient



# uuid's at peripheral
read_characteristic = "00002A3D-0000-1000-8000-00805f9b34fb"
write_characteristic = "00002A58-0000-1000-8000-00805f9b34fb"



address = "67:de:20:60:f4:66"#input("Device Address (e.g. 12:34:56:78:9A:BC): ")
READ_UUID = write_characteristic
WRITE_UUID = read_characteristic
async def run(address):
    async with BleakClient(address) as client:
        await client.write_gatt_char(WRITE_UUID, b"Foo")
        sleep(500)
        
        val = (await client.read_gatt_char(READ_UUID)).decode("UTF-8")
        print("Value: ", val)



loop = asyncio.get_event_loop()
loop.run_until_complete(run(address))