import pandas as pd
import streamlit as st
from modules.utils import calculate_vol

def plots(title, y_axis, time, data, y):
    st.subheader(title)
    chart_data = pd.DataFrame({'time': time, y_axis: data[y]}).set_index('time')
    st.line_chart(chart_data)
    st.markdown("<br><br>", unsafe_allow_html=True)


def cost_analysis(time_frame, flow_avg, flow_rate_range  ):
    st.subheader("Cost Analysis")
    current_volume = calculate_vol(flow_avg, time_frame)
    average_volume = calculate_vol(flow_rate_range, time_frame)
    current_cost = current_volume* 0.0023173
    average_cost = average_volume* 0.0023173
    col3, col4, col5, col6 = st.columns(4)

    with col3:
        st.metric(label=f"{time_frame} Volume (L)", value=f"{current_volume:.2f}", delta=f"{current_volume - average_volume:.2f} L")

    with col4:
        st.metric(label="Average Volume (L)", value=f"{average_volume:.2f}")

    with col5:
        st.metric(label=f"{time_frame} Cost ($)", value=f"${current_cost:.4f}", delta=f"${current_cost - average_cost:.4f}")

    with col6:
        st.metric(label="Average Cost ($)", value=f"${average_cost:.4f}")
