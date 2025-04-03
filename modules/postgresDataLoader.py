import psycopg2 
import pandas as pd
import streamlit as st
import numpy as np

def init_db():
    try:
        conn = psycopg2.connect(host="localhost", dbname="waterstuff", user="oop", password="ucalgary", port=5432)
        return conn
    except psycopg2.Error as e:
        st.error(f"Database connection error: {e}")
        return None

def load_data():
    conn = init_db()
    if conn is None:
        return pd.DataFrame()
    try:
        query = "SELECT * FROM water_usage;"
        data = pd.read_sql(query, conn)
        data['timestamp'] = pd.to_datetime(data['timestamp'], format="%Y-%m-%d %H:%M:%S")  # Ensure datetime format
        return data
    except psycopg2.Error as e:
        st.error(f"Database query error: {e}")
        return pd.DataFrame()
    finally:
        conn.close() 

def insert_data(timestamp, flow_rate, temperature, purity):
    conn = init_db()
    if conn is None:
        return False
    try:
        cur = conn.cursor()
        insert_query = """
        INSERT INTO water_usage (timestamp, flow_rate, temperature, purity)
        VALUES (%s, %s, %s, %s);
        """
        cur.execute(insert_query, (timestamp, flow_rate, temperature, purity))
        conn.commit()
        return True
    except psycopg2.Error as e:
        st.error(f"Insert error: {e}")
        return False
    finally:
        cur.close()
        conn.close()


def load_calgary_data():
    conn = init_db()
    if conn is None:
        return pd.DataFrame()
    try:
        query = "SELECT * FROM calgary_water_consumption;"
        data = pd.read_sql(query, conn)
        data['date'] = pd.to_datetime(data['date'], format="%Y-%m-%d")  # Ensure datetime format
        return data
    except psycopg2.Error as e:
        st.error(f"Database query error: {e}")
        return pd.DataFrame()
    finally:
        conn.close()

def load_water_usage():
    conn = init_db()
    if conn is None:
        return pd.DataFrame()
    try:
        query = "SELECT * FROM water_usage_data WHERE EXTRACT(YEAR FROM timestamp) >= 2024;"
        data = pd.read_sql(query, conn)
        data['timestamp'] = pd.to_datetime(data['timestamp'], format="%Y-%m-%d %h:%m:s")  # Ensure datetime format
        return data
    except psycopg2.Error as e:
        st.error(f"Database query error: {e}")
        return pd.DataFrame()
    finally:
        conn.close()  

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
        return weekly_data.groupby(pd.Grouper(key='timestamp', freq='h'))[numeric_columns].mean().reset_index()
    
    elif type == "Monthly":
        
        monthly_data = data[(data['timestamp'].dt.year == date.year) & (data['timestamp'].dt.month == date.month)]
        
        numeric_columns = monthly_data.select_dtypes(include=[np.number]).columns
    
        return monthly_data.groupby(pd.Grouper(key='timestamp', freq='12h'))[numeric_columns].mean().reset_index()
