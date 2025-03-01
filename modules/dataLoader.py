import pandas as pd
import numpy as np
import streamlit as st

@st.cache_data
def load_historical_data():
    df_daily = pd.read_csv("daily_aggregated.csv", parse_dates=["timestamp"])
    df_daily.set_index("timestamp", inplace=True)
    monthly_data = df_daily.resample("M").agg({"daily_liters_sum": "sum"})
    monthly_data.reset_index(inplace=True)
    monthly_data.rename(columns={"timestamp": "ds", "daily_liters_sum": "y"}, inplace=True)
    return monthly_data

def load_data():
    try:
        data = pd.read_csv("1_year_data.csv")
        data['timestamp'] = pd.to_datetime(data['timestamp'], format="%Y-%m-%d %H:%M:%S")
        return data
    except FileNotFoundError:
        st.error("File '1_year_data.csv' not found.")
        return pd.DataFrame()

def aggregate_data(data, date, type):
    if type == "Daily":
        daily_data = data[data['timestamp'].dt.date == date]
        numeric_columns = daily_data.select_dtypes(include=[np.number]).columns
        return daily_data.groupby(pd.Grouper(key='timestamp', freq='10min'))[numeric_columns].mean().reset_index()
    
    elif type == "Weekly":
        start_of_week = date - pd.Timedelta(days=date.weekday()) 
        end_of_week = start_of_week + pd.Timedelta(days=6)  
      
        weekly_data = data[(data['timestamp'].dt.date >= start_of_week) & (data['timestamp'].dt.date <= end_of_week)]
 
        numeric_columns = weekly_data.select_dtypes(include=[np.number]).columns
        return weekly_data.groupby(pd.Grouper(key='timestamp', freq='H'))[numeric_columns].mean().reset_index()
    
    elif type == "Monthly":
        
        monthly_data = data[(data['timestamp'].dt.year == date.year) & (data['timestamp'].dt.month == date.month)]
        
        numeric_columns = monthly_data.select_dtypes(include=[np.number]).columns
    
        return monthly_data.groupby(pd.Grouper(key='timestamp', freq='12H'))[numeric_columns].mean().reset_index()
