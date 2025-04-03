import pickle
import pandas as pd
import streamlit as st
import plotly.graph_objects as go
import statsmodels.api as sm
import numpy as np
import matplotlib.pyplot as plt
from modules.postgresDataLoader import load_water_usage

@st.cache_data
def return_data():
    return load_water_usage()


def forecast_period(granularity='daily', current_time=None):
    data = return_data()
    data['timestamp'] = pd.to_datetime(data['timestamp'], format="%Y-%m-%d %H:%M:%S")
    with open("./ml_workflows/ols_model.pkl", "rb") as f:
        model = pickle.load(f)

    if current_time is None:
        current_time = pd.Timestamp.now()
    if granularity == 'Daily':
        period_start = current_time.normalize() 
        period_end = period_start + pd.Timedelta(days=1) - pd.Timedelta(seconds=1)
    elif granularity == 'Monthly':
        period_start = current_time.replace(day=1)
        next_month = period_start + pd.offsets.MonthBegin(1)
        period_end = next_month - pd.Timedelta(seconds=1)
    elif granularity == 'Yearly':
        period_start = pd.Timestamp(year=current_time.year, month=1, day=1)
        period_end = pd.Timestamp(year=current_time.year, month=12, day=31, hour=23, minute=59, second=59)
    else:
        raise ValueError("Invalid granularity. Choose 'daily', 'monthly', or 'yearly'.")
    period_data = data[(data['timestamp'] >= period_start) & (data['timestamp'] <= period_end)].copy()
    historical = period_data[period_data['timestamp'] <= current_time].copy()
    future = period_data[period_data['timestamp'] > current_time].copy()
    future['avg_lag1'] = future['water_usage'].shift(1)
    future['avg_lead1'] = future['water_usage'].shift(-1)
    future = future.dropna()  
    X_future = future[['water_usage', 'avg_lag1', 'avg_lead1']]
    X_future = sm.add_constant(X_future)
    future['forecast'] = model.predict(X_future)
    historical['forecast'] = np.nan
    combined = pd.concat([historical, future]).sort_values('timestamp')
    return combined, period_start, period_end

def fore_cast_projection(result, period_start, period_end, time_frame):
    df = return_data()
    df['timestamp'] = pd.to_datetime(df['timestamp'], format="%Y-%m-%d %H:%M:%S")
    df_period = df[(df['timestamp'] >= period_start) & (df['timestamp'] <= period_end)]

    if time_frame == 'Daily':
        pass
    elif time_frame == 'Monthly':
        df_period = df_period.groupby(pd.Grouper(key='timestamp', freq='D')).agg({
            'water_usage': 'sum',
            'individual_consumption': 'sum'
        }).reset_index()
        result = result.groupby(pd.Grouper(key='timestamp', freq='D'))['forecast'].sum().reset_index()
    elif time_frame == 'Yearly':
        df_period = df_period.groupby(pd.Grouper(key='timestamp', freq='D')).agg({
            'water_usage': 'sum',
            'individual_consumption': 'sum'
        }).reset_index()
        df_period = df_period.groupby(pd.Grouper(key='timestamp', freq='M')).agg({
            'water_usage': 'mean',
            'individual_consumption': 'mean'
        }).reset_index()
        result = result.groupby(pd.Grouper(key='timestamp', freq='D'))['forecast'].sum().reset_index()
        result = result.groupby(pd.Grouper(key='timestamp', freq='M'))['forecast'].mean().reset_index()
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.plot(df_period['timestamp'], df_period['water_usage'], label="Average Consumption", alpha=0.5, color='green')
    ax.plot(df_period['timestamp'], df_period['individual_consumption'], label="Individual Consumption", color='blue')
    ax.plot(result['timestamp'], result['forecast'], label="Forecasted Individual Consumption", linestyle="--", color='red')
    #ax.axvline(current_time, color='black', linestyle='--', label='Current Time')
    ax.set_xlabel("Timestamp")
    ax.set_ylabel("Water Consumption")
    ax.set_title(f"Forecasting Individual Consumption ({period_start.date()} to {period_end.date()})")
    ax.legend()
    ax.set_xlim(period_start, period_end)
    st.pyplot(fig)

