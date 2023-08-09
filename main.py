from quart import Quart
from kasa import SmartBulb, Discover
import datetime
import asyncio
import socket

app = Quart(__name__)

bulb = None
timeout_task = None

TIMEOUT_SECONDS = 300
schedule_active_start_time = datetime.time(4, 30, 0) # 4:30 PM (Hours, Minutes, Seconds)
schedule_active_end_time = datetime.time(23, 30, 0) # 11:30 PM (Hours, Minutes, Seconds)


@app.before_serving
async def blink(): #! rename to setup, have another blink function for the actual blink
    global bulb
    global timeout_task

    bulb = await get_bulb("192.168.1.212") # Connect to the bulb

    # Blink the bulb to indicate that the server is running
    await bulb.turn_on()
    await asyncio.sleep(0.5)
    await bulb.turn_off()
    await asyncio.sleep(0.5)
    await bulb.turn_on()

    timeout_task = asyncio.create_task(turn_off_after_timeout())


@app.route('/motion_detected', methods=['POST'])
async def motion_detected():
    global timeout_task

    if timeout_task is not None:
        timeout_task.cancel()

    print("Motion detected!")
    if bulb is not None and within_set_schedule():
        await bulb.turn_on()
        timeout_task = asyncio.create_task(turn_off_after_timeout())
    return 'OK', 200


@app.route("/turn_on", methods=['POST'])
async def turn_on_bulb():
    await bulb.turn_on()
    return "OK", 200


@app.route("/turn_off", methods=['POST'])
async def turn_off_bulb():
    await bulb.turn_off()
    return "OK", 200


async def turn_off_after_timeout(timeout = TIMEOUT_SECONDS):
    await asyncio.sleep(timeout)
    if bulb is not None:
        await bulb.turn_off()


async def get_bulb(ip):
    bulb = SmartBulb(ip)
    await bulb.update()
    return bulb


def within_set_schedule():
        current_time = datetime.datetime.now().time()
        return current_time > schedule_active_start_time and current_time < schedule_active_end_time


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8080)
