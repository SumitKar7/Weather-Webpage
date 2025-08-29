// Example configuration file for Weather App

export const config = {
  // OpenWeatherMap API Configuration
  // Get your API key from: https://openweathermap.org/api
  OPENWEATHER_API_KEY: 'your_api_key_here',
  
  // Default location (optional)
  DEFAULT_LOCATION: 'London',
  
  // Units (metric, imperial, kelvin)
  UNITS: 'metric',
  
  // Language (en, es, fr, de, etc.)
  LANG: 'en',
  
  // API base URL
  API_BASE_URL: 'https://api.openweathermap.org/data/2.5',
  
  // Geolocation timeout (in milliseconds)
  GEOLOCATION_TIMEOUT: 10000,
  
  // Cache duration (in milliseconds) - 5 minutes
  CACHE_DURATION: 5 * 60 * 1000,
};


