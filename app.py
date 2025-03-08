import streamlit as st
from modules.dataLoader import load_data, aggregate_data, load_historical_data
from modules.projections import load_prophet_model, create_forecast_chart
from modules.plots import plots, cost_analysis
from modules.utils import calculate_vol, bar_data
from modules.hardwareConnections import usb_init, read_from_bluetooth, parse_data_packet
import pandas as pd
import time
from datetime import datetime, timedelta

st.set_page_config(page_title="HydroMIND Dashboard", layout="wide")



df = load_data()

with st.sidebar:
    st.title("HydroMIND Dashboard")
    st.header("⚙️ Settings")
    max_date = df['timestamp'].max().date()
    default_start_date = max_date - timedelta(days=365)  # Show a year by default

    page_selection = st.selectbox("Select a Page", ("Real-Time Monitoring", "Historical", "Forecast"))

    start_date = default_start_date
    time_frame = "Daily"
    df_filtered = aggregate_data(df, start_date, time_frame)
    

if page_selection == "Historical" and not df.empty:
    max_date = df['timestamp'].max().date()
    default_start_date = max_date - timedelta(days=365)  # Show a year by default


    date = st.sidebar.date_input("Select Date", max_date)
    time_frame = st.sidebar.radio("Time Frame", ["Daily", "Weekly", "Monthly"])
    df_filtered = aggregate_data(df, date, time_frame)
    flow_avg = df_filtered["flow_rate"].mean()
    
    
    st.title("HydroMIND water Usage Dashboard")
    col1, col2 = st.columns(2)

    with col1:
        plots("Flow Rate Over Time", "Litre/Minute", df_filtered['timestamp'], df_filtered, 'flow_rate')
        plots("Temperature Trends", "°C", df_filtered['timestamp'], df_filtered, 'temperature')

    with col2:
        plots("Turbidity", "Percent %", df_filtered['timestamp'], df_filtered, 'purity')
        total_volume = calculate_vol(df_filtered['flow_rate'].mean(), time_frame)
        st.subheader("Historical Insights")
        st.bar_chart(bar_data(time_frame, df_filtered["temperature"].mean(),df_filtered["flow_rate"].mean(),df_filtered["purity"].mean() ))
    cost_analysis(date, time_frame, flow_avg)

elif page_selection == "Forecast":
   
    st.title("Water Usage Forecast")
    historical_data = load_historical_data()
    model = load_prophet_model()
    forecast = model.predict(historical_data)
    forecast_chart = create_forecast_chart(historical_data, forecast)
    st.plotly_chart(forecast_chart)

elif page_selection == "Real-Time Monitoring":
        # Real-Time Data Section
        st.title("Live Water Usage Monitoring")
        st.warning("Ensure the sensor is connected to USB before starting.")
        
        connection = usb_init()
        if connection != -1:
            st.success("USB Sensor Connected!")
            real_time_chart = st.empty()
            collected_data = []

            while True:
                raw_packet = read_from_bluetooth(connection)
                if raw_packet:
                    parsed_data = parse_data_packet(raw_packet)
                    collected_data.append(parsed_data)

                    df_live = pd.DataFrame(collected_data, columns=["timestamp", "flow", "temperature", "turbidity"])
                    df_live.set_index("timestamp", inplace=True)

                    real_time_chart.line_chart(df_live[["flow", "temperature", "turbidity"]])

                    if len(collected_data) > 50:
                        collected_data.pop(0)

                time.sleep(2)
        else:
            st.error("Failed to connect to USB Sensor.")
