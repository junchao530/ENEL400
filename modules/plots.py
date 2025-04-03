import pandas as pd
import streamlit as st
import numpy as np
from modules.utils import calculate_vol, calculate_average_col
from modules.postgresDataLoader import load_calgary_data, load_water_usage
import calendar 

@st.cache_data
def return_data():
    return load_water_usage()

def plots(title, y_axis, time, data, y):
    st.subheader(title)
    chart_data = pd.DataFrame({'time': time, y_axis: data[y]}).set_index('time')
    st.line_chart(chart_data)
    st.markdown("<br><br>", unsafe_allow_html=True)


def cost_analysis(date, time_frame, flow_avg):
    date = pd.Timestamp(date)
    # Get the new data, which should have at least 'timestamp' and 'individual_consumption' columns.
    df = return_data()
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    if time_frame == "Daily":
        # Use normalize() to get midnight and add one day minus a microsecond for the period end
        period_start = date.normalize()
        period_end = period_start + pd.Timedelta(days=1) - pd.Timedelta(microseconds=1)
        period_label = date.strftime("%B %d, %Y")
        period_hours = 24
    elif time_frame == "Weekly":
        # Compute start of week by subtracting weekday days and normalizing to midnight
        period_start = (date - pd.Timedelta(days=date.weekday())).normalize()
        period_end = period_start + pd.Timedelta(days=7) - pd.Timedelta(microseconds=1)
        period_label = f"Week of {period_start.strftime('%B %d, %Y')}"
        period_hours = 7 * 24
    elif time_frame == "Monthly":
        # Use to_period to determine the first moment of the month and the end of the month
        period_start = date.to_period("M").to_timestamp()
        period_end = (date.to_period("M") + 1).to_timestamp() - pd.Timedelta(microseconds=1)
        period_label = date.strftime("%B %Y")
        # Use the days_in_month attribute from the Period object for total hours
        period_hours = date.to_period("M").days_in_month * 24

    selected_df = df[(df['timestamp'] >= period_start) & (df['timestamp'] <= period_end)]

    average_volume = selected_df['individual_consumption'].mean() * period_hours

    current_volume = flow_avg * period_hours

    cost_factor = 0.0023173
    current_cost = current_volume * cost_factor
    average_cost = average_volume * cost_factor

    st.subheader(f"Cost Analysis for {period_label}")

    col1, col2, col3, col4 = st.columns(4)
    
    with col1:
        st.metric(label=f"{time_frame.capitalize()} Volume (L)", 
                  value=f"{current_volume:.2f}", 
                  delta=f"{current_volume - average_volume:.2f} L")
    with col2:
        st.metric(label="Average Volume (L)", value=f"{average_volume:.2f}")
    with col3:
        st.metric(label=f"{time_frame.capitalize()} Cost ($)", 
                  value=f"${current_cost:.4f}", 
                  delta=f"${current_cost - average_cost:.4f}")
    with col4:
        st.metric(label="Average Cost ($)", value=f"${average_cost:.4f}")
def calgary_water_usage():
    df = load_calgary_data()
    st.line_chart(df.set_index('date')['Daily consumption per capita'])