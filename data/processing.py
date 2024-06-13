from scipy.interpolate import interp1d
import matplotlib.pyplot as plt
import numpy as np
import sys
import re


GEIGER_BACKGROUND_CPM = 25


AltimeterLog = "log/altimeter.log"
GeigerLog = "log/geiger.log"
AltimeterIgnoreLines = 5
CpmSlideWindow = 30


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
    with open(datafile, "r") as fp:
        lines = fp.readlines()
    time_start_ns = int(lines[1].strip())
    lines = lines[3:]
    lines = lines[AltimeterIgnoreLines:]
    lines = lines[:-1]
    _, temp_0, press_0, _ = lines[0].split()
    temp_0, press_0 = float(temp_0), float(press_0)
    timestamps, heights, temperatures = [], [], []
    for line in lines:
        t, temp, press, _ = line.split()
        t, temp, press = int(t), float(temp), float(press)
        altitude = (temp_0 + 273.15) / 0.0065 * (1.0 - pow(press / press_0, 0.1903))
        timestamps.append(time_start_ns + t)
        heights.append(altitude)
        temperatures.append(temp)
    return timestamps, heights, temperatures


def load_geiger_data(datafile):
    with open(datafile, "r") as fp:
        lines = fp.readlines()
    time_start_ns = int(lines[1].strip())
    lines = lines[3:]
    lines = lines[:-1]
    timestamps = []
    for line in lines:
        t, v = line.split()
        t, v = int(t), int(v)
        if v == 0:
            timestamps.append(time_start_ns + t)
    return timestamps


def altimeter_fit(time, heights):
    height_func = interp1d(time, heights, kind="linear", copy=False,
                    bounds_error=False, fill_value=(heights[0], heights[-1]),
                    assume_sorted=True)
    time_func = lambda x: (np.asarray(x, dtype=np.float64) - time[0]) / 1.0e9
    return height_func, time_func


def calculate_cpm(geiger_ns):
    """
    Calculate CPM (signal count per minute).
    For our geiger counter, the CPM ratio is 153.8 CPM/(μSv/h) .
    See https://wiki.dfrobot.com/SKU_SEN0463_Gravity_Geiger_Counter_Module .
    """
    time = []
    cpm = []
    for i in range(len(geiger_ns) - CpmSlideWindow + 1):
        ts = geiger_ns[i]
        te = geiger_ns[i + CpmSlideWindow - 1]
        interval_ns = te - ts
        interval_m = interval_ns / 1.0e9 / 60  # ns -> minute
        time.append((ts + te) / 2.0)
        cpm.append(CpmSlideWindow / interval_m)
    return time, cpm


if __name__ == "__main__":
    altimeter_datafile = sys.argv[1] if len(sys.argv) > 1 else get_datafile_from_log(AltimeterLog)
    geiger_datafile = sys.argv[2] if len(sys.argv) > 2 else get_datafile_from_log(GeigerLog)
    print(f"altimeter datafile: {altimeter_datafile}")
    print(f"geiger datafile: {geiger_datafile}")

    height_timestamps, heights, temperatures = load_altimeter_data(altimeter_datafile)
    height_func, time_func = altimeter_fit(height_timestamps, heights)

    geiger_timestamps = load_geiger_data(geiger_datafile)
    geiger_heights = height_func(geiger_timestamps)
    cpm_timestamps, cpm = calculate_cpm(geiger_timestamps)

    fig, ax1 = plt.subplots()
    linewidth = 0.5

    color = "tab:blue"
    ax1.set_xlabel("time (s)")
    ax1.set_ylabel("height (m)", color=color)
    ax1.plot(time_func(height_timestamps), heights, color=color, linewidth=linewidth)
    ax1.plot(time_func(geiger_timestamps), geiger_heights, '.', markersize=1.0, color="tab:red")
    ax1.tick_params(axis="y", labelcolor=color)
    # ax1.set_ylim([0, 600])

    ax2 = ax1.twinx()

    color = "tab:orange"
    ax2.set_ylabel("CPM", color=color)
    ax2.plot(time_func(cpm_timestamps), cpm, color=color, linewidth=linewidth)
    ax2.axhline(y=GEIGER_BACKGROUND_CPM, color="tab:green", linewidth=linewidth)
    ax2.tick_params(axis="y", labelcolor=color)

    fig.tight_layout()
    plt.show()
