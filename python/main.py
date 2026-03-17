from arduino.app_utils import App, Bridge
import time

x = 0

def loop():
    global x
    Bridge.call("update", x)
    x += 1
    if x > 50:
        x = 0
    time.sleep(0.02)

App.run(user_loop=loop)
