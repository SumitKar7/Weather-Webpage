import React, { useState, useEffect } from 'react';
import WeatherSearch from './components/WeatherSearch';
import CurrentWeather from './components/CurrentWeather';
import Forecast from './components/Forecast';
import { getWeatherData, getForecastData } from './services/weatherService';
import './App.css';

function App() {
  const [currentWeather, setCurrentWeather] = useState(null);
  const [forecast, setForecast] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);


  // Get user's current location on app load
  useEffect(() => {
    if (navigator.geolocation) {
      navigator.geolocation.getCurrentPosition(
        (position) => {
          const { latitude, longitude } = position.coords;
          fetchWeatherByCoords(latitude, longitude);
        },
        (error) => {
          console.log('Geolocation error:', error);
        }
      );
    }
  }, []);

  const fetchWeatherByCoords = async (lat, lon) => {
    setLoading(true);
    setError(null);
    try {
      const weatherData = await getWeatherData(lat, lon);
      const forecastData = await getForecastData(lat, lon);
      
      setCurrentWeather(weatherData);
      setForecast(forecastData);

    } catch (err) {
      setError('Failed to fetch weather data. Please try again.');
      console.error('Weather fetch error:', err);
    } finally {
      setLoading(false);
    }
  };

  const handleSearch = async (searchLocation) => {
    if (!searchLocation.trim()) return;
    
    setLoading(true);
    setError(null);
    try {
      const weatherData = await getWeatherData(searchLocation);
      const forecastData = await getForecastData(searchLocation);
      
      setCurrentWeather(weatherData);
      setForecast(forecastData);

    } catch (err) {
      setError('Location not found. Please check the spelling and try again.');
      console.error('Search error:', err);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="App">
      <div className="container">
        <header className="app-header">
          <h1>🌤️ Weather Forecast</h1>
          <p>Get real-time weather updates and 5-day forecasts</p>
        </header>

        <WeatherSearch onSearch={handleSearch} />

        {error && <div className="error">{error}</div>}

        {loading && <div className="loading">Loading weather data...</div>}

        {currentWeather && !loading && (
          <CurrentWeather weather={currentWeather} />
        )}

        {forecast && !loading && (
          <Forecast forecast={forecast} />
        )}

        {!currentWeather && !loading && !error && (
          <div className="card">
            <h2>Welcome to Weather App!</h2>
            <p>Search for a city or use your current location to get started.</p>
          </div>
        )}
      </div>
    </div>
  );
}

export default App;
