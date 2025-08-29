import React from 'react';
import { 
  WiHumidity, 
  WiStrongWind, 
  WiBarometer
} from 'react-icons/wi';

import { getWeatherIcon } from '../utils/weatherIcons';

const CurrentWeather = ({ weather }) => {
  const {
    name,
    main: { temp, feels_like, humidity, pressure },
    weather: weatherInfo,
    wind: { speed },
    visibility,
    sys: { country }
  } = weather;

  const weatherDescription = weatherInfo[0]?.description || 'Unknown';
  const weatherIcon = weatherInfo[0]?.icon || '01d';

  const formatTemperature = (temp) => {
    return Math.round(temp);
  };

  const formatWindSpeed = (speed) => {
    return Math.round(speed * 2.237); // Convert m/s to mph
  };

  const formatVisibility = (visibility) => {
    return (visibility / 1609.34).toFixed(1); // Convert m to miles
  };

  return (
    <div className="card weather-card">
      <div className="weather-header">
        <h2>{name}, {country}</h2>
        <p className="weather-description">{weatherDescription}</p>
      </div>

      <div className="weather-main">
        <div className="weather-icon">
          {getWeatherIcon(weatherIcon)}
        </div>
        <div className="temperature">
          {formatTemperature(temp)}°F
        </div>
        <p className="feels-like">
          Feels like {formatTemperature(feels_like)}°F
        </p>
      </div>

      <div className="weather-details">
        <div className="detail-item">
          <WiHumidity className="detail-icon" />
          <span>Humidity: {humidity}%</span>
        </div>
        
        <div className="detail-item">
          <WiStrongWind className="detail-icon" />
          <span>Wind: {formatWindSpeed(speed)} mph</span>
        </div>
        
        <div className="detail-item">
          <WiBarometer className="detail-icon" />
          <span>Pressure: {pressure} hPa</span>
        </div>
        
        <div className="detail-item">
          <span>Visibility: {formatVisibility(visibility)} miles</span>
        </div>
      </div>

      <div className="weather-timestamp">
        <small>Last updated: {new Date().toLocaleTimeString()}</small>
      </div>
    </div>
  );
};

export default CurrentWeather;
