import streamlit as st
from modules.postgresDataLoader import load_data, aggregate_data, load_calgary_data, insert_data
from modules.projections import forecast_period, fore_cast_projection
from modules.plots import plots, cost_analysis, calgary_water_usage
from modules.utils import calculate_vol, bar_data
from modules.hardwareConnections import usb_init, read_from_bluetooth, parse_data_packet, Real_Time
import pandas as pd
import time
from datetime import datetime, timedelta

st.set_page_config(page_title="HydroMIND Dashboard", layout="wide")


# Function for continuous data collection



# Run data collection on every page refresh

# Sidebar Controls
with st.sidebar:
    st.title("HydroMIND Dashboard")
    st.header("⚙️ Settings")

    page_selection = st.selectbox("Select a Page", ("Real-Time Monitoring", "Historical", "Forecast"))

    df = load_data()
    max_date = df['timestamp'].max().date()
    default_start_date = max_date - timedelta(days=365)

#Real_Time()
if page_selection == "Historical" and not df.empty:
    date = st.sidebar.date_input("Select Date", max_date)
    time_frame = st.sidebar.radio("Time Frame", ["Daily", "Weekly", "Monthly"])
    df_filtered = aggregate_data(df, date, time_frame)
    flow_avg = df_filtered["flow_rate"].mean()

    st.title("HydroMIND Water Usage Dashboard")
    col1, col2 = st.columns(2)

    with col1:
        plots("Flow Rate Over Time", "Litre/Minute", df_filtered['timestamp'], df_filtered, 'flow_rate')
        plots("Temperature Trends", "°C", df_filtered['timestamp'], df_filtered, 'temperature')

    with col2:
        plots("Turbidity", "Percent %", df_filtered['timestamp'], df_filtered, 'purity')
        total_volume = calculate_vol(df_filtered['flow_rate'].mean(), time_frame)
        st.subheader("Historical Insights")
        st.bar_chart(bar_data(time_frame, df_filtered["temperature"].mean(), df_filtered["flow_rate"].mean(), df_filtered["purity"].mean()))

    cost_analysis(date, time_frame, flow_avg)

elif page_selection == "Forecast":
    time_frame = st.sidebar.radio("Time Frame", ["Daily", "Monthly", "Yearly"])
    st.title("Water Usage Forecast")
    result, period_start, period_end = forecast_period(granularity=time_frame, current_time=None)
    fore_cast_projection(result, period_start, period_end, time_frame)
    st.subheader("Forecast Insights")
    

    

# Real-Time Monitoring always runs
elif page_selection == "Real-Time Monitoring":
    Real_Time() 
    
# st.experimental_rerun()  # Forces UI to refresh
# time.sleep(2)  # Simulate continuous collection every 2 seconds
