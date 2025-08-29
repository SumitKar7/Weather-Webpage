import { render, screen } from '@testing-library/react';
import App from './App';

test('renders weather app header', () => {
  render(<App />);
  const headerElement = screen.getByText(/Weather Forecast/i);
  expect(headerElement).toBeInTheDocument();
});

test('renders search form', () => {
  render(<App />);
  const searchInput = screen.getByPlaceholderText(/Enter city name/i);
  expect(searchInput).toBeInTheDocument();
});

test('renders welcome message when no weather data', () => {
  render(<App />);
  const welcomeMessage = screen.getByText(/Welcome to Weather App!/i);
  expect(welcomeMessage).toBeInTheDocument();
});
