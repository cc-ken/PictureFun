
import os
import sys

def parseCommandLine(argv):
    options = {
        "arch": "x64",
        "os": "Macos"
    }
    for arg in argv:
        if arg in ["Macos", "Android", "Windows", "iOS"]:
            options["os"] = arg
        elif arg in ["x64", "armv8", "arm64"]:
            options["arch"] = arg

    return options

if __name__ == '__main__':
    options = parseCommandLine(sys.argv)
