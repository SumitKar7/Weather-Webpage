// Weather icon mapping based on OpenWeatherMap icon codes
// https://openweathermap.org/weather-conditions

export const getWeatherIcon = (iconCode) => {
  const iconMap = {
    // Clear sky
    '01d': '☀️', // clear sky day
    '01n': '🌙', // clear sky night
    
    // Few clouds
    '02d': '⛅', // few clouds day
    '02n': '☁️', // few clouds night
    
    // Scattered clouds
    '03d': '☁️', // scattered clouds
    '03n': '☁️',
    
    // Broken clouds
    '04d': '☁️', // broken clouds
    '04n': '☁️',
    
    // Shower rain
    '09d': '🌦️', // shower rain
    '09n': '🌧️',
    
    // Rain
    '10d': '🌧️', // rain day
    '10n': '🌧️', // rain night
    
    // Thunderstorm
    '11d': '⛈️', // thunderstorm day
    '11n': '⛈️', // thunderstorm night
    
    // Snow
    '13d': '❄️', // snow day
    '13n': '❄️', // snow night
    
    // Mist/Fog
    '50d': '🌫️', // mist day
    '50n': '🌫️', // mist night
  };

  return iconMap[iconCode] || '🌤️'; // Default icon
};

// Get weather description with emoji
export const getWeatherDescription = (description, iconCode) => {
  const icon = getWeatherIcon(iconCode);
  return `${icon} ${description}`;
};

// Get temperature with appropriate emoji
export const getTemperatureWithEmoji = (temp) => {
  if (temp >= 30) return `🔥 ${temp}°C`;
  if (temp >= 20) return `😎 ${temp}°C`;
  if (temp >= 10) return `😊 ${temp}°C`;
  if (temp >= 0) return `😐 ${temp}°C`;
  if (temp >= -10) return `🥶 ${temp}°C`;
  return `❄️ ${temp}°C`;
};

// Get wind speed with emoji
export const getWindSpeedWithEmoji = (speed) => {
  if (speed < 5) return `🍃 ${speed} km/h`;
  if (speed < 15) return `💨 ${speed} km/h`;
  if (speed < 30) return `🌪️ ${speed} km/h`;
  return `🌀 ${speed} km/h`;
};

// Get humidity with emoji
export const getHumidityWithEmoji = (humidity) => {
  if (humidity < 30) return `🏜️ ${humidity}%`;
  if (humidity < 60) return `🌤️ ${humidity}%`;
  return `�� ${humidity}%`;
};
