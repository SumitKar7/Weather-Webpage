# 🌤️ Weather App

A modern, responsive weather forecasting application built with React.js that provides real-time weather data and 5-day forecasts using the OpenWeatherMap API.

##  Features

- ** Current Weather**: Real-time weather conditions for any location
- ** 5-Day Forecast**: Extended weather predictions with daily averages
- ** Location Search**: Search by city name, country, or coordinates
- ** Geolocation**: Automatic weather detection for your current location
- ** Imperial Units**: Temperature in Fahrenheit, wind in mph, visibility in miles
- ** Responsive Design**: Works perfectly on desktop, tablet, and mobile
- ** Modern UI**: Clean, intuitive interface with weather icons
- ** Real-time Data**: Live weather updates from OpenWeatherMap API

##  Live Demo

This app is running at: **http://localhost:3001**

##  Tech Stack

- **Frontend**: React.js 18.2.0
- **Styling**: CSS3 with responsive design
- **Icons**: React Icons (Weather Icons)
- **API**: OpenWeatherMap Weather API
- **Build Tool**: Create React App
- **Package Manager**: npm

## Prerequisites

- Node.js (version 14 or higher)
- npm (comes with Node.js)
- OpenWeatherMap API key (free)

##  Installation & Setup

### 1. Clone the Repository
```bash
git clone <your-repo-url>
cd WeatherApp
```

### 2. Install Dependencies
```bash
npm install
```

### 3. Get API Key
1. Visit [OpenWeatherMap](https://openweathermap.org/api)
2. Sign up for a free account
3. Get your API key from the dashboard

### 4. Configure API Key
1. Copy the example config:
   ```bash
   cp config.example.js src/config.js
   ```
2. Edit `src/config.js` and add your API key:
   ```javascript
   OPENWEATHER_API_KEY: 'your_actual_api_key_here'
   ```

### 5. Run the Application
```bash
npm start
```

The app will open automatically at **http://localhost:3000** (or another port if 3000 is busy).

## Usage

### Search for Weather
1. Type a city name in the search bar (e.g., "London", "New York", "Tokyo")
2. Press Enter or click the Search button
3. View current weather and 5-day forecast

### Use Current Location
- The app automatically detects your location on first load
- Grant location permissions when prompted
- Weather data will load automatically

### View Weather Details
- **Current Conditions**: Temperature, feels like, humidity, wind, pressure, visibility
- **Forecast**: 5-day weather predictions with daily averages
- **Units**: All data displayed in imperial units (°F, mph, miles)

## ⚙️ Configuration

Edit `src/config.js` to customize:

```javascript
export const config = {
  OPENWEATHER_API_KEY: 'your_api_key',
  DEFAULT_LOCATION: 'London',
  UNITS: 'imperial',        // 'metric', 'imperial', or 'kelvin'
  LANG: 'en',              // Language code
  API_BASE_URL: 'https://api.openweathermap.org/data/2.5',
  GEOLOCATION_TIMEOUT: 10000,
  CACHE_DURATION: 5 * 60 * 1000,
};
```

##  Project Structure

```
WeatherApp/
├── public/
│   └── index.html
├── src/
│   ├── components/
│   │   ├── CurrentWeather.js    # Current weather display
│   │   ├── Forecast.js          # 5-day forecast
│   │   └── WeatherSearch.js     # Search functionality
│   ├── services/
│   │   └── weatherService.js    # API calls and data handling
│   ├── utils/
│   │   └── weatherIcons.js      # Weather icon mapping
│   ├── config.js                # Configuration file
│   ├── App.js                   # Main application component
│   └── App.css                  # Main styles
├── package.json
└── README.md
```

##  Customization

### Change Units
- **Imperial**: `UNITS: 'imperial'` (Fahrenheit, mph, miles)
- **Metric**: `UNITS: 'metric'` (Celsius, km/h, km)
- **Kelvin**: `UNITS: 'kelvin'` (Kelvin, m/s, m)

### Change Language
Set `LANG` in config.js to any supported language code:
- English: `'en'`
- Spanish: `'es'`
- French: `'fr'`
- German: `'de'`
- And many more...

### Styling
Modify `src/App.css` to customize colors, fonts, and layout.

##  Available Scripts

- **`npm start`** - Runs the app in development mode
- **`npm run build`** - Builds the app for production
- **`npm test`** - Runs the test suite
- **`npm run eject`** - Ejects from Create React App (not recommended)

##  API Endpoints Used

- **Current Weather**: `/weather`
- **5-Day Forecast**: `/forecast`
- **Geocoding**: `/geo/1.0/direct`

##  Browser Support

- Chrome (recommended)
- Firefox
- Safari
- Edge
- Mobile browsers

##  Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

##  License

This project is open source and available under the [MIT License](LICENSE).

## Troubleshooting

### Common Issues

**"Failed to fetch weather data"**
- Check your API key in `src/config.js`
- Verify internet connection
- Ensure API key is valid and not expired

**App won't start**
- Make sure all dependencies are installed: `npm install`
- Check if port 3000 is available
- Try running on a different port: `PORT=3001 npm start`

**Weather data not loading**
- Verify API key is correct
- Check browser console for errors
- Ensure location search terms are valid

### Getting Help

- Check the browser console for error messages
- Verify your API key is working
- Ensure all dependencies are properly installed

##  Acknowledgments

- [OpenWeatherMap](https://openweathermap.org/) for providing the weather API
- [React Icons](https://react-icons.github.io/react-icons/) for weather icons
- [Create React App](https://create-react-app.dev/) for the project setup

---

**Happy Weather Forecasting! **
