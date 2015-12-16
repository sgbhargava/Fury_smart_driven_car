/*
 * gps_filter.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: Abhi
 */

/* Applying Kalman filters to GPS data. */

#include <assert.h>
#include <math.h>
#include "stdint.h"
#include "gps_filter.hpp"
#include "hashDefine.hpp"


KalmanFilter alloc_filter(int state_dimension,
              int observation_dimension) {
  KalmanFilter f;
  f.timestep = 0;
  f.state_dimension = state_dimension;
  f.observation_dimension = observation_dimension;

  f.state_transition = alloc_matrix(state_dimension,
                    state_dimension);
  f.observation_model = alloc_matrix(observation_dimension,
                     state_dimension);
  f.process_noise_covariance = alloc_matrix(state_dimension,
                        state_dimension);
  f.observation_noise_covariance = alloc_matrix(observation_dimension,
                        observation_dimension);

  f.observation = alloc_matrix(observation_dimension, 1);

  f.predicted_state = alloc_matrix(state_dimension, 1);
  f.predicted_estimate_covariance = alloc_matrix(state_dimension,
                         state_dimension);
  f.innovation = alloc_matrix(observation_dimension, 1);
  f.innovation_covariance = alloc_matrix(observation_dimension,
                     observation_dimension);
  f.inverse_innovation_covariance = alloc_matrix(observation_dimension,
                         observation_dimension);
  f.optimal_gain = alloc_matrix(state_dimension,
                observation_dimension);
  f.state_estimate = alloc_matrix(state_dimension, 1);
  f.estimate_covariance = alloc_matrix(state_dimension,
                       state_dimension);

  f.vertical_scratch = alloc_matrix(state_dimension,
                    observation_dimension);
  f.small_square_scratch = alloc_matrix(observation_dimension,
                    observation_dimension);
  f.big_square_scratch = alloc_matrix(state_dimension,
                      state_dimension);

  return f;
}

void free_filter(KalmanFilter f) {
  free_matrix(f.state_transition);
  free_matrix(f.observation_model);
  free_matrix(f.process_noise_covariance);
  free_matrix(f.observation_noise_covariance);

  free_matrix(f.observation);

  free_matrix(f.predicted_state);
  free_matrix(f.predicted_estimate_covariance);
  free_matrix(f.innovation);
  free_matrix(f.innovation_covariance);
  free_matrix(f.inverse_innovation_covariance);
  free_matrix(f.optimal_gain);
  free_matrix(f.state_estimate);
  free_matrix(f.estimate_covariance);

  free_matrix(f.vertical_scratch);
  free_matrix(f.small_square_scratch);
  free_matrix(f.big_square_scratch);
}

void update(KalmanFilter f) {
  predict(f);
  estimate(f);
}

void predict(KalmanFilter f) {
  f.timestep++;

  /* Predict the state */
  multiply_matrix(f.state_transition, f.state_estimate,
          f.predicted_state);

  /* Predict the state estimate covariance */
  multiply_matrix(f.state_transition, f.estimate_covariance,
          f.big_square_scratch);
  multiply_by_transpose_matrix(f.big_square_scratch, f.state_transition,
                   f.predicted_estimate_covariance);
  add_matrix(f.predicted_estimate_covariance, f.process_noise_covariance,
         f.predicted_estimate_covariance);
}

void estimate(KalmanFilter f) {
  /* Calculate innovation */
  multiply_matrix(f.observation_model, f.predicted_state,
          f.innovation);
  subtract_matrix(f.observation, f.innovation,
          f.innovation);

  /* Calculate innovation covariance */
  multiply_by_transpose_matrix(f.predicted_estimate_covariance,
                   f.observation_model,
                   f.vertical_scratch);
  multiply_matrix(f.observation_model, f.vertical_scratch,
          f.innovation_covariance);
  add_matrix(f.innovation_covariance, f.observation_noise_covariance,
         f.innovation_covariance);

  /* Invert the innovation covariance.
     Note: this destroys the innovation covariance.
     TODO: handle inversion failure intelligently. */
  destructive_invert_matrix(f.innovation_covariance,
                f.inverse_innovation_covariance);

  /* Calculate the optimal Kalman gain.
     Note we still have a useful partial product in vertical scratch
     from the innovation covariance. */
  multiply_matrix(f.vertical_scratch, f.inverse_innovation_covariance,
          f.optimal_gain);

  /* Estimate the state */
  multiply_matrix(f.optimal_gain, f.innovation,
          f.state_estimate);
  add_matrix(f.state_estimate, f.predicted_state,
         f.state_estimate);

  /* Estimate the state covariance */
  multiply_matrix(f.optimal_gain, f.observation_model,
          f.big_square_scratch);
  subtract_from_identity_matrix(f.big_square_scratch);
  multiply_matrix(f.big_square_scratch, f.predicted_estimate_covariance,
          f.estimate_covariance);
}


