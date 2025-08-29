import React, { useState } from 'react';
import { WiRefresh } from 'react-icons/wi';

const WeatherSearch = ({ onSearch }) => {
  const [searchTerm, setSearchTerm] = useState('');

  const handleSubmit = (e) => {
    e.preventDefault();
    if (searchTerm.trim()) {
      onSearch(searchTerm.trim());
      setSearchTerm('');
    }
  };

  const handleKeyPress = (e) => {
    if (e.key === 'Enter') {
      handleSubmit(e);
    }
  };

  return (
    <div className="card">
      <form onSubmit={handleSubmit} className="search-form">
        <div className="search-container">
          <input
            type="text"
            className="input search-input"
            placeholder="Enter city name (e.g., London, New York, Tokyo)"
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            onKeyPress={handleKeyPress}
          />
          <button type="submit" className="btn search-btn">
            <WiRefresh />
            Search
          </button>
        </div>
        <p className="search-hint">
          💡 Tip: You can search by city name, country, or coordinates
        </p>
      </form>
    </div>
  );
};

export default WeatherSearch;
