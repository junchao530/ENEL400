import pandas as pd
import streamlit as st
import numpy as np
from modules.utils import calculate_vol, calculate_average_col
from modules.dataLoader import load_calgary_data
import calendar 

def plots(title, y_axis, time, data, y):
    st.subheader(title)
    chart_data = pd.DataFrame({'time': time, y_axis: data[y]}).set_index('time')
    st.line_chart(chart_data)
    st.markdown("<br><br>", unsafe_allow_html=True)


def cost_analysis(date, time_frame, flow_avg):
    df = load_calgary_data()
    selected_month = date.month
    daily_average_month = df.loc[selected_month, 'Daily consumption per capita']
    average_volume = calculate_average_col(daily_average_month, time_frame)

    st.subheader(f"Cost Analysis for the month of {calendar.month_name[selected_month]}")
    current_volume = calculate_vol(flow_avg, time_frame)
    current_cost = current_volume* 0.0023173
    average_cost = average_volume* 0.0023173
    col3, col4, col5, col6 = st.columns(4)
    st.header
    with col3:
        st.metric(label=f"{time_frame} Volume (L)", value=f"{current_volume:.2f}", delta=f"{current_volume - average_volume:.2f} L")

    with col4:
        st.metric(label="Average Volume (L)", value=f"{average_volume:.2f}")

    with col5:
        st.metric(label=f"{time_frame} Cost ($)", value=f"${current_cost:.4f}", delta=f"${current_cost - average_cost:.4f}")

    with col6:
        st.metric(label="Average Cost ($)", value=f"${average_cost:.4f}")
