# %%
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pmdarima import auto_arima

# %%
df = pd.read_csv('../calgary_water_analysis.csv')
df.head()

# %%


# %%
monthly_data = df.set_index(pd.to_datetime(df['date']))['Daily consumption per capita']
model = auto_arima(monthly_data, seasonal=True, m=12)  # yearly seasonality
forecast = model.predict(n_periods=84)  # 7 years * 12 months

# %%
print(forecast)

# %%
plt.figure(figsize=(12, 6))
plt.plot(monthly_data, label='Historical Data')
plt.plot(forecast, label='Forecast', color='red')
plt.title('Water Consumption Forecast')
plt.xlabel('Date')
plt.ylabel('Daily Consumption per Capita')
plt.legend()
plt.show()

# %%
from pandas.plotting import autocorrelation_plot
autocorrelation_plot(monthly_data)
#print(autocorrelation_plot(monthly_data))
plt.title('Autocorrelation Plot')
plt.show()

# %%
import pandas as pd
import numpy as np
from pmdarima import auto_arima
import matplotlib.pyplot as plt
from statsmodels.tsa.statespace.sarimax import SARIMAX
from statsmodels.graphics.tsaplots import plot_acf, plot_pacf

# Assuming your DataFrame `df` has columns: ['date', 'monthn', 'month', 'Daily consumption per capita']
df['date'] = pd.to_datetime(df['date'])
ts = df.set_index('date')['Daily consumption per capita'].asfreq('MS')  # Monthly start frequency
ts = ts.dropna()  # Ensure no missing values

# %%
plt.figure(figsize=(12, 6))
ts.plot(title='Monthly Water Consumption per Capita')
plt.ylabel('Liters per Capita')
plt.xlabel('Date')
plt.grid(True)
plt.show()

# %%
from statsmodels.tsa.stattools import adfuller

def test_stationarity(series):
    result = adfuller(series)
    print(f'ADF Statistic: {result[0]}')
    print(f'p-value: {result[1]}')
    if result[1] <= 0.05:
        print("Data is stationary (reject H0)")
    else:
        print("Data is non-stationary (fail to reject H0)")

test_stationarity(ts)

# %%
# First-order differencing
ts_diff = ts.diff().dropna()

# Re-test stationarity
test_stationarity(ts_diff)

# %%
plt.figure(figsize=(12, 4))
ts_diff.plot(title='First-Order Differenced Series')
plt.ylabel('Differenced Consumption')
plt.grid(True)
plt.show()

# %%
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
plot_acf(ts_diff, lags=24, ax=ax1, title='ACF of Differenced Series')
plot_pacf(ts_diff, lags=24, ax=ax2, title='PACF of Differenced Series', method='ywm')
plt.show()

# %%
from pmdarima import auto_arima

model = auto_arima(
    ts_diff,                # Use differenced series
    stationary=True,         # Already stationary (no further differencing)
    seasonal=False,          # Start with non-seasonal model
    start_p=0, max_p=3,
    start_q=0, max_q=3,
    information_criterion='aic',
    trace=True,
    stepwise=True
)
print(model.summary())

# %%
# Plot seasonal decomposition
from statsmodels.tsa.seasonal import seasonal_decompose

result = seasonal_decompose(ts, model='additive', period=12)
result.plot()
plt.show()

# %%
# Seasonal ARIMA with m=12 (monthly data)
model = auto_arima(
    ts,
    d=1,                    # First-order differencing
    seasonal=True,
    m=12,                   # Monthly seasonality
    start_p=0, max_p=2,
    start_q=0, max_q=2,
    D=1,                    # Seasonal differencing (if needed)
    trace=True
)

# %%
stepwise_model = auto_arima(ts, start_p=0, start_q=0,
                            max_p=3, max_q=3, m=12,
                            start_P=0, seasonal=True,
                            d=1, D=1, trace=True,
                            error_action='ignore',  
                            suppress_warnings=True, 
                            stepwise=True)
print(stepwise_model.summary())


# %%
from statsmodels.tsa.statespace.sarimax import SARIMAX

# Example: SARIMA(1,1,1)(1,1,1,12)
model = SARIMAX(
    ts,
    order=(0, 1, 1),                # (p,d,q)
    seasonal_order=(2, 1, 0, 12),    # (P,D,Q,m)
    trend='c'                        # Include constant
)
results = model.fit()
print(results.summary())

# %%
results.plot_diagnostics(figsize=(12, 8))
plt.show()

# %%
# Forecast 84 months (7 years)
forecast_steps = 84
forecast = results.get_forecast(steps=forecast_steps)
forecast_mean = forecast.predicted_mean
conf_int = forecast.conf_int()

# Plot results
plt.figure(figsize=(12, 6))
ts.plot(label='Historical')
forecast_mean.plot(label='Forecast', color='red')
plt.fill_between(conf_int.index, conf_int.iloc[:,0], conf_int.iloc[:,1], color='pink', alpha=0.3)
plt.title('Water Consumption Forecast (2018–2025)')
plt.xlabel('Date')
plt.ylabel('Liters per Capita')
plt.legend()
plt.grid(True)
plt.show()

