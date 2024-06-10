import sys
import re


AltimeterLog = "log/altimeter.log"
GeigerLog = "log/geiger.log"


def get_datafile_from_log(logfile):
    DatafileLinePattern = re.compile(r"\s*data\s+file\s*:\s*(\S+)\s*")
    with open(logfile, "r") as fp:
        lines = fp.readlines()
        for line in lines[::-1]:
            match = DatafileLinePattern.fullmatch(line)
            if match is not None:
                return match[1]
    return None


def load_altimeter_data(datafile):
    ALTIMETER_IGNORE_LINES = 5
    with open(datafile, "r") as fp:
        lines = fp.readlines()
    time_start_ns = int(lines[1].strip())
    lines = lines[3:]
    lines = lines[ALTIMETER_IGNORE_LINES:]
    lines = lines[:-1]
    _, temp_0, press_0, _ = lines[0].split()
    temp_0, press_0 = float(temp_0), float(press_0)
    result = []
    for line in lines:
        t, temp, press, _ = line.split()
        t, temp, press = int(t), float(temp), float(press)
        altitude = (temp_0 + 273.15) / 0.0065 * (1.0 - pow(press / press_0, 0.1903))
        result.append((time_start_ns + t, altitude, temp))
    return result


def load_geiger_data(datafile):
    with open(datafile, "r") as fp:
        lines = fp.readlines()
    time_start_ns = int(lines[1].strip())
    lines = lines[3:]
    lines = lines[:-1]
    result = []
    for line in lines:
        t, v = line.split()
        t, v = int(t), int(v)
        if v == 0:
            result.append(t)
    return result


if __name__ == "__main__":
    altimeter_datafile = sys.argv[1] if len(sys.argv) > 1 else get_datafile_from_log(AltimeterLog)
    geiger_datafile = sys.argv[2] if len(sys.argv) > 2 else get_datafile_from_log(GeigerLog)
    print(f"altimeter datafile: {altimeter_datafile}")
    print(f"geiger datafile: {geiger_datafile}")
    altimeter_data = load_altimeter_data(altimeter_datafile)
    geiger_data = load_geiger_data(geiger_datafile)
    # func_height = altimeter_fit(altimeter_data)
    print()
    for d in altimeter_data:
        print(d)
    print()
    for d in geiger_data:
        print(d)
