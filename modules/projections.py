import pickle
import pandas as pd
import streamlit as st
import plotly.graph_objects as go

@st.cache_resource
def load_prophet_model():
    with open("./prediction/prophet_model.pkl", "rb") as f:
        model = pickle.load(f)
    return model

def create_forecast_chart(monthly_data, forecast):
    last_hist_date = monthly_data['ds'].max()
    forecast_future = forecast[forecast['ds'] > last_hist_date]

    fig = go.Figure([
        go.Scatter(x=monthly_data['ds'], y=monthly_data['y'], mode='lines+markers', name='Historical', line=dict(color='blue')),
        go.Scatter(x=forecast_future['ds'], y=forecast_future['yhat'], mode='lines+markers', name='Forecast', line=dict(color='red')),
        go.Scatter(
            x=forecast_future['ds'].tolist() + forecast_future['ds'][::-1].tolist(),
            y=forecast_future['yhat_upper'].tolist() + forecast_future['yhat_lower'][::-1].tolist(),
            fill='toself',
            fillcolor='rgba(255, 0, 0, 0.2)',
            line=dict(color='rgba(255,255,255,0)'),
            showlegend=True,
            name='Confidence Interval'
        )
    ])
    
    fig.update_layout(title="Historical Water Usage & 2-Year Forecast",
                      xaxis_title="Date",
                      yaxis_title="Monthly Water Usage (Liters)",
                      template="plotly_white")
    return fig
