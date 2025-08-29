// OpenWeatherMap API configuration
import { config } from '../config';

const API_KEY = config.OPENWEATHER_API_KEY;
const BASE_URL = config.API_BASE_URL;

// Helper function to handle API responses
const handleResponse = async (response) => {
  if (!response.ok) {
    if (response.status === 401) {
      throw new Error('Invalid API key. Please check your OpenWeatherMap API key.');
    } else if (response.status === 404) {
      throw new Error('Location not found. Please check the spelling and try again.');
    } else if (response.status === 429) {
      throw new Error('API rate limit exceeded. Please try again later.');
    } else {
      throw new Error(`API error: ${response.status} ${response.statusText}`);
    }
  }
  return response.json();
};

// Get current weather data
export const getWeatherData = async (location) => {
  try {
    let url;
    
    // Check if location is coordinates or city name
    if (typeof location === 'object' && location.lat && location.lon) {
      // Coordinates provided
      url = `${BASE_URL}/weather?lat=${location.lat}&lon=${location.lon}&units=${config.UNITS}&lang=${config.LANG}&appid=${API_KEY}`;
    } else if (typeof location === 'string') {
      // City name provided
      url = `${BASE_URL}/weather?q=${encodeURIComponent(location)}&units=${config.UNITS}&lang=${config.LANG}&appid=${API_KEY}`;
    } else {
      throw new Error('Invalid location format');
    }

    const response = await fetch(url);
    const data = await handleResponse(response);
    
    return data;
  } catch (error) {
    console.error('Error fetching weather data:', error);
    throw error;
  }
};

// Get 5-day forecast data
export const getForecastData = async (location) => {
  try {
    let url;
    
    // Check if location is coordinates or city name
    if (typeof location === 'object' && location.lat && location.lon) {
      // Coordinates provided
      url = `${BASE_URL}/forecast?lat=${location.lat}&lon=${location.lon}&units=${config.UNITS}&lang=${config.LANG}&appid=${API_KEY}`;
    } else if (typeof location === 'string') {
      // City name provided
      url = `${BASE_URL}/forecast?q=${encodeURIComponent(location)}&units=${config.UNITS}&lang=${config.LANG}&appid=${API_KEY}`;
    } else {
      throw new Error('Invalid location format');
    }

    const response = await fetch(url);
    const data = await handleResponse(response);
    
    return data;
  } catch (error) {
    console.error('Error fetching forecast data:', error);
    throw error;
  }
};

// Get weather data by coordinates (for geolocation)
export const getWeatherByCoords = async (lat, lon) => {
  try {
    const weatherData = await getWeatherData({ lat, lon });
    const forecastData = await getForecastData({ lat, lon });
    
    return {
      weather: weatherData,
      forecast: forecastData
    };
  } catch (error) {
    console.error('Error fetching weather by coordinates:', error);
    throw error;
  }
};

// Search for cities (autocomplete functionality)
export const searchCities = async (query) => {
  try {
    if (!query || query.length < 2) {
      return [];
    }
    
    const url = `http://api.openweathermap.org/geo/1.0/direct?q=${encodeURIComponent(query)}&limit=5&appid=${API_KEY}`;
    const response = await fetch(url);
    const data = await handleResponse(response);
    
    return data.map(city => ({
      name: city.name,
      country: city.country,
      state: city.state,
      lat: city.lat,
      lon: city.lon
    }));
  } catch (error) {
    console.error('Error searching cities:', error);
    return [];
  }
};
