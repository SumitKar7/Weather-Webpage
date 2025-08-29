import React from 'react';
import { getWeatherIcon } from '../utils/weatherIcons';

const Forecast = ({ forecast }) => {
  // Group forecast data by day and get daily averages
  const dailyForecast = forecast.list.reduce((acc, item) => {
    const date = new Date(item.dt * 1000);
    const day = date.toDateString();
    
    if (!acc[day]) {
      acc[day] = {
        date: date,
        temps: [],
        descriptions: [],
        icons: [],
        humidity: [],
        wind: []
      };
    }
    
    acc[day].temps.push(item.main.temp);
    acc[day].descriptions.push(item.weather[0]?.description);
    acc[day].icons.push(item.weather[0]?.icon);
    acc[day].humidity.push(item.main.humidity);
    acc[day].wind.push(item.wind.speed);
    
    return acc;
  }, {});

  const formatDate = (date) => {
    const today = new Date();
    const tomorrow = new Date(today);
    tomorrow.setDate(tomorrow.getDate() + 1);
    
    if (date.toDateString() === today.toDateString()) {
      return 'Today';
    } else if (date.toDateString() === tomorrow.toDateString()) {
      return 'Tomorrow';
    } else {
      return date.toLocaleDateString('en-US', { weekday: 'short', month: 'short', day: 'numeric' });
    }
  };

  const getAverageTemp = (temps) => {
    return Math.round(temps.reduce((sum, temp) => sum + temp, 0) / temps.length);
  };

  const getMostFrequent = (array) => {
    const counts = {};
    let maxCount = 0;
    let mostFrequent = array[0];
    
    array.forEach(item => {
      counts[item] = (counts[item] || 0) + 1;
      if (counts[item] > maxCount) {
        maxCount = counts[item];
        mostFrequent = item;
      }
    });
    
    return mostFrequent;
  };

  const getAverageHumidity = (humidity) => {
    return Math.round(humidity.reduce((sum, h) => sum + h, 0) / humidity.length);
  };

  const getAverageWind = (wind) => {
    return Math.round(wind.reduce((sum, w) => sum + w, 0) / wind.length * 2.237); // Convert to mph
  };

  return (
    <div className="card">
      <h2>5-Day Forecast</h2>
      <div className="forecast-grid">
        {Object.values(dailyForecast).slice(0, 5).map((day, index) => (
          <div key={index} className="forecast-day">
            <div className="forecast-date">
              {formatDate(day.date)}
            </div>
            
            <div className="forecast-icon">
              {getWeatherIcon(getMostFrequent(day.icons))}
            </div>
            
            <div className="forecast-temp">
              {getAverageTemp(day.temps)}°F
            </div>
            
            <div className="forecast-desc">
              {getMostFrequent(day.descriptions)}
            </div>
            
            <div className="forecast-details">
              <small>💧 {getAverageHumidity(day.humidity)}%</small>
              <br />
              <small>💨 {getAverageWind(day.wind)} mph</small>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
};

export default Forecast;
