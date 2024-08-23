import subprocess
import paho.mqtt.subscribe as sub
import paho.mqtt.publish as pub

VETRNIK_MQTT_HOST: str = "192.168.1.4"
VETRNIK_TOPIC: str = "vetrnik"
_TOPIC_TELE: str = f"{VETRNIK_TOPIC}/tele"
_TOPIC_CMND: str = f"{VETRNIK_TOPIC}/cmnd"


def vetrnik_set(cmnd: str, message: str) -> None:
    pub.single(
        f"{_TOPIC_CMND}/{cmnd}",
        message,
        hostname=VETRNIK_MQTT_HOST
    )


def vetrnik_get(topic: str) -> str:
    m = sub.simple(
        f"{_TOPIC_TELE}/{topic}",
        hostname=VETRNIK_MQTT_HOST,
        msg_count=1, retained=True
    )
    return m.payload.decode('ascii')


def vetrnik_log_start(logfile):
    f = open(logfile, "w")
    process = subprocess.Popen(
        ["mosquitto_sub", "-h", VETRNIK_MQTT_HOST, "-F", "@T.@N : %t %p", "-t", "vetrnik/#"],
        shell=False, stdout=f
    )
    return (process, f)


def vetrnik_log_stop(t: tuple):
    process, f = t
    process.terminate()
    process.wait()
    f.close()