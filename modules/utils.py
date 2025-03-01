import pandas as pd

def calculate_vol(val, time_frame):
    if time_frame == 'Daily':
        return val * 86.4
    if time_frame == 'Weekly':
        return val * 604.8
    if time_frame == "Monthly":
        return val * 2.628 * 10**3

settings = {
        "Temperature": {"average": 19.67, "range": (5, 15)},
        "Flow_rate": {"average": 5.03, "range": (0, 150)},
        "Purity": {"average": 79.67, "range": (60, 100)},
    }

temperature_range = settings["Temperature"]["average"]
flow_rate_range = settings["Flow_rate"]["average"]
purity_range = settings["Purity"]["average"]


def bar_data(time_frame,temp_avg,flow_avg,purity_avg):
     bar_data = pd.DataFrame({
    f"{time_frame} Average": [temp_avg, flow_avg, purity_avg],
    "Overall_Average":[temperature_range, flow_rate_range, purity_range]
    }, index=["Temperature", "Flow Rate", "Purity"])
     
     return bar_data