KalmanFilter alloc_filter_velocity2d(double_t noise) {
  /* The state model has four dimensions:
     x, y, x', y'
     Each time step we can only observe position, not velocity, so the
     observation vector has only two dimensions.
  */
  KalmanFilter f = alloc_filter(4, 2);

  /* Assuming the axes are rectilinear does not work well at the
     poles, but it has the bonus that we don't need to convert between
     lat/long and more rectangular coordinates. The slight inaccuracy
     of our physics model is not too important.
   */
  set_identity_matrix(f.state_transition);
  set_seconds_per_timestep(f, 1.0);

  /* We observe (x, y) in each time step */
  set_matrix(f.observation_model,
         1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0);

  /* Noise in the world. */
  double_t pos = 0.000001;
  set_matrix(f.process_noise_covariance,
         pos, 0.0, 0.0, 0.0,
         0.0, pos, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 1.0);

  /* Noise in our observation */
  set_matrix(f.observation_noise_covariance,
         pos * noise, 0.0,
         0.0, pos * noise);

  /* The start position is totally unknown, so give a high variance */
  set_matrix(f.state_estimate, 0.0, 0.0, 0.0, 0.0);
  set_identity_matrix(f.estimate_covariance);
  double_t trillion = 1000.0 * 1000.0 * 1000.0 * 1000.0;
  scale_matrix(f.estimate_covariance, trillion);

  return f;
}


/* The position units are in thousandths of latitude and longitude.
   The velocity units are in thousandths of position units per second.

   So if there is one second per timestep, a velocity of 1 will change
   the lat or long by 1 after a million timesteps.

   Thus a typical position is hundreds of thousands of units.
   A typical velocity is maybe ten.
*/
void set_seconds_per_timestep(KalmanFilter f,
                  double_t seconds_per_timestep) {
  /* unit_scaler accounts for the relation between position and
     velocity units */
  double_t unit_scaler = 0.001;
  f.state_transition.data[0][2] = unit_scaler * seconds_per_timestep;
  f.state_transition.data[1][3] = unit_scaler * seconds_per_timestep;
}

void update_velocity2d(KalmanFilter f, double_t lat, double_t lon,
               double_t seconds_since_last_timestep) {
  set_seconds_per_timestep(f, seconds_since_last_timestep);
  set_matrix(f.observation, lat * 1000.0, lon * 1000.0);
  update(f);
}


void get_lat_long(KalmanFilter f, double_t* lat, double_t* lon) {
  *lat = f.state_estimate.data[0][0] / 1000.0;
  *lon = f.state_estimate.data[1][0] / 1000.0;
}


void get_velocity(KalmanFilter f, double_t* delta_lat, double_t* delta_lon) {
  *delta_lat = f.state_estimate.data[2][0] / (1000.0 * 1000.0);
  *delta_lon = f.state_estimate.data[3][0] / (1000.0 * 1000.0);
}

double_t calculate_metersPerSecond(double_t lat, double_t lon,
             double_t delta_lat, double_t delta_lon) {
  /* First, let's calculate a unit-independent measurement - the radii
     of the earth traveled in each second. (Presumably this will be
     a very small number.) */

  /* Convert to radians */
  lat *= TO_RAD;
  lon *= TO_RAD;
  delta_lat *= TO_RAD;
  delta_lon *= TO_RAD;

  /* Haversine formula */
  double_t lat1 = lat - delta_lat;
  double_t sin_half_dlat = sin(delta_lat / 2.0);
  double_t sin_half_dlon = sin(delta_lon / 2.0);
  double_t a = sin_half_dlat * sin_half_dlat + cos(lat1) * cos(lat)
    * sin_half_dlon * sin_half_dlon;
  double_t radians_per_second = 2 * atan2(1000.0 * sqrt(a),
                    1000.0 * sqrt(1.0 - a));

  /* Convert units */
  double_t meters_per_second = radians_per_second * RADIUS;
  return meters_per_second;
}

double_t get_metersPerSecond(KalmanFilter f) {
  double_t lat, lon, delta_lat, delta_lon;
  get_lat_long(f, &lat, &lon);
  get_velocity(f, &delta_lat, &delta_lon);
  return calculate_metersPerSecond(lat, lon, delta_lat, delta_lon);
}

