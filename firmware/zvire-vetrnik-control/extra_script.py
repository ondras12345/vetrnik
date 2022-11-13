Import("env")
import os.path

# see ~/.platformio/platforms/ststm32/builder/main.py
platform = env.PioPlatform()
board = env.BoardConfig()
hwids = board.get("build.hwids", [["0x0483", "0xDF11"]])
vid = hwids[0][0]
pid = hwids[0][1]

env.AddPostAction(
    os.path.join("$BUILD_DIR", "${PROGNAME}.bin"),
    env.VerboseAction(
        " ".join([
            '"%s"' % os.path.join(platform.get_package_dir("tool-dfuutil") or "",
                 "bin", "dfu-suffix"),
            "-v %s" % vid,
            "-p %s" % pid,
            "-d 0xffff", "-a", "$TARGET"
        ]), "Adding dfu suffix to ${PROGNAME}.bin"))
